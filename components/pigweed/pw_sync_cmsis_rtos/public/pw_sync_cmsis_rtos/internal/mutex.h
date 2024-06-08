/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_INTERNAL_MUTEX_H
#define PW_SYNC_CMSIS_RTOS_INTERNAL_MUTEX_H

#include "cmsis_os2.h"
#include "pw_assert/check.h"
#include "pw_iot_sdk_config/config.h"

#define PW_SYNC_CMSIS_RTOS_MUTEX_WAIT_FLAG PW_IOTSDK_MUTEX_WAIT_FLAG

namespace pw::sync::cmsis_rtos {
// Implements try_acquire and release from the pw::sync::Mutex class as a wrapper around osMutex. Only the thread which
// owns the lock may unlock it.
struct InternalMutex final {
    InternalMutex();

    void unlock();

    bool try_lock();

    bool try_lock_for(int64_t ticks);

    void lock();

    osMutexId_t mutex_id();

    InternalMutex(const InternalMutex &) = delete;
    InternalMutex(InternalMutex &&) = delete;
    InternalMutex &operator=(const InternalMutex &) = delete;
    InternalMutex &operator=(InternalMutex &&) = delete;

protected:
    struct OsMutexWrapper final {
        OsMutexWrapper();

        ~OsMutexWrapper();

        void lock();

        void unlock();

        osMutexId_t mutex_id();

    private:
        osMutexId_t _mutex;
    };

    OsMutexWrapper _mutex;
    osThreadId_t _owner;
};

} // namespace pw::sync::cmsis_rtos

#endif // ! PW_SYNC_CMSIS_RTOS_INTERNAL_MUTEX_H
