/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ip6string.h"
#include <gtest/gtest.h>

TEST(TestStoip6, converting_an_ipv6_address_succeeds)
{
    uint8_t dest[16];
    const char *ip6addr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    size_t len = strlen(ip6addr);

    EXPECT_TRUE(stoip6(ip6addr, len, dest));

    // Check if the destination buffer is correctly filled with the expected values
    EXPECT_EQ(0x20, dest[0]);
    EXPECT_EQ(0x01, dest[1]);
    EXPECT_EQ(0x0d, dest[2]);
    EXPECT_EQ(0xb8, dest[3]);
    EXPECT_EQ(0x85, dest[4]);
    EXPECT_EQ(0xa3, dest[5]);
    EXPECT_EQ(0x00, dest[6]);
    EXPECT_EQ(0x00, dest[7]);
    EXPECT_EQ(0x00, dest[8]);
    EXPECT_EQ(0x00, dest[9]);
    EXPECT_EQ(0x8a, dest[10]);
    EXPECT_EQ(0x2e, dest[11]);
    EXPECT_EQ(0x03, dest[12]);
    EXPECT_EQ(0x70, dest[13]);
    EXPECT_EQ(0x73, dest[14]);
    EXPECT_EQ(0x34, dest[15]);
}

TEST(TestStoip6, converting_an_ipv6_address_with_extra_hexadecimal_group_fails)
{
    uint8_t dest[16];
    const char *ip6addr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334:1111";
    size_t len = strlen(ip6addr);

    EXPECT_FALSE(stoip6(ip6addr, len, dest));

    for (int i = 0; i < 16; i++) {
        EXPECT_EQ(0, dest[i]);
    }
}

TEST(TestStoip6, converting_an_ipv6_address_with_insufficient_hexadecimal_group_fails)
{
    uint8_t dest[16];
    const char *ip6addr = "2001:0db8:85a3:0000:0000:8a2e:0370";
    size_t len = strlen(ip6addr);

    EXPECT_FALSE(stoip6(ip6addr, len, dest));

    for (int i = 0; i < 16; i++) {
        EXPECT_EQ(0, dest[i]);
    }
}

TEST(TestStoip6, converting_an_ipv6_address_with_invalid_character_in_hexadecimal_group_fails)
{
    uint8_t dest[16];
    const char *ip6addr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334z";
    size_t len = strlen(ip6addr);

    EXPECT_FALSE(stoip6(ip6addr, len, dest));
}

TEST(TestStoip6, converting_an_ipv6_address_with_single_double_colon_succeeds)
{
    uint8_t dest[16];
    const char *ip6addr = "2001:0db8:85a3::8a2e:0370:7334";
    size_t len = strlen(ip6addr);

    EXPECT_TRUE(stoip6(ip6addr, len, dest));

    // Check if the destination buffer is correctly filled with the expected values
    EXPECT_EQ(0x20, dest[0]);
    EXPECT_EQ(0x01, dest[1]);
    EXPECT_EQ(0x0d, dest[2]);
    EXPECT_EQ(0xb8, dest[3]);
    EXPECT_EQ(0x85, dest[4]);
    EXPECT_EQ(0xa3, dest[5]);
    EXPECT_EQ(0x00, dest[6]);
    EXPECT_EQ(0x00, dest[7]);
    EXPECT_EQ(0x00, dest[8]);
    EXPECT_EQ(0x00, dest[9]);
    EXPECT_EQ(0x8a, dest[10]);
    EXPECT_EQ(0x2e, dest[11]);
    EXPECT_EQ(0x03, dest[12]);
    EXPECT_EQ(0x70, dest[13]);
    EXPECT_EQ(0x73, dest[14]);
    EXPECT_EQ(0x34, dest[15]);
}

TEST(TestStoip6, converting_an_ipv6_address_with_multiple_double_colon_fails)
{
    uint8_t dest[16];
    const char *ip6addr = "2001::85a3::8a2e:0370:7334";
    size_t len = strlen(ip6addr);

    EXPECT_FALSE(stoip6(ip6addr, len, dest));
}
