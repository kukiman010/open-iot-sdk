/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

extern "C" {

#include "iotsdk/ip_network_api.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/xlogging.h"
}

#include "cmsis_os2.h"
#include "fff.h"
#include "gtest/gtest.h"
#include <stdint.h>

DEFINE_FFF_GLOBALS

class TestPlatformOpeniot : public ::testing::Test {
public:
    TestPlatformOpeniot()
    {
        RESET_FAKE(osEventFlagsNew);
        RESET_FAKE(osEventFlagsSet);
        RESET_FAKE(osEventFlagsWait);
        RESET_FAKE(osEventFlagsClear);
        RESET_FAKE(osEventFlagsDelete);
        RESET_FAKE(osMutexNew);
        RESET_FAKE(start_network_task);
        RESET_FAKE(osMutexAcquire);
        RESET_FAKE(osMutexRelease);
        RESET_FAKE(LogError);
        RESET_FAKE(LogInfo);
    }
};

class TestPlatformOpeniotPlatformInitReachesWaitForEvent : public TestPlatformOpeniot {
public:
    TestPlatformOpeniotPlatformInitReachesWaitForEvent() : TestPlatformOpeniot()
    {
        initialise_platform();
        RESET_FAKE(osMutexAcquire);
        RESET_FAKE(osMutexRelease);
    }

private:
    void initialise_platform(void)
    {
        // Successful platform_init() cannot be simulated in test suite as encapsulation
        // prevents manipulation of is_network_up function's return type
        // To maximize code coverage we simulate a partial platform_init
        uint32_t event_flag_id = 1;
        osEventFlagsId_t os_event_flag_id = &event_flag_id;
        osEventFlagsNew_fake.return_val = os_event_flag_id;
        osEventFlagsWait_fake.return_val = osOK;

        start_network_task_fake.return_val = osOK;

        uint32_t mutex_id = 1;
        osMutexId_t os_mutex_id = &mutex_id;
        osMutexNew_fake.return_val = &os_mutex_id;

        osMutexAcquire_fake.return_val = osOK;
        osMutexRelease_fake.return_val = osOK;

        EXPECT_EQ(osOK, platform_init());
    }
};

TEST_F(TestPlatformOpeniot, platform_init_returns_failure_if_new_event_flags_cannot_be_created)
{
    osEventFlagsNew_fake.return_val = nullptr;
    EXPECT_EQ(MU_FAILURE, platform_init());
}

TEST_F(TestPlatformOpeniot, platform_init_returns_failure_when_new_mutex_cannot_be_created)
{
    uint32_t flag_id = 1;
    osEventFlagsId_t os_event_flag_id = &flag_id;
    osEventFlagsNew_fake.return_val = os_event_flag_id;

    osMutexNew_fake.return_val = nullptr;
    EXPECT_EQ(MU_FAILURE, platform_init());
}

TEST_F(TestPlatformOpeniot, platform_init_returns_failure_when_network_task_cannot_be_started)
{
    int32_t flag_id = 1;
    osEventFlagsId_t os_event_flag_id = &flag_id;
    osEventFlagsNew_fake.return_val = os_event_flag_id;

    uint32_t mutex_id = 1;
    osMutexId_t os_mutex_id = &mutex_id;
    osMutexNew_fake.return_val = &os_mutex_id;

    start_network_task_fake.return_val = osError;

    EXPECT_EQ(osError, platform_init());
}

TEST_F(TestPlatformOpeniot, os_event_flag_is_deleted_when_new_mutex_cannot_be_created)
{
    uint32_t flag_id = 1;
    osEventFlagsId_t os_event_flag_id = &flag_id;
    osEventFlagsNew_fake.return_val = os_event_flag_id;
    osEventFlagsDelete_fake.return_val = osOK;

    osMutexNew_fake.return_val = nullptr;

    EXPECT_EQ(MU_FAILURE, platform_init());
    EXPECT_EQ(osEventFlagsDelete_fake.call_count, 1);
}

