/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fff.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/hmac_drbg.h"
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"

DEFINE_FAKE_VALUE_FUNC(int, mbedtls_ssl_setup, mbedtls_ssl_context *, const mbedtls_ssl_config *);
DEFINE_FAKE_VALUE_FUNC(int, mbedtls_ssl_set_hostname, mbedtls_ssl_context *, const char *);
DEFINE_FAKE_VALUE_FUNC(int, mbedtls_ssl_conf_own_cert, mbedtls_ssl_config *, mbedtls_x509_crt *, mbedtls_pk_context *);
DEFINE_FAKE_VALUE_FUNC(int, mbedtls_ssl_handshake, mbedtls_ssl_context *);
DEFINE_FAKE_VALUE_FUNC(uint32_t, mbedtls_ssl_get_verify_result, const mbedtls_ssl_context *);
DEFINE_FAKE_VALUE_FUNC(const mbedtls_x509_crt *, mbedtls_ssl_get_peer_cert, const mbedtls_ssl_context *);
DEFINE_FAKE_VALUE_FUNC(int, mbedtls_ssl_write, mbedtls_ssl_context *, const unsigned char *, size_t);
DEFINE_FAKE_VALUE_FUNC(int, mbedtls_ssl_read, mbedtls_ssl_context *, unsigned char *, size_t);
DEFINE_FAKE_VALUE_FUNC(int, mbedtls_ssl_config_defaults, mbedtls_ssl_config *, int, int, int);
DEFINE_FAKE_VALUE_FUNC(int, mbedtls_ssl_close_notify, mbedtls_ssl_context *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_free, mbedtls_ssl_context *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_init, mbedtls_ssl_context *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_conf_rng, mbedtls_ssl_config *, f_rng, void *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_conf_verify, mbedtls_ssl_config *, int *, void *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_conf_ca_chain, mbedtls_ssl_config *, mbedtls_x509_crt *, mbedtls_x509_crl *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_conf_dbg, mbedtls_ssl_config *, void *, void *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_set_bio,
                      mbedtls_ssl_context *,
                      void *,
                      mbedtls_ssl_send_t *,
                      mbedtls_ssl_recv_t *,
                      mbedtls_ssl_recv_timeout_t *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_set_bio_ctx, mbedtls_ssl_context *, void *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_config_init, mbedtls_ssl_config *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_conf_authmode, mbedtls_ssl_config *, int);
DEFINE_FAKE_VOID_FUNC(mbedtls_ssl_config_free, mbedtls_ssl_config *);

DEFINE_FAKE_VALUE_FUNC(int, mbedtls_x509_crt_parse, mbedtls_x509_crt *, const unsigned char *, size_t);
DEFINE_FAKE_VALUE_FUNC(int, mbedtls_x509_crt_info, char *, size_t, const char *, const mbedtls_x509_crt *);
DEFINE_FAKE_VALUE_FUNC(int, mbedtls_x509_crt_verify_info, char *, size_t, const char *, uint32_t);
DEFINE_FAKE_VOID_FUNC(mbedtls_x509_crt_init, mbedtls_x509_crt *);
DEFINE_FAKE_VOID_FUNC(mbedtls_x509_crt_free, mbedtls_x509_crt *);

DEFINE_FAKE_VALUE_FUNC(int,
                       mbedtls_hmac_drbg_seed,
                       mbedtls_hmac_drbg_context *,
                       const mbedtls_md_info_t *,
                       int *,
                       void *,
                       const unsigned char *,
                       size_t);

DEFINE_FAKE_VOID_FUNC(mbedtls_pk_init, mbedtls_pk_context *);
DEFINE_FAKE_VOID_FUNC(mbedtls_pk_free, mbedtls_pk_context *);

#if (MBEDTLS_VERSION_MAJOR >= 3)
DEFINE_FAKE_VALUE_FUNC(int,
                       mbedtls_pk_parse_key,
                       mbedtls_pk_context *,
                       const unsigned char *,
                       size_t,
                       const unsigned char *,
                       size_t,
                       int *,
                       void *);
#else
DEFINE_FAKE_VALUE_FUNC(
    int, mbedtls_pk_parse_key, mbedtls_pk_context *, const unsigned char *, size_t, const unsigned char *, size_t);
#endif

DEFINE_FAKE_VOID_FUNC(mbedtls_debug_set_threshold, int);
DEFINE_FAKE_VOID_FUNC(mbedtls_strerror, int, char *, size_t);

// platform

DEFINE_FAKE_VALUE_FUNC(int, mbedtls_platform_setup, mbedtls_platform_context *);

DEFINE_FAKE_VOID_FUNC(mbedtls_platform_teardown, mbedtls_platform_context *);

// entropy

DEFINE_FAKE_VALUE_FUNC(int, mbedtls_entropy_func, void *, unsigned char *, size_t);

DEFINE_FAKE_VOID_FUNC(mbedtls_entropy_init, mbedtls_entropy_context *);
DEFINE_FAKE_VOID_FUNC(mbedtls_entropy_free, mbedtls_entropy_context *);

// ctr_drbg

DEFINE_FAKE_VALUE_FUNC(
    int, mbedtls_ctr_drbg_seed, mbedtls_ctr_drbg_context *, func_type, void *, const unsigned char *, size_t);
DEFINE_FAKE_VALUE_FUNC(int, mbedtls_ctr_drbg_random, void *, unsigned char *, size_t);

DEFINE_FAKE_VOID_FUNC(mbedtls_ctr_drbg_init, mbedtls_ctr_drbg_context *);
DEFINE_FAKE_VOID_FUNC(mbedtls_ctr_drbg_free, mbedtls_ctr_drbg_context *);
