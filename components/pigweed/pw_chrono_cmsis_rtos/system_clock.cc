/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "pw_chrono/system_clock.h"

#include "cmsis_os2.h"
#include "pw_chrono_cmsis_rtos/config.h"
#include "pw_sync/interrupt_spin_lock.h"

namespace pw::chrono::backend {

using native_tick_type_t = uint32_t;
static sync::InterruptSpinLock spinlock;
static native_tick_type_t tick_count_high;
static native_tick_type_t tick_count_low;

int64_t GetSystemClockTickCount()
{
    spinlock.lock();

    auto new_tick_count = static_cast<int64_t>(osKernelGetTickCount());

    // The native tick count overflows every UINT32_MAX ticks (~49 days for 1 tick = 1 ms). Provided this function gets
    // called at least twice in that interval we can accurately handle overflow.
    if (new_tick_count < tick_count_low) {
        tick_count_high++;
    }

    tick_count_low = new_tick_count;

    auto ret = (static_cast<int64_t>(tick_count_high) << 32) | tick_count_low;

    spinlock.unlock();
    return ret;
}

} // namespace pw::chrono::backend
