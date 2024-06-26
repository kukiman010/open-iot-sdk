/*
 * Copyright (c) 2018-2022 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "netsocket/TLSSocketWrapper.h"
#include <new>

#if defined(MBEDTLS_USE_PSA_CRYPTO)
#include "psa/crypto.h"
#endif

// This class requires Mbed TLS SSL/TLS client code
#if defined(MBEDTLS_SSL_CLI_C)

TLSSocketWrapper::TLSSocketWrapper(TCPSocket *transport, const char *hostname, control_transport control)
    : _transport(transport), _connect_transport(control == TRANSPORT_CONNECT || control == TRANSPORT_CONNECT_AND_CLOSE),
      _close_transport(control == TRANSPORT_CLOSE || control == TRANSPORT_CONNECT_AND_CLOSE), _tls_initialized(false),
      _handshake_completed(false), _cacert_allocated(false), _clicert_allocated(false), _ssl_conf_allocated(false)
{
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    // It is safe to call psa_crypto_init() any number of times as
    // defined by the PSA Crypto API. There is no standard "deinit"
    // function.
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        PW_LOG_ERROR("psa_crypto_init() failed ( %i )", status);
    }
#endif /* MBEDTLS_USE_PSA_CRYPTO */

#if defined(MBEDTLS_PLATFORM_C)
    int ret = mbedtls_platform_setup(nullptr);
    if (ret != 0) {
        PW_LOG_ERROR("mbedtls_platform_setup() returned %i", ret);
    }
#endif /* MBEDTLS_PLATFORM_C */
    memset(&_entropy, 0x0, sizeof(_entropy));
    mbedtls_entropy_init(&_entropy);
    DRBG_INIT(&_drbg);

    mbedtls_ssl_init(&_ssl);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_pk_init(&_pkctx);
#endif

    if (hostname) {
        set_hostname(hostname);
    }
}

TLSSocketWrapper::~TLSSocketWrapper()
{
    if (_transport) {
        if (_handshake_completed) {
            _transport->set_blocking(true);
            mbedtls_ssl_close_notify(&_ssl);
            _handshake_completed = false;
        }

        if (_close_transport) {
            _transport->close();
        }
        _transport = nullptr;
    }

    mbedtls_entropy_free(&_entropy);

    DRBG_FREE(&_drbg);

    mbedtls_ssl_free(&_ssl);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_pk_free(&_pkctx);
    set_own_cert(nullptr);
    set_ca_chain(nullptr);
#endif
    set_ssl_config(nullptr);
#if defined(MBEDTLS_PLATFORM_C)
    mbedtls_platform_teardown(nullptr);
#endif /* MBEDTLS_PLATFORM_C */
}

void TLSSocketWrapper::set_hostname(const char *hostname)
{
#if defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_X509_REMOVE_HOSTNAME_VERIFICATION)
    mbedtls_ssl_set_hostname(&_ssl, hostname);
#endif
}

nsapi_error_t TLSSocketWrapper::set_root_ca_cert(const void *root_ca, size_t len)
{
#if !defined(MBEDTLS_X509_CRT_PARSE_C)
    return NSAPI_ERROR_UNSUPPORTED;
#else
    mbedtls_x509_crt *crt;

    crt = new (std::nothrow) mbedtls_x509_crt;
    if (!crt) {
        return NSAPI_ERROR_NO_MEMORY;
    }

    mbedtls_x509_crt_init(crt);

    /* Parse CA certification */
    int ret;
    if ((ret = mbedtls_x509_crt_parse(crt, static_cast<const unsigned char *>(root_ca), len)) != 0) {
        PW_LOG_ERROR("mbedtls_x509_crt_parse returned %i", ret);
        mbedtls_x509_crt_free(crt);
        delete crt;
        return NSAPI_ERROR_PARAMETER;
    }
    set_ca_chain(crt);
    _cacert_allocated = true;
    return NSAPI_ERROR_OK;
#endif
}

nsapi_error_t TLSSocketWrapper::set_root_ca_cert(const char *root_ca_pem)
{
    return set_root_ca_cert(root_ca_pem, strlen(root_ca_pem) + 1);
}

nsapi_error_t TLSSocketWrapper::set_client_cert_key(const char *client_cert_pem, const char *client_private_key_pem)
{
    return set_client_cert_key(
        client_cert_pem, strlen(client_cert_pem) + 1, client_private_key_pem, strlen(client_private_key_pem) + 1);
}

