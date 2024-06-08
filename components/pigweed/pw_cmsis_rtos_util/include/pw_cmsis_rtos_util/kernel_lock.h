/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_CMSIS_RTOS_UTIL_INTERNAL_LOCK_GUARD_H
#define PW_CMSIS_RTOS_UTIL_INTERNAL_LOCK_GUARD_H

#ifdef __cplusplus

#include "cmsis_os2.h"
#include "pw_assert/check.h"

#include <cstdint>
#include <mutex>

namespace pw_cmsis_rtos_util {
/// Utility class for locking and restoring the lock state of the kernel.
struct KernelLock final {
    KernelLock() = default;

    void lock();

    void unlock();

    KernelLock(KernelLock &) = delete;
    KernelLock(KernelLock &&) = delete;
    void operator=(KernelLock &) = delete;
    void operator=(KernelLock &&) = delete;

private:
    using lock_state_t = int32_t;
    static int counter_;
    static lock_state_t prev_lock_;
};

/// Lock the kernel upon construction, restore the previous state upon destruction.
struct KernelLockGuard final {
    inline KernelLockGuard()
    {
        _lock.lock();
    }

    inline ~KernelLockGuard()
    {
        _lock.unlock();
    }

private:
    KernelLock _lock;
};
} // namespace pw_cmsis_rtos_util

#endif // __cplusplus

#endif // ! PW_CMSIS_RTOS_UTIL_INTERNAL_LOCK_GUARD_H
