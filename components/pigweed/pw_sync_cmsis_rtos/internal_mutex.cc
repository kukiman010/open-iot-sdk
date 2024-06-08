/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "cmsis_os2.h"
#include "pw_assert/check.h"
#include "pw_cmsis_rtos_util/kernel_lock.h"
#include "pw_log/log.h"
#include "pw_preprocessor/compiler.h"
#include "pw_sync_cmsis_rtos/config.h"
#include "pw_sync_cmsis_rtos/internal/mutex.h"

#include <cinttypes>

static inline uint32_t bit_mask(uint32_t pos)
{
    return 1 << pos;
}

static inline bool is_bit_set(uint32_t x, uint32_t pos)
{
    uint32_t m = bit_mask(pos);
    return (x & m) == m;
}

static inline bool is_os_flags_error(uint32_t x)
{
    return x >= osFlagsError;
}

namespace pw::sync::cmsis_rtos {
InternalMutex::OsMutexWrapper::OsMutexWrapper() : _mutex(osMutexNew(nullptr))
{
    PW_CHECK_PTR_NE(_mutex, nullptr, "osMutexNew failed");
}

InternalMutex::OsMutexWrapper::~OsMutexWrapper()
{
    osStatus_t res = osMutexDelete(_mutex);
    PW_CHECK_INT_EQ(res, osOK, "osMutexDelete failed");
}

osMutexId_t InternalMutex::OsMutexWrapper::mutex_id()
{
    return _mutex;
}

void InternalMutex::OsMutexWrapper::lock()
{
    osStatus_t res = osMutexAcquire(_mutex, osWaitForever);
    PW_CHECK_INT_EQ(res, osOK, "osMutexAcquire failed");
}

void InternalMutex::OsMutexWrapper::unlock()
{
    osStatus_t res = osMutexRelease(_mutex);
    PW_CHECK_INT_EQ(res, osOK, "osMutexRelease failed");
}

InternalMutex::InternalMutex() : _owner(nullptr)
{
}

osMutexId_t InternalMutex::mutex_id()
{
    return _mutex.mutex_id();
}

bool InternalMutex::try_lock()
{
    _mutex.lock();

    osThreadId_t this_thread = osThreadGetId();

    // Mutex is non-recursive so the same thread attempting to reacquire the lock is a bug.
    PW_DCHECK_PTR_NE(_owner, this_thread, "double acquire");

    // The lock is already owned.
    if (_owner != nullptr) {
        _mutex.unlock();
        return false;
    }

    // Take ownership of the lock.
    _owner = this_thread;
    uint32_t status = osThreadFlagsClear(bit_mask(PW_SYNC_CMSIS_RTOS_MUTEX_WAIT_FLAG));
    PW_CHECK(!is_os_flags_error(status), "osThreadFlagsClear failed: %#" PRIx32, status);

    _mutex.unlock();
    return true;
}

void InternalMutex::lock()
{
    _mutex.lock();

    // Check for double lock.
    osThreadId_t this_thread = osThreadGetId();
    PW_DCHECK_PTR_NE(_owner, this_thread, "double acquire");

    uint32_t status;

    // Wait for the unlock flag to be set.
    while (_owner != nullptr) {
        {
            _mutex.unlock();

            status = osThreadFlagsWait(
                bit_mask(PW_SYNC_CMSIS_RTOS_MUTEX_WAIT_FLAG), osFlagsWaitAll | osFlagsNoClear, osWaitForever);

            _mutex.lock();
        }

        // Ignore timeout error.
        if (status == osFlagsErrorTimeout) {
            continue;
        }

        // Fail on other error.
        PW_CHECK(!is_os_flags_error(status), "osThreadFlagsWait failed: %#" PRIx32, status);

        // Stop when the bit is set.
        if (is_bit_set(status, PW_SYNC_CMSIS_RTOS_MUTEX_WAIT_FLAG)) {
            break;
        }
    }

    // Acquire the lock.
    _owner = this_thread;

    status = osThreadFlagsClear(bit_mask(PW_SYNC_CMSIS_RTOS_MUTEX_WAIT_FLAG));
    PW_CHECK(!is_os_flags_error(status), "osThreadFlagsClear failed: %#" PRIx32, status);

    _mutex.unlock();
}

void InternalMutex::unlock()
{
    _mutex.lock();

    PW_CHECK_NOTNULL(_owner, "released unowned lock");

    // For InternalMutex, only the owning thread may release the lock.
    osThreadId_t this_thread = osThreadGetId();
    PW_CHECK_PTR_EQ(_owner, this_thread, "released by non-owning thread");

    // Release the lock.
    osThreadId_t blocked_thread = _owner;
    _owner = nullptr;
    _mutex.unlock();

    // Wake the thread if it is waiting.
    uint32_t status = osThreadFlagsSet(blocked_thread, bit_mask(PW_SYNC_CMSIS_RTOS_MUTEX_WAIT_FLAG));
    PW_CHECK(!is_os_flags_error(status), "osThreadFlagsSet failed: %#" PRIx32, status);
}

} // namespace pw::sync::cmsis_rtos
