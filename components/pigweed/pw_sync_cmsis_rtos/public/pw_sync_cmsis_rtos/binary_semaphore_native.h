/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_BINARY_SEMAPHORE_NATIVE_H
#define PW_SYNC_CMSIS_RTOS_BINARY_SEMAPHORE_NATIVE_H

#include "cmsis_os2.h"
#include "pw_sync_cmsis_rtos/internal/counting_semaphore.h"
#include "pw_sync_cmsis_rtos/internal/mutex.h"

namespace pw::sync::backend {
using NativeBinarySemaphore = cmsis_rtos::InternalCountingSemaphore<ptrdiff_t, 1>;

using NativeBinarySemaphoreHandle = osSemaphoreId_t;

inline constexpr NativeBinarySemaphore::counter_type kBinarySemaphoreMaxValue = 1;
} // namespace pw::sync::backend

#endif // ! PW_SYNC_CMSIS_RTOS_BINARY_SEMAPHORE_NATIVE_H
