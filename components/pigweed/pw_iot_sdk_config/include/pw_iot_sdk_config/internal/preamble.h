/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_IOT_SDK_CONFIG_INTERNAL_PREAMBLE_H
#define PW_IOT_SDK_CONFIG_INTERNAL_PREAMBLE_H

/// Minimum size of the timer thread stack.
#define _PW_IOTSDK_TIMER_THREAD_MIN_STACK_SIZE 2048

#if defined(PW_IOTSDK_CMSIS_RTX) && PW_IOTSDK_CMSIS_RTX
#include "RTE_Components.h"
#include "RTX_Config.h"
#include "rtx_os.h"
#elif defined(PW_IOTSDK_FREERTOS) && PW_IOTSDK_FREERTOS
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#endif

#endif // ! PW_IOT_SDK_CONFIG_INTERNAL_PREAMBLE_H
