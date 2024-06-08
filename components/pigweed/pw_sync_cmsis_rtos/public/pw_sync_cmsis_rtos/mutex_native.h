/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_MUTEX_NATIVE_H
#define PW_SYNC_CMSIS_RTOS_MUTEX_NATIVE_H

#include "cmsis_os2.h"
#include "pw_sync_cmsis_rtos/internal/mutex.h"

namespace pw::sync::backend {
using NativeMutex = cmsis_rtos::InternalMutex;
using NativeMutexHandle = osMutexId_t;
} // namespace pw::sync::backend

#endif // ! PW_SYNC_CMSIS_RTOS_MUTEX_NATIVE_H
