/*
 * Copyright (c) 2018, Arm Limited and affiliates
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

#include "gtest/gtest.h"
#include "mbedtls/platform.h"
#include "netsocket/TCPSocket.h"
#include "netsocket/TLSSocketWrapper.h"
#include <cstring> //memset

#include "mbed_error.h"

static uint32_t exampleValidSocketId = 1234;

mbed_error_status_t mbed_error(mbed_error_status_t error_status,
                               const char *error_msg,
                               unsigned int error_value,
                               const char *filename,
                               int line_number)
{
    return 0;
}

class TLSSocketWrapperFixture : public testing::Test {
public:
    unsigned int dataSize = 10;
    char dataBuf[10];

protected:
    TCPSocket transport;
    TLSSocketWrapper *wrapper;

    virtual void SetUp()
    {
        wrapper = new TLSSocketWrapper(&transport);
        // CMSIS
        RESET_FAKE(osMutexNew);
        RESET_FAKE(osMutexAcquire);
        RESET_FAKE(osMutexRelease);
        RESET_FAKE(osEventFlagsWait);
        RESET_FAKE(osEventFlagsSet);

        // IoT_Socket
        RESET_FAKE(iotSocketCreate);
        RESET_FAKE(iotSocketBind);
        RESET_FAKE(iotSocketListen);
        RESET_FAKE(iotSocketAccept);
        RESET_FAKE(iotSocketConnect);
        RESET_FAKE(iotSocketRecv);
        RESET_FAKE(iotSocketRecvFrom);
        RESET_FAKE(iotSocketSend);
        RESET_FAKE(iotSocketSendTo);
        RESET_FAKE(iotSocketGetSockName);
        RESET_FAKE(iotSocketGetPeerName);
        RESET_FAKE(iotSocketGetOpt);
        RESET_FAKE(iotSocketSetOpt);
        RESET_FAKE(iotSocketClose);
        RESET_FAKE(iotSocketGetHostByName);

        // Mbedtls
        RESET_FAKE(mbedtls_ssl_setup);
        RESET_FAKE(mbedtls_ssl_set_hostname);
        RESET_FAKE(mbedtls_ssl_conf_own_cert);
        RESET_FAKE(mbedtls_ssl_handshake);
        RESET_FAKE(mbedtls_ssl_get_verify_result);
        RESET_FAKE(mbedtls_ssl_get_peer_cert);
        RESET_FAKE(mbedtls_ssl_write);
        RESET_FAKE(mbedtls_ssl_read);
        RESET_FAKE(mbedtls_ssl_config_defaults);
        RESET_FAKE(mbedtls_ssl_close_notify);
        RESET_FAKE(mbedtls_ssl_free);
        RESET_FAKE(mbedtls_ssl_init);
        RESET_FAKE(mbedtls_ssl_conf_rng);
        RESET_FAKE(mbedtls_ssl_conf_verify);
        RESET_FAKE(mbedtls_ssl_conf_ca_chain);
        RESET_FAKE(mbedtls_ssl_conf_dbg);
        RESET_FAKE(mbedtls_ssl_set_bio);
        RESET_FAKE(mbedtls_ssl_set_bio_ctx);
        RESET_FAKE(mbedtls_ssl_config_init);
        RESET_FAKE(mbedtls_ssl_conf_authmode);
        RESET_FAKE(mbedtls_ssl_config_free);

        RESET_FAKE(mbedtls_platform_setup);
        RESET_FAKE(mbedtls_platform_teardown);

        RESET_FAKE(mbedtls_entropy_free);
        RESET_FAKE(mbedtls_entropy_init);

        RESET_FAKE(mbedtls_ctr_drbg_init);
        RESET_FAKE(mbedtls_ctr_drbg_seed);
        RESET_FAKE(mbedtls_ctr_drbg_random);
        RESET_FAKE(mbedtls_ctr_drbg_free);

        RESET_FAKE(mbedtls_x509_crt_parse);
        RESET_FAKE(mbedtls_x509_crt_info);
        RESET_FAKE(mbedtls_x509_crt_verify_info);
        RESET_FAKE(mbedtls_x509_crt_init);
        RESET_FAKE(mbedtls_x509_crt_free);

        RESET_FAKE(mbedtls_hmac_drbg_seed);
        RESET_FAKE(mbedtls_pk_init);
        RESET_FAKE(mbedtls_pk_free);
        RESET_FAKE(mbedtls_pk_parse_key);
        RESET_FAKE(mbedtls_debug_set_threshold);
        RESET_FAKE(mbedtls_strerror);
    }

    virtual void TearDown()
    {
        delete wrapper;
    }

    const char *cert = "-----BEGIN CERTIFICATE-----\
                MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\
                MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\
                DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\
                SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\
                GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\
                AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\
                q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\
                SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\
                Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\
                a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\
                /PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\
                AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\
                CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\
                bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\
                c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\
                VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\
                ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\
                MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\
                Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\
                AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\
                uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\
                wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\
                X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\
                PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\
                KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\
                -----END CERTIFICATE-----";
};

static bool callback_is_called;
static void my_callback()
{
    callback_is_called = true;
}

TEST_F(TLSSocketWrapperFixture, no_socket)
{
    TLSSocketWrapper *wrapperTmp = new TLSSocketWrapper(NULL);
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapperTmp->connect(a), NSAPI_ERROR_NO_SOCKET);
    EXPECT_EQ(wrapperTmp->bind(a), NSAPI_ERROR_NO_SOCKET);
    EXPECT_EQ(wrapperTmp->setsockopt(0, 0, 0, 0), NSAPI_ERROR_NO_SOCKET);
    EXPECT_EQ(wrapperTmp->getsockopt(0, 0, 0, 0), NSAPI_ERROR_NO_SOCKET);
    EXPECT_EQ(wrapperTmp->send(dataBuf, dataSize), NSAPI_ERROR_NO_SOCKET);
    EXPECT_EQ(wrapperTmp->recv(dataBuf, dataSize), NSAPI_ERROR_NO_SOCKET);
    EXPECT_EQ(wrapperTmp->close(), NSAPI_ERROR_NO_SOCKET);
    delete wrapperTmp;
}

/* connect */

