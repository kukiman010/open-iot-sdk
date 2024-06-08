/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_COUNTING_SEMAPHORE_H
#define PW_SYNC_CMSIS_RTOS_COUNTING_SEMAPHORE_H

#include "cmsis_os2.h"
#include "pw_sync_cmsis_rtos/internal/counting_semaphore.h"
#include "pw_sync_cmsis_rtos/internal/mutex.h"

#include <limits>

namespace pw::sync::backend {
using NativeCountingSemaphore = cmsis_rtos::InternalCountingSemaphore<ptrdiff_t, std::numeric_limits<ptrdiff_t>::max()>;

using NativeCountingSemaphoreHandle = osSemaphoreId_t;

inline constexpr NativeCountingSemaphore::counter_type kCountingSemaphoreMaxValue = NativeCountingSemaphore::max_count;
} // namespace pw::sync::backend

#endif // ! PW_SYNC_CMSIS_RTOS_COUNTING_SEMAPHORE_H
