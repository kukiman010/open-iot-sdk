/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "pw_chrono/system_timer.h"

#include "cmsis_os2.h"
#include "pw_assert/assert.h"
#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_chrono_cmsis_rtos/config.h"
#include "pw_cmsis_rtos_util/kernel_lock.h"
#include "pw_log/log.h"

#include <algorithm>
#include <chrono>
#include <cinttypes>
#include <limits>
#include <mutex>
#include <utility>

using ::pw_cmsis_rtos_util::KernelLockGuard;

#if PW_IOTSDK_FREERTOS
// CMSIS RTOS for FreeRTOS has a bug where a one-shot timer cannot be restarted and stopped in its callback as is
// done in the facade test "SystemTimer.RescheduleFromCallback". As a workaround we can use a periodic timer and
// cancel it ourselves when it fires.
#define osTimerTypeForRTOS osTimerPeriodic
#define UnsetAfterInvoke() Unset()
#else
// Since the bug doesn't occur on RTX we do it properly with a one-shot timer.
#define osTimerTypeForRTOS osTimerOnce
#define UnsetAfterInvoke() \
    do {                   \
        _set = false;      \
    } while (0)
#endif

namespace pw::chrono::backend {
// Simple wrapper callback that invokes the timer.
void NativeSystemTimer::TimerCallback(void *native_timer_ptr)
{
    PW_ASSERT(native_timer_ptr != nullptr);

    auto timer = reinterpret_cast<NativeSystemTimer *>(native_timer_ptr);

    timer->Invoke();
}

NativeSystemTimer::NativeSystemTimer(Callback &&callback) : _callback(std::move(callback)), _set(false)
{
    _timer_id = osTimerNew(TimerCallback, osTimerTypeForRTOS, reinterpret_cast<void *>(this), nullptr);

    PW_CHECK_PTR_NE(_timer_id, nullptr, "osTimerNew failed");
}

NativeSystemTimer::~NativeSystemTimer()
{
    PW_CHECK_PTR_NE(_timer_id, nullptr);
    Unset();

    osStatus_t res = osTimerDelete(_timer_id);
    PW_CHECK_INT_EQ(res, osOK, "osTimerDelete failed");
}

void NativeSystemTimer::Set(SystemClock::time_point deadline)
{
    KernelLockGuard guard;

    // Deadline has to be provided to the user callback verbatim, even if we "stretch" it.
    _deadline = deadline;

    // Cancel any previous callback.
    Unset();

    // Clamp ticks to between 1 and UINT32_MAX.
    //
    // The minimum wait is 1 tick so that if the deadline falls on the current tick or a tick that elapsed already, we
    // invoke the callback on the next tick (per API for InvokeAt()).
    //
    // osTimerStart only accepts 32-bit ticks so when the deadline is further away we clamp downwards. Invoke() will
    // re-start the timer for the remaining time.
    //
    // Note for TICKS_HI - it's not documented whether osWaitForever applies to the ticks value in osTimerStart() so
    // assuming it does we use a max of one less than UINT32_MAX.
    static constexpr int64_t TICKS_LO = 1;
    static constexpr int64_t TICKS_HI = static_cast<int64_t>(osWaitForever - 1);
    int64_t ticks_delta = (deadline - SystemClock::now()).count();
    uint32_t ticks = static_cast<uint32_t>(std::clamp(ticks_delta, TICKS_LO, TICKS_HI));

    // (Re-)Set the timer.
    _set = true;
    osStatus_t res = osTimerStart(_timer_id, ticks);
    PW_CHECK_INT_EQ(res, osOK, "osTimerStart failed");
}

void NativeSystemTimer::Unset()
{
    KernelLockGuard guard;

    if (!_set) {
        return;
    }

    osStatus_t res = osTimerStop(_timer_id);
    PW_CHECK_INT_EQ(res, osOK, "osTimerStop failed");
    _set = false;
}

void NativeSystemTimer::Invoke()
{
    SystemClock::time_point deadline;

    {
        KernelLockGuard guard;

        deadline = _deadline;

        // Do not invoke if the timer was cancelled.
        if (!_set) {
            return;
        }

        // Do not invoke if the duration has not elapsed (see comments in Set()).
        if (SystemClock::now() < deadline) {
            Set(deadline);
            return;
        }

        UnsetAfterInvoke();
    }

    _callback(deadline);
}

} // namespace pw::chrono::backend
