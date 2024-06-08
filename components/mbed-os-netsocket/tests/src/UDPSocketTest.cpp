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

#include "netsocket/UDPSocket.h"

#include "gtest/gtest.h"

static uint32_t exampleValidSocketId = 1234;
static uint32_t socket_opt_val = 0;

/**
 * This test needs to access a private function
 * which is why it is declared as friend
 * following the documentation of google test.
 * [*](https://github.com/google/googletest/blob/master/googletest/docs/advanced.md)
 */
class UDPSocketFriend : public UDPSocket {
    friend class UDPSocketFixture;
    FRIEND_TEST(UDPSocketFixture, get_proto);
};

class UDPSocketFixture : public testing::Test {
protected:
    UDPSocket socket;
    unsigned int dataSize = 10;
    char dataBuf[10];

    virtual void SetUp()
    {
        RESET_FAKE(osMutexNew);
        RESET_FAKE(osMutexAcquire);
        RESET_FAKE(osMutexRelease);
        RESET_FAKE(osEventFlagsWait);
        RESET_FAKE(osEventFlagsSet);
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
        socket_opt_val = 0;
    }

    virtual void TearDown()
    {
    }
};

TEST_F(UDPSocketFixture, opening_socket_with_unsupported_nsapi_socket_type)
{
    EXPECT_EQ(socket.open(NSAPI_UNSPEC), NSAPI_ERROR_PARAMETER);
}

TEST_F(UDPSocketFixture, opening_ipv4_socket_succeeds)
{
    EXPECT_EQ(socket.open(NSAPI_IPv4), NSAPI_ERROR_OK);
}

TEST_F(UDPSocketFixture, opening_opened_ipv4_socket_fails)
{
    EXPECT_EQ(socket.open(NSAPI_IPv4), NSAPI_ERROR_OK);
    EXPECT_EQ(socket.open(NSAPI_IPv4), NSAPI_ERROR_UNKNOWN);
}

TEST_F(UDPSocketFixture, closing_ipv4_socket_succeeds)
{
    EXPECT_EQ(socket.open(NSAPI_IPv4), NSAPI_ERROR_OK);
    EXPECT_EQ(socket.close(), NSAPI_ERROR_OK);
}

TEST_F(UDPSocketFixture, opening_closed_ipv4_socket_succeeds)
{
    EXPECT_EQ(socket.open(NSAPI_IPv4), NSAPI_ERROR_OK);
    EXPECT_EQ(socket.close(), NSAPI_ERROR_OK);
    EXPECT_EQ(socket.open(NSAPI_IPv4), NSAPI_ERROR_OK);
}

TEST_F(UDPSocketFixture, opening_ipv6_socket_succeeds)
{
    EXPECT_EQ(socket.open(NSAPI_IPv6), NSAPI_ERROR_OK);
}

TEST_F(UDPSocketFixture, opening_opened_ipv6_socket_fails)
{
    EXPECT_EQ(socket.open(NSAPI_IPv6), NSAPI_ERROR_OK);
    EXPECT_EQ(socket.open(NSAPI_IPv6), NSAPI_ERROR_UNKNOWN);
}

TEST_F(UDPSocketFixture, closing_ipv6_socket_succeeds)
{
    EXPECT_EQ(socket.open(NSAPI_IPv6), NSAPI_ERROR_OK);
    EXPECT_EQ(socket.close(), NSAPI_ERROR_OK);
}

TEST_F(UDPSocketFixture, opening_closed_ipv6_socket_succeeds)
{
    EXPECT_EQ(socket.open(NSAPI_IPv6), NSAPI_ERROR_OK);
    EXPECT_EQ(socket.close(), NSAPI_ERROR_OK);
    EXPECT_EQ(socket.open(NSAPI_IPv6), NSAPI_ERROR_OK);
}

TEST_F(UDPSocketFixture, closing_invalid_socket_fails)
{
    iotSocketClose_fake.return_val = IOT_SOCKET_ESOCK;
    EXPECT_NE(socket.close(), NSAPI_ERROR_OK);
}

