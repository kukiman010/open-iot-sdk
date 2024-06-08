/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_CHRONO_CMSIS_RTOS_CONFIG_H
#define PW_CHRONO_CMSIS_RTOS_CONFIG_H

#ifndef PW_LOG_MODULE_NAME
#define PW_LOG_MODULE_NAME "sync_cmsis_rtos"
#endif

#include "pw_iot_sdk_config/config.h"

#define PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_WAIT_FLAG PW_IOTSDK_THREAD_NOTIFICATION_WAIT_FLAG
#define PW_SYNC_CMSIS_RTOS_SEMAPHORE_MAX                 PW_IOTSDK_SEMAPHORE_MAX

#ifdef __cplusplus
#include <cstdint>

#endif // ! __cplusplus

#endif // ! PW_CHRONO_CMSIS_RTOS_CONFIG_H