nsapi_error_t TLSSocketWrapper::set_client_cert_key(const void *client_cert,
                                                    size_t client_cert_len,
                                                    const void *client_private_key_pem,
                                                    size_t client_private_key_len)
{
#if !defined(MBEDTLS_X509_CRT_PARSE_C) || !defined(MBEDTLS_PK_C)
    return NSAPI_ERROR_UNSUPPORTED;
#else

    int ret;
    mbedtls_x509_crt *crt = new (std::nothrow) mbedtls_x509_crt;
    if (!crt) {
        return NSAPI_ERROR_NO_MEMORY;
    }
    mbedtls_x509_crt_init(crt);
    if ((ret = mbedtls_x509_crt_parse(crt, static_cast<const unsigned char *>(client_cert), client_cert_len)) != 0) {
        PW_LOG_ERROR("mbedtls_x509_crt_parse returned %i", ret);
        mbedtls_x509_crt_free(crt);
        delete crt;
        return NSAPI_ERROR_PARAMETER;
    }
    mbedtls_pk_init(&_pkctx);
    if ((ret = mbedtls_pk_parse_key(
             &_pkctx, static_cast<const unsigned char *>(client_private_key_pem), client_private_key_len, nullptr, 0))
        != 0) {
        PW_LOG_ERROR("mbedtls_pk_parse_key returned %i", ret);
        mbedtls_x509_crt_free(crt);
        delete crt;
        return NSAPI_ERROR_PARAMETER;
    }
    set_own_cert(crt);
    _clicert_allocated = true;

    return NSAPI_ERROR_OK;
#endif /* MBEDTLS_X509_CRT_PARSE_C */
}

nsapi_error_t TLSSocketWrapper::start_handshake(bool first_call)
{
    const unsigned char DRBG_PERS[] = "mbed TLS client";
    int ret;

    if (!_transport) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    if (_tls_initialized) {
        return continue_handshake();
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_X509_REMOVE_HOSTNAME_VERIFICATION)
    PW_LOG_INFO("Starting TLS handshake with %s", _ssl.hostname);
#else
    PW_LOG_INFO("Starting TLS handshake");
#endif
    /*
     * Initialize TLS-related stuf.
     */
#if defined(MBEDTLS_CTR_DRBG_C)
    mbedtls_ctr_drbg_init(&_drbg);
    if ((ret = mbedtls_ctr_drbg_seed(&_drbg, mbedtls_entropy_func, &_entropy, DRBG_PERS, sizeof(DRBG_PERS))) != 0) {
        PW_LOG_ERROR("mbedtls_ctr_drbg_seed returned %i", ret);
        return NSAPI_ERROR_AUTH_FAILURE;
    }
#elif defined(MBEDTLS_HMAC_DRBG_C)
    if ((ret = mbedtls_hmac_drbg_seed(&_drbg,
                                      mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                                      mbedtls_entropy_func,
                                      &_entropy,
                                      DRBG_PERS,
                                      sizeof(DRBG_PERS)))
        != 0) {
        PW_LOG_ERROR("mbedtls_hmac_drbg_seed returned %i", ret);
        return NSAPI_ERROR_AUTH_FAILURE;
    }
#else
#error "CTR or HMAC must be defined for TLSSocketWrapper!"
#endif

    mbedtls_ssl_conf_rng(get_ssl_config(), DRBG_RANDOM, &_drbg);

#if MBED_CONF_TLS_SOCKET_DEBUG_LEVEL > 0
    mbedtls_ssl_conf_verify(get_ssl_config(), my_verify, nullptr);
    mbedtls_ssl_conf_dbg(get_ssl_config(), my_debug, nullptr);
    mbedtls_debug_set_threshold(MBED_CONF_TLS_SOCKET_DEBUG_LEVEL);
#endif

    PW_LOG_DEBUG("mbedtls_ssl_setup()");
    if ((ret = mbedtls_ssl_setup(&_ssl, get_ssl_config())) != 0) {
        PW_LOG_ERROR("mbedtls_ssl_setup returned %i", ret);
        return NSAPI_ERROR_AUTH_FAILURE;
    }

    _transport->set_blocking(false);

    mbedtls_ssl_set_bio(&_ssl, this, ssl_send, ssl_recv, nullptr);

    _tls_initialized = true;

    ret = continue_handshake();
    if (first_call) {
        if (ret == NSAPI_ERROR_ALREADY) {
            ret = NSAPI_ERROR_IN_PROGRESS; // If first call should return IN_PROGRESS
        }
        if (ret == NSAPI_ERROR_IS_CONNECTED) {
            ret = NSAPI_ERROR_OK; // If we happened to complete the request on the first call, return OK.
        }
    }
    return ret;
}

