/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_THREAD_CMSIS_RTOS_CONFIG_H
#define PW_THREAD_CMSIS_RTOS_CONFIG_H

#ifndef PW_LOG_MODULE_NAME
#define PW_LOG_MODULE_NAME "thread_iotsdk"
#endif // ! PW_LOG_MODULE_NAME

#include "pw_iot_sdk_config/config.h"

#define PW_THREAD_JOINING_ENABLED             PW_IOTSDK_THREAD_JOINING_ENABLED
#define PW_THREAD_CMSIS_RTOS_HAVE_JOIN_DETACH PW_IOTSDK_RTOS_CAN_JOIN_DETACH

#endif // ! PW_THREAD_CMSIS_RTOS_CONFIG_H