TEST_F(TLSSocketWrapperFixture, connect)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_IS_CONNECTED);
}

/* connect: TCP-related errors */

TEST_F(TLSSocketWrapperFixture, connect_no_open)
{
    iotSocketConnect_fake.return_val = 0;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TLSSocketWrapperFixture, connect_error_in_progress_no_timeout)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();

    iotSocketConnect_fake.return_val = IOT_SOCKET_EINPROGRESS;
    const SocketAddress a("127.0.0.1", 1024);
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_IN_PROGRESS);
}

TEST_F(TLSSocketWrapperFixture, connect_with_timeout)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();

    iotSocketConnect_fake.return_val = IOT_SOCKET_EINPROGRESS;
    const SocketAddress a("127.0.0.1", 1024);
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    wrapper->set_blocking(true);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_IN_PROGRESS);
}

TEST_F(TLSSocketWrapperFixture, connect_error_is_connected)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();

    iotSocketConnect_fake.return_val = IOT_SOCKET_EISCONN;
    const SocketAddress a("127.0.0.1", 1024);
    wrapper->set_timeout(1);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_IS_CONNECTED);
}

/* connect: SSL-related errors */

TEST_F(TLSSocketWrapperFixture, connect_fail_ctr_drbg_seed)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    mbedtls_ctr_drbg_seed_fake.return_val = 1;

    iotSocketConnect_fake.return_val = 0;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_AUTH_FAILURE);
    mbedtls_ctr_drbg_seed_fake.return_val = 0;
}

TEST_F(TLSSocketWrapperFixture, connect_fail_ssl_setup)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();

    mbedtls_ssl_config_defaults_fake.return_val = 0;
    mbedtls_ssl_setup_fake.return_val = 2;
    iotSocketConnect_fake.return_val = NSAPI_ERROR_OK;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_AUTH_FAILURE);
}

TEST_F(TLSSocketWrapperFixture, connect_handshake_fail_ssl_handshake)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();

    mbedtls_ssl_handshake_fake.return_val = -1;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_AUTH_FAILURE);
}

TEST_F(TLSSocketWrapperFixture, connect_handshake_fail_ssl_handshake_in_progress)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    wrapper->set_timeout(1);
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    mbedtls_ssl_handshake_fake.return_val = MBEDTLS_ERR_SSL_WANT_READ;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_IN_PROGRESS);

    // Check that send will fail in this situation.
    mbedtls_ssl_handshake_fake.return_val = MBEDTLS_ERR_SSL_WANT_READ;
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    EXPECT_EQ(wrapper->send(dataBuf, dataSize), NSAPI_ERROR_WOULD_BLOCK);

    // Check that recv will fail in this situation.
    mbedtls_ssl_handshake_fake.return_val = MBEDTLS_ERR_SSL_WANT_READ;
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    EXPECT_EQ(wrapper->recv(dataBuf, dataSize), NSAPI_ERROR_WOULD_BLOCK);
}

