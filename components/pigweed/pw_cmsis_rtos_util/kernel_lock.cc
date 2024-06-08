/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "pw_cmsis_rtos_util/kernel_lock.h"

#include "cmsis_os2.h"
#include "pw_assert/check.h"

#include <cstdint>
#include <mutex>

namespace pw_cmsis_rtos_util {
int KernelLock::counter_(0);
KernelLock::lock_state_t KernelLock::prev_lock_(-1);

void KernelLock::lock()
{
    if (counter_ == 0) {
        // If we are pre-empted here, the pre-empting thread will also find counter to be zero and the lock/unlock cycle
        // will complete before we get back here. Or we pre-empt it in turn before it locks the kernel in which case
        // our lock/unlock cycle completes before it locks the kernel again.
        prev_lock_ = osKernelLock();
        PW_CHECK_INT_GE(prev_lock_, 0, "osKernelLock failed");
    }

    counter_++;
}

void KernelLock::unlock()
{
    counter_--;
    if (counter_ == 0) {
        int32_t res = osKernelRestoreLock(prev_lock_);
        PW_CHECK_INT_GE(res, 0, "osKernelRestoreLock failed");
    }
}
} // namespace pw_cmsis_rtos_util
