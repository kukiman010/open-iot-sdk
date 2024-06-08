/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gtest/gtest.h"
#include "ip4string.h"

TEST(TestIp4tos, converting_an_ipv4_address_to_string_succeeds)
{
    uint8_t ip[] = {192, 168, 0, 1};
    char str[16];

    uint_fast8_t len = ip4tos(ip, str);

    EXPECT_EQ(len, 11);
    EXPECT_STREQ(str, "192.168.0.1");
}

TEST(TestIp4tos, converting_a_large_ipv4_address_to_string_succeeds)
{
    uint8_t ip[] = {255, 255, 255, 255};
    char str[16];

    uint_fast8_t len = ip4tos(ip, str);

    EXPECT_EQ(len, 15);
    EXPECT_STREQ(str, "255.255.255.255");
}

TEST(TestIp4tos, converting_an_ipv4_address_with_all_zero_succeeds)
{
    uint8_t ip[] = {0, 0, 0, 0};
    char str[16];

    uint_fast8_t len = ip4tos(ip, str);

    EXPECT_EQ(len, 7);
    EXPECT_STREQ(str, "0.0.0.0");
}
