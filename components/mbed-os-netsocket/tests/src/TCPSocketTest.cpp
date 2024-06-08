/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "netsocket/TCPSocket.h"

#include "gtest/gtest.h"

static uint32_t exampleValidSocketId = 1234;
static uint32_t exampleValidServerSocketId = 5678;
// Control the rtos EventFlags stub. See EventFlags_stub.cpp
// extern std::list<uint32_t> eventFlagsStubNextRetval;

// To test protected functions
class TCPSocketFriend : public TCPSocket {
    friend class TCPSocketFixture;
    FRIEND_TEST(TCPSocketFixture, get_proto);
};

class TCPSocketFixture : public testing::Test {
public:
    unsigned int dataSize = 10;
    char dataBuf[10];

protected:
    TCPSocket socket;

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
    }

    virtual void TearDown()
    {
    }
};

// Control the rtos EventFlags stub. See EventFlags_stub.cpp
// extern std::list<uint32_t> eventFlagsStubNextRetval;

class TCPServerFixture : public testing::Test {
public:
    unsigned int dataSize = 10;
    char dataBuf[10];

protected:
    TCPSocket socket;
    TCPSocket server;

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(TCPSocketFixture, constructor_parameters)
{
    TCPSocket socketParam;
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socketParam.open();

    iotSocketConnect_fake.return_val = 0;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socketParam.connect(a), NSAPI_ERROR_OK);
}

/* connect */

TEST_F(TCPSocketFixture, connect)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();
    const SocketAddress a("127.0.0.1", 1024);
    iotSocketConnect_fake.return_val = 0;
    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_OK);
}

TEST_F(TCPSocketFixture, connect_no_open)
{
    iotSocketConnect_fake.return_val = IOT_SOCKET_EINVAL;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TCPSocketFixture, connect_error_in_progress_no_timeout)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();

    iotSocketConnect_fake.return_val = IOT_SOCKET_EINPROGRESS;
    const SocketAddress a("127.0.0.1", 1024);
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_IN_PROGRESS);
}

TEST_F(TCPSocketFixture, connect_with_timeout)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();

    iotSocketConnect_fake.return_val = IOT_SOCKET_EINPROGRESS;
    const SocketAddress a("127.0.0.1", 1024);
    osEventFlagsWait_fake.return_val = osFlagsErrorTimeout; // Breaking the event flag check loop
    socket.set_blocking(true);
    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_IN_PROGRESS);
}

TEST_F(TCPSocketFixture, connect_error_is_connected)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();

    iotSocketConnect_fake.return_val = IOT_SOCKET_EISCONN;
    const SocketAddress a("127.0.0.1", 1024);
    socket.set_timeout(1);
    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_IS_CONNECTED);
}

/* send */

TEST_F(TCPSocketFixture, send_no_open)
{
    iotSocketSend_fake.return_val = 0;
    EXPECT_EQ(socket.send(dataBuf, dataSize), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TCPSocketFixture, send_in_one_chunk)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();
    socket.bind(1);

    iotSocketSend_fake.return_val = dataSize;
    EXPECT_EQ(socket.send(dataBuf, dataSize), dataSize);
    EXPECT_EQ(dataBuf, iotSocketSend_fake.arg1_history[0]);
    EXPECT_EQ(dataSize, iotSocketSend_fake.arg2_history[0]);
}

TEST_F(TCPSocketFixture, send_error_other)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();
    socket.bind(1);

    iotSocketSend_fake.return_val = IOT_SOCKET_ENOMEM;
    EXPECT_EQ(socket.send(dataBuf, dataSize), NSAPI_ERROR_NO_MEMORY);
    EXPECT_EQ(dataBuf, iotSocketSend_fake.arg1_history[0]);
    EXPECT_EQ(dataSize, iotSocketSend_fake.arg2_history[0]);
}

TEST_F(TCPSocketFixture, send_error_no_timeout)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();
    socket.bind(1);

    iotSocketSend_fake.return_val = IOT_SOCKET_EAGAIN;
    socket.set_blocking(false);
    EXPECT_EQ(socket.send(dataBuf, dataSize), NSAPI_ERROR_WOULD_BLOCK);
    EXPECT_EQ(dataBuf, iotSocketSend_fake.arg1_history[0]);
    EXPECT_EQ(dataSize, iotSocketSend_fake.arg2_history[0]);
}

TEST_F(TCPSocketFixture, send_to)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();
    socket.bind(1);

    iotSocketSend_fake.return_val = 10;
    const SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket.sendto(a, dataBuf, dataSize), dataSize);
    EXPECT_EQ(dataBuf, iotSocketSend_fake.arg1_history[0]);
    EXPECT_EQ(dataSize, iotSocketSend_fake.arg2_history[0]);
}

/* recv */

