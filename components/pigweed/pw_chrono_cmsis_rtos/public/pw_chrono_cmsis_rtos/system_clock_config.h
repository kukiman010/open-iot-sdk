/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_CHRONO_CMSIS_RTOS_SYSTEM_CLOCK_CONFIG_H
#define PW_CHRONO_CMSIS_RTOS_SYSTEM_CLOCK_CONFIG_H

#include "cmsis_os2.h"
#include "pw_iot_sdk_config/config.h"

#define PW_CHRONO_SYSTEM_CLOCK_PERIOD_SECONDS_NUMERATOR   (1)
#define PW_CHRONO_SYSTEM_CLOCK_PERIOD_SECONDS_DENOMINATOR (PW_IOTSDK_RTOS_TICK_RATE_HZ)

#ifdef __cplusplus

#include "pw_chrono/epoch.h"
#include "pw_chrono/system_clock.h"

namespace pw::chrono::backend {
// This backend handles 32-bit overflow of the CMSIS RTOS counter, so the times reported are always ticks since boot.
inline constexpr Epoch kSystemClockEpoch = pw::chrono::Epoch::kTimeSinceBoot;

// The backend is not NMI safe.
inline constexpr bool kSystemClockNmiSafe = false;

// The system clock does not tick when the systick interrupt is masked.
inline constexpr bool kSystemClockFreeRunning = false;
} // namespace pw::chrono::backend

#endif // __cplusplus

#endif // ! PW_CHRONO_CMSIS_RTOS_SYSTEM_CLOCK_CONFIG_H
