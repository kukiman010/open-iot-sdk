/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_CHRONO_CMSIS_RTOS_SYSTEM_TIMER_INLINE_H
#define PW_CHRONO_CMSIS_RTOS_SYSTEM_TIMER_INLINE_H

#ifdef __cplusplus

#include "pw_assert/assert.h"
#include "pw_assert/check.h"
#include "pw_chrono/system_timer.h"
#include "pw_chrono_cmsis_rtos/system_timer_native.h"
#include "pw_log/log.h"

namespace pw::chrono {
inline SystemTimer::SystemTimer(SystemTimer::ExpiryCallback &&callback)
    : native_type_(reinterpret_cast<pw::chrono::backend::NativeSystemTimer::Callback &&>(callback))
{
}

inline SystemTimer::~SystemTimer()
{
}

inline void SystemTimer::InvokeAfter(SystemClock::duration delay)
{
    InvokeAt(SystemClock::TimePointAfterAtLeast(delay));
}

inline void SystemTimer::InvokeAt(SystemClock::time_point timestamp)
{
    native_type_.Set(timestamp);
}

inline void SystemTimer::Cancel()
{
    native_type_.Unset();
}
} // namespace pw::chrono

#endif // __cplusplus

#endif // ! PW_CHRONO_CMSIS_RTOS_SYSTEM_TIMER_INLINE_H
