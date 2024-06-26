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
#include "netsocket/TLSSocket.h"

#include "mbed_error.h"

class TLSSocketFixture : public testing::Test {
public:
    unsigned int dataSize = 10;
    char dataBuf[10];

protected:
    TLSSocket socket;

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

/* connect */

TEST_F(TLSSocketFixture, connect)
{
    socket.open();

    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_OK);
}

TEST_F(TLSSocketFixture, connect_no_open)
{
    SocketAddress a("127.0.0.1", 1024);
    EXPECT_EQ(socket.connect(a), NSAPI_ERROR_NO_SOCKET);
}
