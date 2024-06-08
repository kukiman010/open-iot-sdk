/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_IOT_SDK_CONFIG_INTERNAL_VALIDATION_H
#define PW_IOT_SDK_CONFIG_INTERNAL_VALIDATION_H

#ifndef __cplusplus
#include <assert.h>
#endif // ! __cplusplus

// Timer thread stack size validation
#if PW_IOTSDK_CMSIS_RTX
#ifdef OS_TIMER_THREAD_STACK_SIZE
static_assert(OS_TIMER_THREAD_STACK_SIZE >= _PW_IOTSDK_TIMER_THREAD_MIN_STACK_SIZE,
              "OS_TIMER_THREAD_STACK_SIZE must be at least _PW_IOTSDK_TIMER_THREAD_MIN_STACK_SIZE bytes");
#else
#error OS_TIMER_THREAD_STACK_SIZE must be defined
#endif // OS_TIMER_THREAD_STACK_SIZE
#elif PW_IOTSDK_FREERTOS
#ifdef configTIMER_TASK_STACK_DEPTH
static_assert(4 * configTIMER_TASK_STACK_DEPTH >= _PW_IOTSDK_TIMER_THREAD_MIN_STACK_SIZE,
              "configTIMER_TASK_STACK_DEPTH must be at least _PW_IOTSDK_TIMER_THREAD_MIN_STACK_SIZE bytes");
#else
#error configTIMER_TASK_STACK_DEPTH must be defined
#endif // configTIMER_TASK_STACK_DEPTH
#endif // PW_IOTSDK_CMSIS_RTX

// Wait flag validation
static_assert(PW_IOTSDK_THREAD_NOTIFICATION_WAIT_FLAG != PW_IOTSDK_MUTEX_WAIT_FLAG,
              "PW_IOTSDK_THREAD_NOTIFICATION_WAIT_FLAG and PW_IOTSDK_MUTEX_WAIT_FLAG must have distinct values");

// sys_io buffer size validation.
static_assert(PW_IOTSDK_SYS_IO_TX_BUFFER_SIZE > 0, "sys_io Tx buffer size must be strictly positive.");
static_assert(PW_IOTSDK_SYS_IO_RX_BUFFER_SIZE > 0, "sys_io Rx buffer size must be strictly positive.");
static_assert(PW_IOTSDK_SYS_IO_PERIPHERAL_RX_BUFFER_SIZE > 0,
              "sys_io peripheral Rx buffer size must be strictly positive.");

#endif // ! PW_IOT_SDK_CONFIG_INTERNAL_VALIDATION_H