TEST_F(TLSSocketWrapperFixture, connect_handshake_fail_ssl_get_verify_result)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();

    mbedtls_ssl_get_verify_result_fake.return_val = 1;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
}

/* send */

TEST_F(TLSSocketWrapperFixture, send_no_open)
{
    iotSocketSend_fake.return_val = 0;
    EXPECT_EQ(wrapper->send(dataBuf, dataSize), NSAPI_ERROR_NO_CONNECTION);
}

TEST_F(TLSSocketWrapperFixture, send_in_one_chunk)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();

    const SocketAddress a("127.0.0.1", 1024);
    mbedtls_ssl_write_fake.return_val = dataSize;
    iotSocketSend_fake.return_val = dataSize;

    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->send(dataBuf, dataSize), dataSize);
    EXPECT_EQ(static_cast<void *>(dataBuf), mbedtls_ssl_write_fake.arg1_history[0]);
    EXPECT_EQ(dataSize, mbedtls_ssl_write_fake.arg2_history[0]);
}

TEST_F(TLSSocketWrapperFixture, send_error_would_block)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();

    mbedtls_ssl_write_fake.return_val = MBEDTLS_ERR_SSL_WANT_WRITE;
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->send(dataBuf, dataSize), NSAPI_ERROR_WOULD_BLOCK);
}

TEST_F(TLSSocketWrapperFixture, send_device_error)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();

    mbedtls_ssl_write_fake.return_val = MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE;
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->send(dataBuf, dataSize), NSAPI_ERROR_DEVICE_ERROR);
}

TEST_F(TLSSocketWrapperFixture, send_to)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();

    mbedtls_ssl_write_fake.return_val = dataSize;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->sendto(a, dataBuf, dataSize), dataSize);
}

/* recv */

TEST_F(TLSSocketWrapperFixture, recv_no_open)
{
    iotSocketRecv_fake.return_val = 0;
    EXPECT_EQ(wrapper->recv(dataBuf, dataSize), NSAPI_ERROR_NO_CONNECTION);
}

TEST_F(TLSSocketWrapperFixture, recv_all_data)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    mbedtls_ssl_read_fake.return_val = dataSize;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->recv(dataBuf, dataSize), dataSize);
    EXPECT_EQ(static_cast<void *>(dataBuf), mbedtls_ssl_read_fake.arg1_history[0]);
    EXPECT_EQ(dataSize, mbedtls_ssl_read_fake.arg2_history[0]);
}

TEST_F(TLSSocketWrapperFixture, recv_less_than_expected)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    unsigned int lessThanDataSize = dataSize - 1;
    mbedtls_ssl_read_fake.return_val = lessThanDataSize;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->recv(dataBuf, dataSize), lessThanDataSize);
}

TEST_F(TLSSocketWrapperFixture, recv_would_block)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    mbedtls_ssl_read_fake.return_val = MBEDTLS_ERR_SSL_WANT_WRITE;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    EXPECT_EQ(wrapper->recv(dataBuf, dataSize), NSAPI_ERROR_WOULD_BLOCK);
}

TEST_F(TLSSocketWrapperFixture, recv_device_error)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    mbedtls_ssl_read_fake.return_val = MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    EXPECT_EQ(wrapper->recv(dataBuf, dataSize), NSAPI_ERROR_DEVICE_ERROR);
}

TEST_F(TLSSocketWrapperFixture, recv_peer_clode_notify)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    mbedtls_ssl_read_fake.return_val = MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    EXPECT_EQ(wrapper->recv(dataBuf, dataSize), 0);
}

TEST_F(TLSSocketWrapperFixture, recv_from_no_socket)
{
    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->recvfrom(&a, dataBuf, dataSize), NSAPI_ERROR_NO_CONNECTION);
}

