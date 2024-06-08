/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "netsocket/SocketAddress.h"

#include "fff.h"
#include "gtest/gtest.h"
#include <chrono>
#include <future>
#include <thread>
DEFINE_FFF_GLOBALS

class SocketAddressFixture : public testing::Test {
protected:
    SocketAddress address;
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(SocketAddressFixture, constructor_default)
{
    EXPECT_EQ(NULL, address.get_ip_address());
    EXPECT_EQ(0, address.get_port());
    EXPECT_EQ(NSAPI_UNSPEC, address.get_ip_version());
}

TEST_F(SocketAddressFixture, constructor_address_ip4)
{
    SocketAddress addr("127.0.0.1", 80);
    EXPECT_EQ(std::string("127.0.0.1"), std::string(addr.get_ip_address()));
    EXPECT_EQ(80, addr.get_port());
    EXPECT_EQ(NSAPI_IPv4, addr.get_ip_version());
}

TEST_F(SocketAddressFixture, constructor_address_ip6)
{
    uint8_t addrBytes[NSAPI_IP_BYTES] = {18, 52, 86, 120, 154, 188, 222, 240, 18, 52, 86, 120, 0, 0, 222, 240};
    SocketAddress addr("1234:5678:9abc:def0:1234:5678::def0", 80);
    EXPECT_EQ(80, addr.get_port());
    EXPECT_EQ(NSAPI_IPv6, addr.get_ip_version());
    // Note that a missing zero is added between two neighbouring colons
    EXPECT_EQ(std::string("1234:5678:9abc:def0:1234:5678:0:def0"), std::string(addr.get_ip_address()));
    EXPECT_TRUE(0 == memcmp(addrBytes, static_cast<const uint8_t *>(addr.get_ip_bytes()), sizeof(addrBytes)));
}

TEST_F(SocketAddressFixture, constructor_address_ip6_invalid)
{
    SocketAddress addr("zzzz:yyyy:xxxx:def0:1234:5678:9abc:def0", 80);
    EXPECT_EQ(80, addr.get_port());
    EXPECT_EQ(NSAPI_UNSPEC, addr.get_ip_version());
    EXPECT_EQ(NULL, addr.get_ip_address());
    EXPECT_EQ(NULL, addr.get_ip_bytes());
}

TEST_F(SocketAddressFixture, constructor_address_bytes_ip4)
{
    uint8_t addrBytes[NSAPI_IP_BYTES] = {127, 0, 0, 2};

    SocketAddress addr(addrBytes, NSAPI_IPv4, 80);
    EXPECT_EQ(std::string("127.0.0.2"), std::string(addr.get_ip_address()));
    EXPECT_EQ(80, addr.get_port());
    EXPECT_EQ(NSAPI_IPv4, addr.get_ip_version());
    EXPECT_TRUE(0 == memcmp(addrBytes, static_cast<const uint8_t *>(addr.get_ip_bytes()), sizeof(addrBytes)));
}

TEST_F(SocketAddressFixture, constructor_address_bytes_ip6)
{
    uint8_t addrBytes[NSAPI_IP_BYTES] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};

    SocketAddress addr(addrBytes, NSAPI_IPv6, 80);
    // the leading zeroes are removed
    EXPECT_EQ(std::string("1:203:405:607:809:a0b:c0d:e0f"), std::string(addr.get_ip_address()));
    EXPECT_EQ(80, addr.get_port());
    EXPECT_EQ(NSAPI_IPv6, addr.get_ip_version());
    EXPECT_TRUE(0 == memcmp(addrBytes, static_cast<const uint8_t *>(addr.get_ip_bytes()), NSAPI_IP_BYTES));
}

TEST_F(SocketAddressFixture, constructor_copy)
{
    SocketAddress addr(address);
    EXPECT_EQ(address.get_ip_address(), addr.get_ip_address());
    EXPECT_EQ(address.get_port(), addr.get_port());
    EXPECT_EQ(address.get_ip_version(), addr.get_ip_version());
}

TEST_F(SocketAddressFixture, assignment_and_equality_operator_ip4)
{
    SocketAddress addr;
    address.set_ip_address("127.0.0.2");
    address.set_port(81);
    // First verify assignment operator (including return value) using equality operator.
    EXPECT_EQ((addr = address), address);
    // Then check inequality operator
    EXPECT_FALSE(addr != address);
    // Now proceed to check that the equality operator really works.
    EXPECT_EQ(addr, address);
}

TEST_F(SocketAddressFixture, assignment_and_equality_operator_ip6)
{
    SocketAddress addr;
    address.set_ip_address("0:0:0:0:0:ffff:7f00:1");
    address.set_port(81);
    // First verify assignment operator (including return value) using equality operator.
    EXPECT_EQ((addr = address), address);
    // Then check inequality operator
    EXPECT_FALSE(addr != address);
    // Now proceed to check that the equality operator really works.
    EXPECT_EQ(addr, address);
}

TEST_F(SocketAddressFixture, equality_null_check)
{
    address.set_ip_address("127.0.0.2");
    EXPECT_NE(address, static_cast<SocketAddress>(NULL));
}

TEST_F(SocketAddressFixture, equality_wrong_version)
{
    SocketAddress addr4("127.0.0.1", 80);
    SocketAddress addr6("0:0:0:0:0:ffff:7f00:1", 80); // This is converted 127.0.0.1
    EXPECT_NE(addr4, addr6);                          // But they should still be inequal.
}

TEST_F(SocketAddressFixture, bool_operator_false)
{
    EXPECT_FALSE(address ? true : false);
}

TEST_F(SocketAddressFixture, bool_operator_ip4_true)
{
    SocketAddress addr("127.0.0.1", 80);
    EXPECT_TRUE(addr ? true : false);
}

TEST_F(SocketAddressFixture, bool_operator_ip6_true)
{
    SocketAddress addr("1234:5678:9abc:def0:1234:5678:9abc:def0", 80);
    EXPECT_TRUE(addr ? true : false);
}

TEST_F(SocketAddressFixture, bool_operator_ip6_false)
{
    SocketAddress addr("0:0:0:0:0:0:0:0", 80);
    EXPECT_FALSE(addr ? true : false);
}