TEST_F(UDPSocketFixture, destructor_calls_close)
{
    EXPECT_EQ(iotSocketClose_fake.call_count, 0);
    {
        UDPSocket s;
        EXPECT_EQ(s.open(NSAPI_IPv4), NSAPI_ERROR_OK);
    }

    EXPECT_GT(iotSocketClose_fake.call_count, 0);
}

TEST_F(UDPSocketFixture, binding_unopened_socket_fails)
{
    EXPECT_EQ(socket.bind(80), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(UDPSocketFixture, binding_opened_socket_succeeds)
{
    iotSocketBind_fake.return_val = 0;

    EXPECT_EQ(socket.open(NSAPI_IPv4), NSAPI_ERROR_OK);
    EXPECT_EQ(socket.bind(80), NSAPI_ERROR_OK);
}

TEST_F(UDPSocketFixture, binding_opened_socket_failure_returns_failures)
{
    iotSocketBind_fake.return_val = IOT_SOCKET_ERROR;

    EXPECT_EQ(socket.open(NSAPI_IPv4), NSAPI_ERROR_OK);
    EXPECT_NE(socket.bind(80), NSAPI_ERROR_OK);
}

TEST_F(UDPSocketFixture, binding_unopened_to_address_socket_fails)
{
    const SocketAddress a("127.0.0.1", 80);
    EXPECT_EQ(socket.bind(a), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(UDPSocketFixture, binding_opened_unbound_socket_of_one_type_to_socket_address_of_another_type_succeeds)
{
    EXPECT_EQ(socket.open(NSAPI_IPv4), NSAPI_ERROR_OK);
    const SocketAddress a("::1", 80);
    EXPECT_EQ(socket.bind(a), NSAPI_ERROR_OK);
}

TEST_F(UDPSocketFixture, binding_opened_bound_socket_of_one_type_to_socket_address_of_another_type_fails)
{
    EXPECT_EQ(socket.open(NSAPI_IPv4), NSAPI_ERROR_OK);

    const SocketAddress a1("::1", 80);
    EXPECT_EQ(socket.bind(a1), NSAPI_ERROR_OK);

    const SocketAddress a2("127.0.0.1", 83);
    EXPECT_EQ(socket.bind(a2), NSAPI_ERROR_PARAMETER);
}

TEST_F(UDPSocketFixture, setting_blocking_to_true_sets_the_option_to_wait_forever)
{
    socket.set_blocking(true);
    EXPECT_EQ(osWaitForever, *(static_cast<const uint32_t *>(iotSocketSetOpt_fake.arg2_history[0])));
}

TEST_F(UDPSocketFixture, setting_blocking_to_false_does_not_sets_the_option_to_wait_forever)
{
    socket.set_blocking(false);
    EXPECT_NE(osWaitForever, *(static_cast<const uint32_t *>(iotSocketSetOpt_fake.arg2_history[0])));
}

TEST_F(UDPSocketFixture,
       setting_timeout_to_positive_integer_sets_the_option_to_wait_for_period_equals_to_the_positive_integer)
{
    socket.set_timeout(1);
    EXPECT_EQ(1, *(static_cast<const uint32_t *>(iotSocketSetOpt_fake.arg2_history[0])));
}

TEST_F(UDPSocketFixture, setting_timeout_to_negative_integer_sets_the_option_to_wait_forever)
{
    socket.set_timeout(-1);
    EXPECT_EQ(osWaitForever, *(static_cast<const uint32_t *>(iotSocketSetOpt_fake.arg2_history[0])));
}

TEST_F(UDPSocketFixture, setting_timeout_to_zero_sets_the_option_to_wait_to_zero)
{
    iotSocketSetOpt_fake.custom_fake = [](int32_t, int32_t, const void *opt_val, uint32_t) {
        socket_opt_val = *(static_cast<const uint32_t *>(opt_val));
        return 0;
    };

    socket.set_timeout(0);
    EXPECT_EQ(0, socket_opt_val);
}

TEST_F(UDPSocketFixture, sendto_addr_port)
{
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());

    iotSocketSendTo_fake.return_val = 0;
    EXPECT_EQ(socket.sendto(a, 0, 0), 0);

    iotSocketSetOpt_fake.return_val = IOT_SOCKET_ENOTSUP;
    EXPECT_EQ(socket.setsockopt(NSAPI_SOCKET, NSAPI_BIND_TO_DEVICE, "12324", 5), NSAPI_ERROR_UNSUPPORTED);

    iotSocketSendTo_fake.return_val = 0;
    EXPECT_EQ(socket.sendto(a, 0, 0), 0);
}

TEST_F(UDPSocketFixture, connect)
{
    const nsapi_addr_t addr = {NSAPI_IPv4, {127, 0, 0, 1}};
    const SocketAddress a(addr, 1024);

    iotSocketCreate_fake.return_val = exampleValidSocketId;
    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());
    iotSocketSendTo_fake.return_val = 0;
    EXPECT_EQ(socket.send(dataBuf, dataSize), NSAPI_ERROR_NO_ADDRESS);

    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_OK);

    iotSocketSendTo_fake.return_val = 100;
    EXPECT_EQ(socket.send(dataBuf, dataSize), 100);
}

TEST_F(UDPSocketFixture, sendto_timeout)
{
    const nsapi_addr_t saddr = {NSAPI_IPv4, {127, 0, 0, 1}};
    const SocketAddress addr(saddr, 1024);

    iotSocketCreate_fake.return_val = exampleValidSocketId;
    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());

    iotSocketSendTo_fake.return_val = IOT_SOCKET_EAGAIN;
    uint32_t returnValueSequence[2] = {0, osFlagsErrorTimeout}; // Breaking the event flag check loop
    SET_RETURN_SEQ(osEventFlagsWait, returnValueSequence, 2);
    EXPECT_EQ(socket.sendto(addr, 0, 100), NSAPI_ERROR_WOULD_BLOCK);

    socket.set_timeout(1);

    iotSocketSendTo_fake.return_val = IOT_SOCKET_EAGAIN;
    EXPECT_EQ(socket.sendto(addr, 0, 100), NSAPI_ERROR_WOULD_BLOCK);
}