TEST_F(TCPSocketFixture, recv_no_open)
{
    iotSocketRecv_fake.return_val = 0;
    EXPECT_EQ(socket.recv(dataBuf, dataSize), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TCPSocketFixture, recv_all_data)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();
    socket.bind(1);

    iotSocketRecv_fake.return_val = dataSize;
    EXPECT_EQ(socket.recv(dataBuf, dataSize), dataSize);
    EXPECT_EQ(dataBuf, iotSocketRecv_fake.arg1_history[0]);
    EXPECT_EQ(dataSize, iotSocketRecv_fake.arg2_history[0]);
}

TEST_F(TCPSocketFixture, recv_less_than_expected)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();
    socket.bind(1);

    unsigned int lessThanDataSize = dataSize - 1;
    iotSocketRecv_fake.return_val = lessThanDataSize;
    EXPECT_EQ(socket.recv(dataBuf, dataSize), lessThanDataSize);
    EXPECT_EQ(dataBuf, iotSocketRecv_fake.arg1_history[0]);
    EXPECT_EQ(dataSize, iotSocketRecv_fake.arg2_history[0]);
}

TEST_F(TCPSocketFixture, recv_from_no_socket)
{
    iotSocketRecvFrom_fake.return_val = 0;
    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket.recvfrom(&a, dataBuf, dataSize), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TCPSocketFixture, recv_from)
{
    SocketAddress a("127.0.0.1", 1024);
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    EXPECT_EQ(socket.open(), NSAPI_ERROR_OK);

    iotSocketConnect_fake.return_val = 0;
    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_OK);
    SocketAddress b;

    iotSocketRecvFrom_fake.return_val = 0;
    EXPECT_EQ(socket.recvfrom(&b, dataBuf, dataSize), NSAPI_ERROR_OK);
    EXPECT_EQ(dataBuf, iotSocketRecvFrom_fake.arg1_history[0]);
    EXPECT_EQ(dataSize, iotSocketRecvFrom_fake.arg2_history[0]);
}

TEST_F(TCPSocketFixture, recv_from_null)
{
    SocketAddress a("127.0.0.1", 1024);
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    EXPECT_EQ(socket.open(), NSAPI_ERROR_OK);

    iotSocketConnect_fake.return_val = 0;
    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_OK);

    iotSocketRecvFrom_fake.return_val = 0;
    EXPECT_EQ(socket.recvfrom(NULL, dataBuf, dataSize), NSAPI_ERROR_OK);
    EXPECT_EQ(dataBuf, iotSocketRecvFrom_fake.arg1_history[0]);
    EXPECT_EQ(dataSize, iotSocketRecvFrom_fake.arg2_history[0]);
}

TEST_F(TCPSocketFixture, unsupported_api)
{
    SocketAddress addr;
    EXPECT_EQ(socket.join_multicast_group(addr), NSAPI_ERROR_UNSUPPORTED);
}

/* listen */

TEST_F(TCPSocketFixture, listen_no_open)
{
    iotSocketListen_fake.return_val = 0;
    EXPECT_EQ(socket.listen(1), NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TCPSocketFixture, listen)
{
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    socket.open();
    socket.bind(1);
    iotSocketListen_fake.return_val = 0;
    EXPECT_EQ(socket.listen(1), NSAPI_ERROR_OK);
}

/* TCP server */

TEST_F(TCPServerFixture, accept_no_open)
{
    nsapi_error_t error;
    iotSocketAccept_fake.return_val = 0;
    EXPECT_EQ(socket.accept(&error), nullptr);
    EXPECT_EQ(error, NSAPI_ERROR_NO_SOCKET);
}

TEST_F(TCPServerFixture, accept)
{
    const SocketAddress a("127.0.0.1", 1024);
    iotSocketCreate_fake.return_val = exampleValidSocketId;
    EXPECT_EQ(socket.open(), NSAPI_ERROR_OK);

    iotSocketConnect_fake.return_val = 0;
    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_OK);

    nsapi_error_t error;
    iotSocketCreate_fake.return_val = exampleValidServerSocketId;
    EXPECT_EQ(server.open(), 0);

    iotSocketBind_fake.return_val = 0;
    EXPECT_EQ(server.bind(a), NSAPI_ERROR_OK);

    iotSocketListen_fake.return_val = 0;
    server.listen(1);

    iotSocketCreate_fake.return_val = 0;
    socket.open();

    iotSocketAccept_fake.return_val = 0;
    TCPSocket *sock = server.accept(&error);
    EXPECT_NE(sock, nullptr);
    EXPECT_EQ(error, NSAPI_ERROR_OK);
    if (sock) {
        sock->close();
    }
}

TEST_F(TCPServerFixture, accept_error)
{
    nsapi_error_t error;
    iotSocketCreate_fake.return_val = exampleValidServerSocketId;
    EXPECT_EQ(server.open(), 0);
    EXPECT_EQ(server.bind(1), 0);
    iotSocketAccept_fake.return_val = IOT_SOCKET_ECONNRESET;
    EXPECT_EQ(server.accept(&error), nullptr);
    EXPECT_EQ(error, NSAPI_ERROR_AUTH_FAILURE);
}