nsapi_error_t TLSSocketWrapper::continue_handshake()
{
    int ret;

    if (_handshake_completed) {
        return NSAPI_ERROR_IS_CONNECTED;
    }

    if (!_tls_initialized) {
        return NSAPI_ERROR_NO_CONNECTION;
    }

    ret = mbedtls_ssl_handshake(&_ssl);

    if (ret < 0) {
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            return NSAPI_ERROR_ALREADY;
        } else {
            PW_LOG_ERROR("mbedtls_ssl_handshake returned %i", ret);
            return NSAPI_ERROR_AUTH_FAILURE;
        }
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_X509_REMOVE_HOSTNAME_VERIFICATION)
    /* It also means the handshake is done, time to print info */
    PW_LOG_INFO("TLS connection to %s established", _ssl.hostname);
#else
    PW_LOG_INFO("TLS connection established");
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(FEA_TRACE_SUPPORT) && !defined(MBEDTLS_X509_REMOVE_INFO)
    /* Prints the server certificate and verify it. */
    const size_t buf_size = 1024;
    char *buf = new (std::nothrow) char[buf_size];
    if (!buf) {
        PW_LOG_ERROR("new (std::nothrow) char[buf_size] failed in continue_handshake");
        return NSAPI_ERROR_NO_MEMORY;
    }
    mbedtls_x509_crt_info(buf, buf_size, "\r    ", mbedtls_ssl_get_peer_cert(&_ssl));
    PW_LOG_DEBUG("Server certificate:\r\n%s\r\n", buf);

    uint32_t flags = mbedtls_ssl_get_verify_result(&_ssl);
    if (flags != 0) {
        /* Verification failed. */
        mbedtls_x509_crt_verify_info(buf, buf_size, "\r  ! ", flags);
        PW_LOG_ERROR("Certificate verification failed:\r\n%s", buf);
    } else {
        /* Verification succeeded. */
        PW_LOG_INFO("Certificate verification passed");
    }
    delete[] buf;
#endif

    _handshake_completed = true;
    return NSAPI_ERROR_IS_CONNECTED;
}

nsapi_error_t TLSSocketWrapper::send(const void *data, nsapi_size_t size)
{
    int ret;

    if (!_transport) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    _transport->set_blocking(0);

    PW_LOG_DEBUG("send %u", size);

    if (!_handshake_completed) {
        ret = continue_handshake();
        if (ret != NSAPI_ERROR_IS_CONNECTED) {
            if (ret == NSAPI_ERROR_ALREADY) {
                ret = NSAPI_ERROR_WOULD_BLOCK;
            }
            return ret;
        }
    }

    ret = mbedtls_ssl_write(&_ssl, static_cast<const unsigned char *>(data), size);

    if (ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ) {
        // translate to socket error
        return NSAPI_ERROR_WOULD_BLOCK;
    }

    if (ret < 0) {
        PW_LOG_ERROR("mbedtls_ssl_write returned %i", ret);
        return NSAPI_ERROR_DEVICE_ERROR;
    }
    return ret; // Assume "non negative errorcode" to be propagated from Socket layer
}

nsapi_size_or_error_t TLSSocketWrapper::sendto(const SocketAddress &, const void *data, nsapi_size_t size)
{
    // Ignore the SocketAddress
    return send(data, size);
}

nsapi_size_or_error_t TLSSocketWrapper::recv(void *data, nsapi_size_t size)
{
    int ret;

    if (!_transport) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    _transport->set_blocking(0);

    if (!_handshake_completed) {
        ret = continue_handshake();
        if (ret != NSAPI_ERROR_IS_CONNECTED) {
            if (ret == NSAPI_ERROR_ALREADY) {
                ret = NSAPI_ERROR_WOULD_BLOCK;
            }
            return ret;
        }
    }

    ret = mbedtls_ssl_read(&_ssl, static_cast<unsigned char *>(data), size);

    if (ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ) {
        // translate to socket error
        return NSAPI_ERROR_WOULD_BLOCK;
    } else if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
        /* MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY is not considered as error.
         * Just ignore here. Once connection is closed, mbedtls_ssl_read()
         * will return 0.
         */
        return 0;
    } else if (ret < 0) {
        PW_LOG_ERROR("mbedtls_ssl_read returned %i", ret);
        //  There is no mapping of TLS error codes to Socket API so return most generic error to application
        return NSAPI_ERROR_DEVICE_ERROR;
    }
    return ret;
}

