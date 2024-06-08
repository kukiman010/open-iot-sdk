/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_THREAD_CMSIS_RTOS_ID_NATIVE_H_
#define PW_THREAD_CMSIS_RTOS_ID_NATIVE_H_

#include "cmsis_os2.h"

#ifdef __cplusplus
namespace pw::thread::backend {
using NativeId = osThreadId_t;
}

#endif

#endif /* PW_THREAD_CMSIS_RTOS_ID_NATIVE_H_ */
