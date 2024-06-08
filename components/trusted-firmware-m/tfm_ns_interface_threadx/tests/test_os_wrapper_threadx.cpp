/* Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

extern "C" {
#include "os_wrapper/mutex.h"
}

#include "fff.h"
#include "tx_api.h"

#include "gtest/gtest.h"

DEFINE_FFF_GLOBALS

class TestTfmOsWrapperThreadx : public testing::Test {
public:
    TestTfmOsWrapperThreadx()
    {
        RESET_FAKE(tx_mutex_create);
        RESET_FAKE(tx_mutex_delete);
        RESET_FAKE(tx_mutex_get);
        RESET_FAKE(tx_mutex_put);
    }
};

TEST_F(TestTfmOsWrapperThreadx, acquiring_a_single_mutex_returns_non_nullptr)
{
    tx_mutex_create_fake.return_val = TX_SUCCESS;

    EXPECT_NE(nullptr, os_wrapper_mutex_create());
}

TEST_F(TestTfmOsWrapperThreadx, acquiring_more_than_one_mutex_returns_nullptr)
{
    tx_mutex_create_fake.return_val = TX_SUCCESS;

    EXPECT_NE(nullptr, os_wrapper_mutex_create());
    EXPECT_EQ(nullptr, os_wrapper_mutex_create());
}

TEST_F(TestTfmOsWrapperThreadx, acquiring_a_mutex_after_previous_has_been_delted_returns_non_nullptr)
{
    tx_mutex_create_fake.return_val = TX_SUCCESS;
    tx_mutex_delete_fake.return_val = TX_SUCCESS;

    void *mutex = os_wrapper_mutex_create();
    EXPECT_NE(nullptr, mutex);
    EXPECT_EQ(OS_WRAPPER_SUCCESS, os_wrapper_mutex_delete(mutex));
    EXPECT_NE(nullptr, os_wrapper_mutex_create());
}

TEST_F(TestTfmOsWrapperThreadx, deleting_mutex_without_creating_it_fails)
{
    EXPECT_NE(OS_WRAPPER_SUCCESS, os_wrapper_mutex_delete(nullptr));
}

TEST_F(TestTfmOsWrapperThreadx, failing_to_delete_created_mutex_returns_failure)
{
    tx_mutex_create_fake.return_val = TX_SUCCESS;
    tx_mutex_delete_fake.return_val = !TX_SUCCESS;

    void *mutex = os_wrapper_mutex_create();
    EXPECT_NE(nullptr, mutex);
    EXPECT_NE(OS_WRAPPER_SUCCESS, os_wrapper_mutex_delete(mutex));
}

TEST_F(TestTfmOsWrapperThreadx, deleting_previously_deleted_mutex_returns_failure)
{
    tx_mutex_create_fake.return_val = TX_SUCCESS;
    tx_mutex_delete_fake.return_val = TX_SUCCESS;

    void *mutex = os_wrapper_mutex_create();
    EXPECT_NE(nullptr, mutex);
    EXPECT_EQ(OS_WRAPPER_SUCCESS, os_wrapper_mutex_delete(mutex));
    EXPECT_NE(OS_WRAPPER_SUCCESS, os_wrapper_mutex_delete(mutex));
}

TEST_F(TestTfmOsWrapperThreadx, acquiring_mutex_without_creating_it_returns_failure)
{
    EXPECT_NE(OS_WRAPPER_SUCCESS, os_wrapper_mutex_acquire(nullptr, 1));
}

TEST_F(TestTfmOsWrapperThreadx, acquiring_mutex_after_creating_it_returns_success)
{
    tx_mutex_create_fake.return_val = TX_SUCCESS;
    tx_mutex_get_fake.return_val = TX_SUCCESS;

    void *mutex = os_wrapper_mutex_create();
    EXPECT_NE(nullptr, mutex);
    EXPECT_EQ(OS_WRAPPER_SUCCESS, os_wrapper_mutex_acquire(mutex, 1));
}

TEST_F(TestTfmOsWrapperThreadx, acquiring_mutex_after_deleting_it_returns_failure)
{
    tx_mutex_create_fake.return_val = TX_SUCCESS;

    void *mutex = os_wrapper_mutex_create();
    EXPECT_NE(nullptr, mutex);
    EXPECT_EQ(OS_WRAPPER_SUCCESS, os_wrapper_mutex_delete(mutex));
    EXPECT_NE(OS_WRAPPER_SUCCESS, os_wrapper_mutex_acquire(mutex, 1));
}

TEST_F(TestTfmOsWrapperThreadx, releasing_mutex_after_deleting_it_returns_failure)
{
    tx_mutex_create_fake.return_val = TX_SUCCESS;

    void *mutex = os_wrapper_mutex_create();
    EXPECT_NE(nullptr, mutex);
    EXPECT_EQ(OS_WRAPPER_SUCCESS, os_wrapper_mutex_delete(mutex));
    EXPECT_NE(OS_WRAPPER_SUCCESS, os_wrapper_mutex_release(mutex));
}

TEST_F(TestTfmOsWrapperThreadx, acquiring_mutex_with_forever_wait_translates_to_threadx_forever_wait)
{
    tx_mutex_create_fake.return_val = TX_SUCCESS;

    void *mutex = os_wrapper_mutex_create();
    EXPECT_NE(nullptr, mutex);
    EXPECT_EQ(OS_WRAPPER_SUCCESS, os_wrapper_mutex_acquire(mutex, OS_WRAPPER_WAIT_FOREVER));
    EXPECT_EQ(tx_mutex_get_fake.arg1_val, TX_WAIT_FOREVER);
}