nsapi_size_or_error_t TLSSocketWrapper::recvfrom(SocketAddress *address, void *data, nsapi_size_t size)
{
    if (address) {
        getpeername(address);
    }
    return recv(data, size);
}

#if MBED_CONF_TLS_SOCKET_DEBUG_LEVEL > 0

void TLSSocketWrapper::my_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    const char *p, *basename;
    (void)ctx;

    /* Extract basename from file */
    for (p = basename = file; *p != '\0'; p++) {
        if (*p == '/' || *p == '\\') {
            basename = p + 1;
        }
    }

    PW_LOG_DEBUG("%s:%04d: |%d| %s", basename, line, level, str);
}

int TLSSocketWrapper::my_verify(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    const uint32_t buf_size = 1024;
    char *buf = new char[buf_size];
    (void)data;

    PW_LOG_DEBUG("\nVerifying certificate at depth %d:\n", depth);
    mbedtls_x509_crt_info(buf, buf_size - 1, "  ", crt);
    PW_LOG_DEBUG("%s", buf);

    if (*flags == 0) {
        PW_LOG_INFO("No verification issue for this certificate\n");
    } else {
        mbedtls_x509_crt_verify_info(buf, buf_size, "  ! ", *flags);
        PW_LOG_INFO("%s\n", buf);
    }

    delete[] buf;

    return 0;
}

#endif /* MBED_CONF_TLS_SOCKET_DEBUG_LEVEL > 0 */

int TLSSocketWrapper::ssl_recv(void *ctx, unsigned char *buf, size_t len)
{
    int ret;

    TLSSocketWrapper *my = static_cast<TLSSocketWrapper *>(ctx);

    if (!my->_transport) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    ret = my->_transport->recv(buf, len);

    if (NSAPI_ERROR_WOULD_BLOCK == ret) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    } else if (ret < 0) {
        PW_LOG_ERROR("Socket recv error %d", ret);
    }
    // Propagate also Socket errors to SSL, it allows negative error codes to be returned here.
    return ret;
}

int TLSSocketWrapper::ssl_send(void *ctx, const unsigned char *buf, size_t len)
{
    int size = -1;
    TLSSocketWrapper *my = static_cast<TLSSocketWrapper *>(ctx);

    if (!my->_transport) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    size = my->_transport->send(buf, len);

    if (NSAPI_ERROR_WOULD_BLOCK == size) {
        return MBEDTLS_ERR_SSL_WANT_WRITE;
    } else if (size < 0) {
        PW_LOG_ERROR("Socket send error %d", size);
    }
    // Propagate also Socket errors to SSL, it allows negative error codes to be returned here.
    return size;
}

#if defined(MBEDTLS_X509_CRT_PARSE_C)

mbedtls_x509_crt *TLSSocketWrapper::get_own_cert()
{
    return _clicert;
}

int TLSSocketWrapper::set_own_cert(mbedtls_x509_crt *crt)
{
    int ret = 0;
    if (_clicert && _clicert_allocated) {
        mbedtls_x509_crt_free(_clicert);
        delete _clicert;
        _clicert_allocated = false;
    }
    _clicert = crt;
    if (crt) {
        if ((ret = mbedtls_ssl_conf_own_cert(get_ssl_config(), _clicert, &_pkctx)) != 0) {
            PW_LOG_ERROR("mbedtls_ssl_conf_own_cert returned %i", ret);
        }
    }
    return ret;
}

mbedtls_x509_crt *TLSSocketWrapper::get_ca_chain()
{
    return _cacert;
}

void TLSSocketWrapper::set_ca_chain(mbedtls_x509_crt *crt)
{
    if (_cacert && _cacert_allocated) {
        mbedtls_x509_crt_free(_cacert);
        delete _cacert;
        _cacert_allocated = false;
    }
    _cacert = crt;
    PW_LOG_DEBUG("mbedtls_ssl_conf_ca_chain()");
    mbedtls_ssl_conf_ca_chain(get_ssl_config(), _cacert, nullptr);
}

