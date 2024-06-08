/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

extern "C" {
#include "cmsis_os2.h"
#include "mbedtls_threading_cmsis_rtos.h"
}

#include "fff.h"

#include "gtest/gtest.h"

#include "mbedtls/threading.h"

DEFINE_FFF_GLOBALS

class TestMbedTlsThreadingCmsisRtos : public ::testing::Test {
public:
    TestMbedTlsThreadingCmsisRtos()
    {
        mbedtls_threading_set_cmsis_rtos();

        mbedtls_threading_mutex_init = mbedtls_threading_set_alt_fake.arg0_val;
        mbedtls_threading_mutex_free = mbedtls_threading_set_alt_fake.arg1_val;
        mbedtls_threading_mutex_lock = mbedtls_threading_set_alt_fake.arg2_val;
        mbedtls_threading_mutex_unlock = mbedtls_threading_set_alt_fake.arg3_val;

        RESET_FAKE(mbedtls_threading_set_alt);
    }

    mbedtls_threading_mutex_t mutex = {.mutex = nullptr};
    void (*mbedtls_threading_mutex_init)(mbedtls_threading_mutex_t *);
    void (*mbedtls_threading_mutex_free)(mbedtls_threading_mutex_t *);
    int (*mbedtls_threading_mutex_lock)(mbedtls_threading_mutex_t *);
    int (*mbedtls_threading_mutex_unlock)(mbedtls_threading_mutex_t *);
};

class TestMbedTlsThreadingCmsisRtosInitializedMutex : public TestMbedTlsThreadingCmsisRtos {
public:
    TestMbedTlsThreadingCmsisRtosInitializedMutex() : TestMbedTlsThreadingCmsisRtos()
    {
        osMutexNew_fake.return_val = &mutex_id;

        mbedtls_threading_mutex_init(&mutex);

        RESET_FAKE(osMutexNew);
        RESET_FAKE(osMutexDelete);
        RESET_FAKE(osMutexAcquire);
        RESET_FAKE(osMutexRelease);
    }

    int mutex_id = 0xDEADBEEF;
};

TEST_F(TestMbedTlsThreadingCmsisRtos, failure_to_create_a_mutex_returns_a_nullptr_mutex_id)
{
    osMutexNew_fake.return_val = nullptr;

    mbedtls_threading_mutex_init(&mutex);

    EXPECT_EQ(nullptr, mutex.mutex);
}

TEST_F(TestMbedTlsThreadingCmsisRtos, locking_a_mutex_that_was_not_created_returns_failure)
{
    EXPECT_NE(0, mbedtls_threading_mutex_lock(&mutex));
}

TEST_F(TestMbedTlsThreadingCmsisRtos, unlocking_a_mutex_that_was_not_created_returns_failure)
{
    EXPECT_NE(0, mbedtls_threading_mutex_unlock(&mutex));
}

TEST_F(TestMbedTlsThreadingCmsisRtosInitializedMutex, create_an_already_created_mutex_id_leaves_the_mutex_id_as_it_was)
{
    EXPECT_EQ(&mutex_id, mutex.mutex);
    mbedtls_threading_mutex_init(&mutex);
    EXPECT_EQ(&mutex_id, mutex.mutex);
}

TEST_F(TestMbedTlsThreadingCmsisRtosInitializedMutex, successfully_freeing_acquired_mutex_frees_the_mutex_id)
{
    osMutexDelete_fake.return_val = osOK;

    mbedtls_threading_mutex_free(&mutex);
    EXPECT_EQ(nullptr, mutex.mutex);
}

TEST_F(TestMbedTlsThreadingCmsisRtosInitializedMutex, freeing_an_already_freed_mutex_leaves_mutex_id_as_nullptr)
{
    osMutexDelete_fake.return_val = osOK;

    mbedtls_threading_mutex_free(&mutex);
    EXPECT_EQ(nullptr, mutex.mutex);

    mbedtls_threading_mutex_free(&mutex);
    EXPECT_EQ(nullptr, mutex.mutex);
}

TEST_F(TestMbedTlsThreadingCmsisRtosInitializedMutex, unlocking_a_locked_mutex_returns_successfully)
{
    osMutexAcquire_fake.return_val = osOK;
    osMutexRelease_fake.return_val = osOK;

    EXPECT_EQ(0, mbedtls_threading_mutex_lock(&mutex));
    EXPECT_EQ(0, mbedtls_threading_mutex_unlock(&mutex));
}

TEST_F(TestMbedTlsThreadingCmsisRtosInitializedMutex, locking_an_already_locked_mutex_fails)
{
    osStatus_t mutex_acquire_return_values[] = {osOK, osErrorResource};
    SET_RETURN_SEQ(osMutexAcquire,
                   mutex_acquire_return_values,
                   sizeof(mutex_acquire_return_values) / sizeof(mutex_acquire_return_values[0]));

    EXPECT_EQ(0, mbedtls_threading_mutex_lock(&mutex));
    EXPECT_EQ(MBEDTLS_ERR_THREADING_MUTEX_ERROR, mbedtls_threading_mutex_lock(&mutex));
}

TEST_F(TestMbedTlsThreadingCmsisRtosInitializedMutex, unlocking_an_already_unlocked_mutex_fails)
{
    osMutexRelease_fake.return_val = osErrorResource;
    EXPECT_EQ(MBEDTLS_ERR_THREADING_MUTEX_ERROR, mbedtls_threading_mutex_unlock(&mutex));
}