TEST_F(UDPSocketFixture, recv_address_filtering)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());
    const nsapi_addr_t addr1 = {NSAPI_IPv4, {127, 0, 0, 1}};
    const nsapi_addr_t addr2 = {NSAPI_IPv4, {127, 0, 0, 2}};
    SocketAddress a1(addr1, 1024);
    SocketAddress a2(addr2, 1024);

    EXPECT_EQ(socket.connect(a1), NSAPI_ERROR_OK);

    int32_t returnValueSequence[1] = {IOT_SOCKET_ECONNABORTED};
    SET_RETURN_SEQ(iotSocketRecvFrom, returnValueSequence, 1);
    EXPECT_EQ(socket.recvfrom(&a2, &dataBuf, dataSize), NSAPI_ERROR_CONNECTION_LOST);

    int32_t returnValueSequence2[1] = {100};
    SET_RETURN_SEQ(iotSocketRecvFrom, returnValueSequence2, 1);
    EXPECT_EQ(socket.recvfrom(&a2, &dataBuf, dataSize), 0);
}

TEST_F(UDPSocketFixture, setting_socket_options_with_level_7000_to_keep_alive_succeeds)
{
    iotSocketSetOpt_fake.return_val = 0;

    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());
    int optval = 1;
    EXPECT_EQ(NSAPI_ERROR_OK, socket.setsockopt(NSAPI_SOCKET, NSAPI_KEEPALIVE, &optval, sizeof(optval)));
}

TEST_F(UDPSocketFixture, setting_socket_options_for_unopened_socket_fails)
{
    iotSocketSetOpt_fake.return_val = 0;

    int optval = 1;
    EXPECT_EQ(NSAPI_ERROR_NO_SOCKET, socket.setsockopt(NSAPI_SOCKET, NSAPI_KEEPALIVE, &optval, sizeof(optval)));
}

TEST_F(UDPSocketFixture, setting_socket_options_with_optname_other_than_to_keep_alive_fails)
{
    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());
    int optval = 1;
    EXPECT_EQ(NSAPI_ERROR_UNSUPPORTED, socket.setsockopt(NSAPI_SOCKET, NSAPI_KEEPIDLE, &optval, sizeof(optval)));
}

