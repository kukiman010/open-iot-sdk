/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_CHRONO_CMSIS_RTOS_SYSTEM_TIMER_NATIVE_H
#define PW_CHRONO_CMSIS_RTOS_SYSTEM_TIMER_NATIVE_H

#include "cmsis_os2.h"
#include "pw_chrono/system_clock.h"
#include "pw_function/function.h"

namespace pw::chrono::backend {
// Thread-safe wrapper around osTimer.
struct NativeSystemTimer final {
    // This is needed to avoid a circular dependency between this header and pw_chrono/system_timer.h
    using Callback = pw::Function<void(SystemClock::time_point)>;

    NativeSystemTimer(Callback &&callback);

    ~NativeSystemTimer();

    void Set(SystemClock::time_point deadline);

    void Unset();

private:
    osTimerId_t _timer_id;
    const Callback _callback;
    SystemClock::time_point _deadline;
    bool _set;

    void Invoke();

    static void TimerCallback(void *native_timer_ptr);
};

using NativeSystemTimerHandle = NativeSystemTimer *;
} // namespace pw::chrono::backend

#endif // ! PW_CHRONO_CMSIS_RTOS_SYSTEM_TIMER_NATIVE_H
