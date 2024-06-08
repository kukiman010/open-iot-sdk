/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gtest/gtest.h"

#include "common_functions.h"

TEST(TestBitCopy, copying_a_full_byte_succeeds)
{
    uint8_t dst[1] = {0};
    uint8_t src[1] = {0xF};
    uint8_t expected[1] = {0xF};

    bitcopy(dst, src, 8);

    EXPECT_EQ(0, memcmp(dst, expected, 1));
}

TEST(TestBitCopy, copying_a_byte_partially_succeeds)
{
    uint8_t dst[1] = {0};
    uint8_t src[1] = {0xF0};
    uint8_t expected[1] = {0xF0};

    bitcopy(dst, src, 4);

    EXPECT_EQ(0, memcmp(dst, expected, 1));
}

TEST(TestBitCopy, copying_multiple_bytes_succeeds)
{
    uint8_t dst[3] = {0};
    uint8_t src[3] = {0xF, 0xF, 0xF};
    uint8_t expected[3] = {0xF, 0xF, 0xF};

    bitcopy(dst, src, 24);

    EXPECT_EQ(0, memcmp(dst, expected, 3));
}

TEST(TestBitCopy, copying_zero_bit_succeeds)
{
    uint8_t dst[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t src[5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
    uint8_t expected_dst[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

    bitcopy(dst, src, 0);

    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(dst[i], expected_dst[i]);
    }
}

TEST(TestBitCopy, copying_eight_bits_succeeds)
{
    uint8_t src[2] = {0xAB, 0xCD};
    uint8_t dst[2] = {0x00, 0x00};
    uint8_t expected[2] = {0xAB, 0x00};

    bitcopy(dst, src, 8);

    for (int i = 0; i < 2; i++) {
        EXPECT_EQ(expected[i], dst[i]);
    }
}
