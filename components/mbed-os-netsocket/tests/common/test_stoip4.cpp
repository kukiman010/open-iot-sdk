/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ip4string.h"
#include <gtest/gtest.h>

TEST(TestStoip4, converting_an_ipv4_address_succeeds)
{
    char ip[] = "192.168.0.1";
    uint8_t dest[4];
    bool result = stoip4(ip, strlen(ip), dest);
    EXPECT_TRUE(result);
    EXPECT_EQ(dest[0], 192);
    EXPECT_EQ(dest[1], 168);
    EXPECT_EQ(dest[2], 0);
    EXPECT_EQ(dest[3], 1);
}

TEST(TestStoip4, converting_an_ipv4_address_with_extra_decimal_group_fails)
{
    char ip[] = "192.168.0.1.1";
    uint8_t dest[4];
    bool result = stoip4(ip, strlen(ip), dest);
    EXPECT_FALSE(result);
}

TEST(TestStoip4, converting_an_ipv4_address_with_non_decimal_character_in_group_fails)
{
    char ip[] = "192.168.0.1a";
    uint8_t dest[4];
    bool result = stoip4(ip, strlen(ip), dest);
    EXPECT_FALSE(result);
}

TEST(TestStoip4, converting_an_ipv4_address_with_decimal_group_exceeding_255_fails)
{
    char ip[] = "192.168.0.256";
    uint8_t dest[4];
    bool result = stoip4(ip, strlen(ip), dest);
    EXPECT_FALSE(result);
}

TEST(TestStoip4, converting_an_non_terminated_ipv4_address_fails)
{
    char ip[] = "192.168.0.1 ";
    uint8_t dest[4];
    bool result = stoip4(ip, strlen(ip), dest);
    EXPECT_FALSE(result);
}