TEST_F(UDPSocketFixture, return_a_failure_if_an_error_occurs_when_setting_socket_options)
{
    iotSocketSetOpt_fake.return_val = IOT_SOCKET_ERROR;

    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());
    int optval = 1;
    EXPECT_NE(NSAPI_ERROR_OK, socket.setsockopt(NSAPI_SOCKET, NSAPI_KEEPALIVE, &optval, sizeof(optval)));
}

TEST_F(UDPSocketFixture, getting_socket_options_with_level_7000_to_keep_alive_succeeds)
{
    iotSocketGetOpt_fake.return_val = 0;

    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());
    int optval;
    unsigned optlen = sizeof(optval);
    EXPECT_EQ(NSAPI_ERROR_OK, socket.getsockopt(NSAPI_SOCKET, NSAPI_KEEPALIVE, &optval, &optlen));
}

TEST_F(UDPSocketFixture, getting_socket_options_for_unopened_socket_fails)
{
    int optval;
    unsigned optlen = sizeof(optval);
    EXPECT_EQ(NSAPI_ERROR_NO_SOCKET, socket.getsockopt(NSAPI_SOCKET, NSAPI_KEEPALIVE, &optval, &optlen));
}

TEST_F(UDPSocketFixture, getting_socket_options_with_optname_other_than_to_keep_alive_fails)
{
    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());
    int optval;
    unsigned optlen = sizeof(optval);
    EXPECT_EQ(NSAPI_ERROR_UNSUPPORTED, socket.getsockopt(NSAPI_SOCKET, NSAPI_KEEPINTVL, &optval, &optlen));
}

TEST_F(UDPSocketFixture, getting_peer_name_for_unopened_socket_fails)
{
    SocketAddress a;
    EXPECT_EQ(NSAPI_ERROR_NO_SOCKET, socket.getpeername(&a));
}

TEST_F(UDPSocketFixture, TestGetpeernameWhenSocketNotConnected)
{
    iotSocketGetPeerName_fake.return_val = IOT_SOCKET_ENOTCONN;

    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());
    EXPECT_EQ(NSAPI_ERROR_OK, socket.bind(8000));
    SocketAddress a;
    EXPECT_EQ(NSAPI_ERROR_NO_CONNECTION, socket.getpeername(&a));
}

TEST_F(UDPSocketFixture, getting_peer_name_with_nullptr_address_fails)
{
    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());
    EXPECT_EQ(socket.getpeername(nullptr), NSAPI_ERROR_PARAMETER);
}

TEST_F(UDPSocketFixture, getting_peer_name_for_bound_sockets_succeeds)
{
    iotSocketGetPeerName_fake.custom_fake = [](int32_t, uint8_t *ip, uint32_t *, uint16_t *port) {
        ip[0] = 192;
        ip[1] = 168;
        ip[2] = 1;
        ip[3] = 1;

        *port = 8000;

        return 0;
    };

    EXPECT_EQ(NSAPI_ERROR_OK, socket.open());
    EXPECT_EQ(NSAPI_ERROR_OK, socket.bind(8000));

    SocketAddress a;
    EXPECT_EQ(socket.getpeername(&a), NSAPI_ERROR_OK);
    char expected_ip_address[] = "192.168.1.1";
    const char *retrieved_socket_address = a.get_ip_address();
    EXPECT_STREQ(expected_ip_address, retrieved_socket_address);
    EXPECT_EQ(a.get_port(), 8000);
}

TEST_F(UDPSocketFixture, unsupported_api)
{
    nsapi_error_t error;
    EXPECT_EQ(socket.accept(&error), static_cast<Socket *>(NULL));
    EXPECT_EQ(error, NSAPI_ERROR_UNSUPPORTED);
    EXPECT_EQ(socket.listen(1), NSAPI_ERROR_UNSUPPORTED);
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket.join_multicast_group(a), NSAPI_ERROR_UNSUPPORTED);
    EXPECT_EQ(socket.leave_multicast_group(a), NSAPI_ERROR_UNSUPPORTED);
}