TEST_F(TLSSocketWrapperFixture, recv_from)
{
    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(transport.open(), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    SocketAddress b;
    EXPECT_EQ(wrapper->recvfrom(&b, dataBuf, dataSize), NSAPI_ERROR_OK);
}

TEST_F(TLSSocketWrapperFixture, recv_from_null)
{
    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(transport.open(), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->connect(a), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->recvfrom(NULL, dataBuf, dataSize), NSAPI_ERROR_OK);
}

/* set_root_ca_cert */

TEST_F(TLSSocketWrapperFixture, set_root_ca_cert)
{
    EXPECT_EQ(wrapper->get_ca_chain(), static_cast<mbedtls_x509_crt *>(NULL));
    EXPECT_EQ(transport.open(), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->set_root_ca_cert(cert, strlen(cert)), NSAPI_ERROR_OK);
    EXPECT_NE(wrapper->get_ca_chain(), static_cast<mbedtls_x509_crt *>(NULL));
    ASSERT_EQ(mbedtls_x509_crt_init_fake.call_count, 1);
    ASSERT_EQ(mbedtls_x509_crt_parse_fake.call_count, 1);
    EXPECT_EQ(reinterpret_cast<const unsigned char *>(cert), mbedtls_x509_crt_parse_fake.arg1_history[0]);
}

TEST_F(TLSSocketWrapperFixture, set_root_ca_cert_nolen)
{
    EXPECT_EQ(transport.open(), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->set_root_ca_cert(cert), NSAPI_ERROR_OK);
}

TEST_F(TLSSocketWrapperFixture, set_root_ca_cert_invalid)
{
    EXPECT_EQ(transport.open(), NSAPI_ERROR_OK);
    mbedtls_x509_crt_parse_fake.return_val = 1;
    EXPECT_EQ(wrapper->set_root_ca_cert(cert, strlen(cert)), NSAPI_ERROR_PARAMETER);
}

TEST_F(TLSSocketWrapperFixture, set_client_cert_key)
{
    EXPECT_EQ(wrapper->get_own_cert(), static_cast<mbedtls_x509_crt *>(NULL));
    EXPECT_EQ(transport.open(), NSAPI_ERROR_OK);
    EXPECT_EQ(wrapper->set_client_cert_key(cert, cert), NSAPI_ERROR_OK);
    EXPECT_NE(wrapper->get_own_cert(), static_cast<mbedtls_x509_crt *>(NULL));

    ASSERT_EQ(mbedtls_x509_crt_init_fake.call_count, 1);
    ASSERT_EQ(mbedtls_x509_crt_parse_fake.call_count, 1);
    EXPECT_EQ(reinterpret_cast<const unsigned char *>(cert), mbedtls_x509_crt_parse_fake.arg1_history[0]);

    ASSERT_EQ(mbedtls_pk_init_fake.call_count, 1);
    ASSERT_EQ(mbedtls_pk_parse_key_fake.call_count, 1);
    EXPECT_EQ(reinterpret_cast<const unsigned char *>(cert), mbedtls_pk_parse_key_fake.arg1_history[0]);
}

TEST_F(TLSSocketWrapperFixture, set_client_cert_key_invalid)
{
    EXPECT_EQ(transport.open(), NSAPI_ERROR_OK);
    mbedtls_x509_crt_parse_fake.return_val = 1;
    EXPECT_EQ(wrapper->set_client_cert_key(cert, cert), NSAPI_ERROR_PARAMETER);
}

TEST_F(TLSSocketWrapperFixture, set_client_cert_key_invalid_pem)
{
    EXPECT_EQ(transport.open(), NSAPI_ERROR_OK);
    mbedtls_x509_crt_parse_fake.return_val = 0;
    mbedtls_pk_parse_key_fake.return_val = 1;
    EXPECT_EQ(wrapper->set_client_cert_key(cert, cert), NSAPI_ERROR_PARAMETER);
}

TEST_F(TLSSocketWrapperFixture, bind)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->bind(a), NSAPI_ERROR_OK);
}

TEST_F(TLSSocketWrapperFixture, bind_no_open)
{
    iotSocketBind_fake.return_val = 0;
    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(wrapper->bind(a), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TLSSocketWrapperFixture, setsockopt)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    EXPECT_EQ(wrapper->setsockopt(0, 0, 0, 0), NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TLSSocketWrapperFixture, getsockopt_no_stack)
{
    EXPECT_EQ(wrapper->getsockopt(0, 0, 0, 0), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TLSSocketWrapperFixture, getsockopt)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    transport.open();
    EXPECT_EQ(wrapper->getsockopt(0, 0, 0, 0), NSAPI_ERROR_UNSUPPORTED);
}

/* unsupported */

TEST_F(TLSSocketWrapperFixture, listen_unsupported)
{
    EXPECT_EQ(wrapper->listen(1), NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TLSSocketWrapperFixture, accept_unsupported)
{
    nsapi_error_t error;
    EXPECT_EQ(wrapper->accept(&error), static_cast<TCPSocket *>(NULL));
    EXPECT_EQ(error, NSAPI_ERROR_UNSUPPORTED);
}
