/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "cmsis_os2.h"
#include "pw_assert/check.h"
#include "pw_cmsis_rtos_util/kernel_lock.h"
#include "pw_cmsis_rtos_util/repeat.h"
#include "pw_interrupt/context.h"
#include "pw_log/log.h"
#include "pw_sync/thread_notification.h"
#include "pw_sync_cmsis_rtos/config.h"

#include <cinttypes>
#include <limits>
#include <type_traits>

using pw_cmsis_rtos_util::KernelLock;
using pw_cmsis_rtos_util::KernelLockGuard;
using pw_cmsis_rtos_util::repeat_for_ticks;

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

namespace pw::sync::backend {
NativeThreadNotification::NativeThreadNotification() : blocked_thread_id_(nullptr), notified_(false)
{
}

bool NativeThreadNotification::try_acquire()
{
    KernelLockGuard guard;

    PW_CHECK(!interrupt::InInterruptContext(), "cannot be acquired from interrupt");
    PW_CHECK_PTR_EQ(blocked_thread_id_, nullptr, "cannot block multiple threads");

    if (notified_) {
        notified_ = false;
        return true;
    }

    return false;
}

bool NativeThreadNotification::try_acquire_for(int64_t ticks)
{
    uint32_t status = try_acquire_for_with_status(ticks);
    if (is_os_flags_error(status)) {
        return false;
    }

    return true;
}

void NativeThreadNotification::acquire()
{
    uint32_t status = try_acquire_for_with_status(osWaitForever);
    PW_CHECK(!is_os_flags_error(status), "osThreadFlagsWait failed: %#" PRIx32, status);
}

void NativeThreadNotification::release()
{
    KernelLock kernel;

    if (!interrupt::InInterruptContext()) {
        kernel.lock();
    }

    notified_ = true;
    if (blocked_thread_id_ != nullptr) {
        uint32_t status =
            osThreadFlagsSet(blocked_thread_id_, bit_mask(PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_WAIT_FLAG));
        PW_CHECK(!is_os_flags_error(status), "osThreadFlagsSet failed: %#" PRIx32, status);
    }

    if (!interrupt::InInterruptContext()) {
        kernel.unlock();
    }
}

uint32_t NativeThreadNotification::try_acquire_for_with_status(int64_t ticks)
{
    PW_CHECK(!interrupt::InInterruptContext(), "cannot be acquired from interrupt");
    PW_CHECK_PTR_EQ(blocked_thread_id_, nullptr, "cannot block multiple threads");

    osThreadId_t thread_id = osThreadGetId();
    uint32_t status;

    // First check if the thread was already notified before it subscribed. If it was we can return immediately.
    {
        KernelLockGuard guard;
        if (notified_) {
            blocked_thread_id_ = nullptr;
            notified_ = false;
            return 0;
        }

        blocked_thread_id_ = thread_id;

        status = osThreadFlagsClear(bit_mask(PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_WAIT_FLAG));
        PW_CHECK(!is_os_flags_error(status), "osThreadFlagsClear failed: %#" PRIx32, status);
    }

    // Otherwise wait for the flags to be set.
    if (ticks == 0) {
        // For ticks == 0 call FlagsWait once directly.
        status = osThreadFlagsWait(
            bit_mask(PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_WAIT_FLAG), osFlagsWaitAll | osFlagsNoClear, 0);
    } else if (ticks == osWaitForever) {
        // For ticks == osWaitForever call FlagsWait repeatedly.
        // NB: With the combination of RTX and GCC, osThreadFlagsWait does not respect osWaitForever, so we loop it.
        // We pass the osFlagsNoClear flag because otherwise the first osThreadFlagsWait will clear the flags despite
        // returning an error status and make us loop forever.
        while (true) {
            status = osThreadFlagsWait(bit_mask(PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_WAIT_FLAG),
                                       osFlagsWaitAll | osFlagsNoClear,
                                       osWaitForever);

            // Ignore timeout error.
            if (status == osFlagsErrorTimeout) {
                continue;
            }

            // Stop on success or when result is error other than timeout.
            if (is_bit_set(status, PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_WAIT_FLAG) || is_os_flags_error(status)) {
                break;
            }
        }
    } else {
        // For other values of ticks call FlagsWait repeatedly until ticks are consumed.
        status = repeat_for_ticks<uint32_t>(ticks, [](uint32_t ticks_in, const uint32_t &ticks_out, uint32_t &result) {
            (void)ticks_out;

            result = osThreadFlagsWait(
                bit_mask(PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_WAIT_FLAG), osFlagsWaitAll | osFlagsNoClear, ticks_in);

            // Stop (return false) when result is error other than timeout.
            if (is_os_flags_error(result)) {
                return result == osFlagsErrorTimeout;
            }

            // Stop when bit toggled
            return !is_bit_set(result, PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_WAIT_FLAG);
        });
    }

    if (is_os_flags_error(status)) {
        return status;
    }

    // Awaken the thread.
    blocked_thread_id_ = nullptr;
    notified_ = false;
    status = osThreadFlagsClear(bit_mask(PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_WAIT_FLAG));

    return status;
}
} // namespace pw::sync::backend
