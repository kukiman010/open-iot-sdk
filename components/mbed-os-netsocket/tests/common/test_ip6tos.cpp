/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gtest/gtest.h"

#include "common_functions.h"
#include "ip6string.h"

TEST(TestIp6tos, converting_ipv6_address_with_consecutive_zeros_succeeds)
{
    uint8_t ipv6_address[16] = {
        0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00, 0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34};
    char str_ipv6[40];
    uint_fast8_t length = ip6tos(ipv6_address, str_ipv6);
    EXPECT_EQ(28, length);
    EXPECT_STREQ(str_ipv6, "2001:db8:85a3::8a2e:370:7334");
}

TEST(TestIp6tos, converting_ipv6_address_with_no_consecutive_zeros_succeeds)
{
    char output[40];
    uint8_t address[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    uint_fast8_t length = ip6tos(address, output);
    EXPECT_EQ(29, length);
    EXPECT_STREQ(output, "1:203:405:607:809:a0b:c0d:e0f");
}

TEST(TestIp6tos, converting_compact_ipv6_address_succeeds)
{
    char output[40];
    uint8_t address[16] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    uint_fast8_t length = ip6tos(address, output);
    EXPECT_EQ(3, length);
    EXPECT_STREQ(output, "::1");
}

TEST(TestIp6tos, converting_ipv6_address_with_all_zeros_succeeds)
{
    uint8_t ipv6_address[16] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    char str_ipv6[40];
    uint_fast8_t length = ip6tos(ipv6_address, str_ipv6);
    EXPECT_EQ(2, length);
    EXPECT_STREQ(str_ipv6, "::");
}

TEST(TestIp6tos, converting_ipv6_address_with_all_ffs_succeeds)
{
    uint8_t ipv6_address[16] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    char str_ipv6[40];
    uint_fast8_t length = ip6tos(ipv6_address, str_ipv6);
    EXPECT_EQ(39, length);
    EXPECT_STREQ(str_ipv6, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
}