#endif /* MBEDTLS_X509_CRT_PARSE_C */

mbedtls_ssl_config *TLSSocketWrapper::get_ssl_config()
{
    if (!_ssl_conf) {
        _ssl_conf = new (std::nothrow) mbedtls_ssl_config;
        if (!_ssl_conf) {
            return nullptr;
        }
        mbedtls_ssl_config_init(_ssl_conf);
        _ssl_conf_allocated = true;
        int ret;

        if ((ret = mbedtls_ssl_config_defaults(
                 _ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT))
            != 0) {
            PW_LOG_ERROR("mbedtls_ssl_config_defaults returned %i", ret);
            set_ssl_config(nullptr);
            return nullptr;
        }
        /* It is possible to disable authentication by passing
         * MBEDTLS_SSL_VERIFY_NONE in the call to mbedtls_ssl_conf_authmode()
         */
        mbedtls_ssl_conf_authmode(get_ssl_config(), MBEDTLS_SSL_VERIFY_REQUIRED);
    }
    return _ssl_conf;
}

void TLSSocketWrapper::set_ssl_config(mbedtls_ssl_config *conf)
{
    if (_ssl_conf && _ssl_conf_allocated) {
        mbedtls_ssl_config_free(_ssl_conf);
        delete _ssl_conf;
        _ssl_conf_allocated = false;
    }
    _ssl_conf = conf;
}

mbedtls_ssl_context *TLSSocketWrapper::get_ssl_context()
{
    return &_ssl;
}

nsapi_error_t TLSSocketWrapper::close()
{
    if (!_transport) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    PW_LOG_INFO("Closing TLS");

    int ret = 0;
    if (_handshake_completed) {
        _transport->set_blocking(true);
        ret = mbedtls_ssl_close_notify(&_ssl);
        if (ret) {
            PW_LOG_ERROR("mbedtls_ssl_close_notify returned %i", ret);
        }
        _handshake_completed = false;
    }

    if (_close_transport) {
        int ret2 = _transport->close();
        if (!ret) {
            ret = ret2;
        }
    }

    _transport = nullptr;

    return ret;
}

nsapi_error_t TLSSocketWrapper::connect(const SocketAddress &address)
{
    nsapi_error_t ret = NSAPI_ERROR_OK;
    if (!_transport) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    if (!is_handshake_started() && _connect_transport) {
        ret = _transport->connect(address);
        if (ret && ret != NSAPI_ERROR_IS_CONNECTED) {
            return ret;
        }
    }
    return start_handshake(ret == NSAPI_ERROR_OK);
}

nsapi_error_t TLSSocketWrapper::bind(const SocketAddress &address)
{
    if (!_transport) {
        return NSAPI_ERROR_NO_SOCKET;
    }
    return _transport->bind(address);
}

void TLSSocketWrapper::set_blocking(bool blocking)
{
    set_timeout(blocking ? -1 : 0);
}

void TLSSocketWrapper::set_timeout(int timeout)
{
    if (!is_handshake_started() && timeout != -1 && _connect_transport) {
        // If we have not yet connected the transport, we need to modify its blocking mode as well.
        // After connection is initiated, it is already set to non blocking mode
        _transport->set_timeout(timeout);
    }
}

nsapi_error_t TLSSocketWrapper::setsockopt(int level, int optname, const void *optval, unsigned optlen)
{
    if (!_transport) {
        return NSAPI_ERROR_NO_SOCKET;
    }
    return _transport->setsockopt(level, optname, optval, optlen);
}

nsapi_error_t TLSSocketWrapper::getsockopt(int level, int optname, void *optval, unsigned *optlen)
{
    if (!_transport) {
        return NSAPI_ERROR_NO_SOCKET;
    }
    return _transport->getsockopt(level, optname, optval, optlen);
}

Socket *TLSSocketWrapper::accept(nsapi_error_t *err)
{
    if (err) {
        *err = NSAPI_ERROR_UNSUPPORTED;
    }
    return nullptr;
}

nsapi_error_t TLSSocketWrapper::listen(int)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

bool TLSSocketWrapper::is_handshake_started() const
{
    return _tls_initialized;
}

nsapi_error_t TLSSocketWrapper::getpeername(SocketAddress *address)
{
    if (!_handshake_completed) {
        return NSAPI_ERROR_NO_CONNECTION;
    }
    return _transport->getpeername(address);
}

#endif /* MBEDTLS_SSL_CLI_C */