TEST_F(TestPlatformOpeniot, os_event_flag_is_deleted_when_network_task_cannot_be_started)
{
    uint32_t flag_id = 1;
    osEventFlagsId_t os_event_flag_id = &flag_id;
    osEventFlagsNew_fake.return_val = os_event_flag_id;

    uint32_t mutex_id = 1;
    osMutexId_t os_mutex_id = &mutex_id;
    osMutexNew_fake.return_val = &os_mutex_id;

    start_network_task_fake.return_val = osError;

    EXPECT_EQ(osError, platform_init());
    EXPECT_EQ(osEventFlagsDelete_fake.call_count, 1);
}

TEST_F(TestPlatformOpeniot, mutex_is_deleted_when_network_task_cannot_be_started)
{
    uint32_t flag_id = 1;
    osEventFlagsId_t os_event_flag_id = &flag_id;
    osEventFlagsNew_fake.return_val = os_event_flag_id;

    uint32_t mutex_id = 1;
    osMutexId_t os_mutex_id = &mutex_id;
    osMutexNew_fake.return_val = &os_mutex_id;

    start_network_task_fake.return_val = osError;

    EXPECT_EQ(osError, platform_init());
    EXPECT_EQ(osMutexDelete_fake.call_count, 1);
}

TEST_F(TestPlatformOpeniotPlatformInitReachesWaitForEvent, network_state_callback_exits_when_mutex_cannot_be_acquired)
{
    osMutexAcquire_fake.return_val = osError;

    start_network_task_fake.return_val = osOK;
    network_state_callback_t network_state_callback = start_network_task_fake.arg0_val;
    network_state_callback(NETWORK_UP);

    EXPECT_EQ(osMutexAcquire_fake.call_count, 1);
    EXPECT_EQ(osEventFlagsSet_fake.call_count, 0);
}

TEST_F(TestPlatformOpeniotPlatformInitReachesWaitForEvent,
       network_state_callback_sets_event_flag_when_mutex_is_acquired)
{
    osMutexAcquire_fake.return_val = osOK;
    osEventFlagsSet_fake.return_val = osError;

    start_network_task_fake.return_val = osOK;
    network_state_callback_t network_state_callback = start_network_task_fake.arg0_val;
    network_state_callback(NETWORK_UP);

    EXPECT_EQ(osMutexAcquire_fake.call_count, 1);
    EXPECT_EQ(osEventFlagsSet_fake.call_count, 1);
}

TEST_F(TestPlatformOpeniotPlatformInitReachesWaitForEvent,
       network_state_callback_releases_mutex_when_os_event_flags_is_set)
{
    osMutexAcquire_fake.return_val = osOK;
    osEventFlagsSet_fake.return_val = osOK;
    osMutexRelease_fake.return_val = osOK;

    start_network_task_fake.return_val = osOK;
    network_state_callback_t network_state_callback = start_network_task_fake.arg0_val;
    network_state_callback(NETWORK_UP);

    EXPECT_EQ(osEventFlagsSet_fake.call_count, 1);
    EXPECT_EQ(osMutexRelease_fake.call_count, 1);
}

TEST_F(TestPlatformOpeniotPlatformInitReachesWaitForEvent,
       network_state_callback_releases_mutex_when_os_event_flag_cannot_be_set)
{
    osMutexAcquire_fake.return_val = osOK;
    osEventFlagsSet_fake.return_val = osError;
    osMutexRelease_fake.return_val = osOK;

    network_state_callback_t network_state_callback = start_network_task_fake.arg0_val;
    start_network_task_fake.return_val = osOK;
    network_state_callback(NETWORK_UP);

    EXPECT_EQ(osEventFlagsSet_fake.call_count, 1);
    EXPECT_EQ(osMutexRelease_fake.call_count, 1);
}
