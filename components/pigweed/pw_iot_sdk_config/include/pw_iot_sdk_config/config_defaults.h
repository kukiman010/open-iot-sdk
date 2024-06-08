/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_IOT_SDK_CONFIG_DEFAULT_H
#define PW_IOT_SDK_CONFIG_DEFAULT_H

////////////////////////////////////
// CHRONO
////////////////////////////////////

/// PW_IOTSDK_RTOS_TICK_RATE_HZ
/// Must be set to RTOS tick rate.
/// Must be overridden when RTOS is not RTX or FreeRTOS.
#ifndef PW_IOTSDK_RTOS_TICK_RATE_HZ
#if PW_IOTSDK_CMSIS_RTX
#define PW_IOTSDK_RTOS_TICK_RATE_HZ OS_TICK_FREQ
#elif PW_IOTSDK_FREERTOS
#define PW_IOTSDK_RTOS_TICK_RATE_HZ configTICK_RATE_HZ
#else
#error RTOS is not known, PW_IOTSDK_RTOS_TICK_RATE_HZ must be user-supplied
#endif // PW_IOTSDK_CMSIS_RTX
#endif // ! PW_IOTSDK_RTOS_TICK_RATE_HZ

////////////////////////////////////
// LOG
////////////////////////////////////

/// PW_IOTSDK_LOG_SHOW_FILENAME
/// Controls whether log backends include filename in log messages.
#ifndef PW_IOTSDK_LOG_SHOW_FILENAME
#define PW_IOTSDK_LOG_SHOW_FILENAME 0
#endif // PW_IOTSDK_LOG_SHOW_FILENAME

/// PW_IOTSDK_LOG_TIMESTAMP
/// Controls whether log backends include timestamps in log messages.
#ifndef PW_IOTSDK_LOG_TIMESTAMP
#define PW_IOTSDK_LOG_TIMESTAMP 0
#endif // PW_IOTSDK_LOG_TIMESTAMP

/// PW_IOTSDK_LOG_LINE_LENGTH
/// Controls maximum log line length.
#ifndef PW_IOTSDK_LOG_LINE_LENGTH
#define PW_IOTSDK_LOG_LINE_LENGTH 150
#endif // PW_IOTSDK_LOG_LINE_LENGTH

/// PW_IOTSDK_LOG_MODULE_LENGTH
/// Controls space left for module name.
#ifndef PW_IOTSDK_LOG_MODULE_LENGTH
#define PW_IOTSDK_LOG_MODULE_LENGTH 4
#endif // PW_IOTSDK_LOG_MODULE_LENGTH

////////////////////////////////////
// THREAD
////////////////////////////////////

/// PW_IOTSDK_THREAD_JOINING_ENABLED
/// Whether to enable thread joining. This does not related to whether the RTOS supports it (see
/// @ref PW_IOTSDK_RTOS_CAN_JOIN_DETACH).
#ifndef PW_IOTSDK_THREAD_JOINING_ENABLED
#define PW_IOTSDK_THREAD_JOINING_ENABLED 1
#endif // ! PW_IOTSDK_THREAD_JOINING_ENABLED

/// PW_IOTSDK_RTOS_CAN_JOIN_DETACH
/// Whether the RTOS supports join and detach (CMSIS RTOS API provides join and detach when the underlying RTOS is RTX
/// but not FreeRTOS). If not, the IoT SDK pw_thread backend provides them.
#if PW_IOTSDK_THREAD_JOINING_ENABLED
#ifndef PW_IOTSDK_RTOS_CAN_JOIN_DETACH
#if PW_IOTSDK_CMSIS_RTX
#define PW_IOTSDK_RTOS_CAN_JOIN_DETACH 1
#elif PW_IOTSDK_FREERTOS
#define PW_IOTSDK_RTOS_CAN_JOIN_DETACH 0
#else
#define PW_IOTSDK_RTOS_CAN_JOIN_DETACH 0
#warning RTOS is not known, assuming join and detach need to be supplied (PW_IOTSDK_RTOS_CAN_JOIN_DETACH = 0)
#endif // PW_IOTSDK_CMSIS_RTX
#endif // ! PW_IOTSDK_RTOS_CAN_JOIN_DETACH
#endif // PW_IOTSDK_THREAD_JOINING_ENABLED

////////////////////////////////////
// SYNC
////////////////////////////////////

/// PW_IOTSDK_SEMAPHORE_MAX
/// Set to maximum semaphore count.
/// Must be overridden when RTOS is not RTX or FreeRTOS.
#ifndef PW_IOTSDK_SEMAPHORE_MAX
#if PW_IOTSDK_CMSIS_RTX
#define PW_IOTSDK_SEMAPHORE_MAX osRtxSemaphoreTokenLimit
#elif PW_IOTSDK_FREERTOS
#define PW_IOTSDK_SEMAPHORE_MAX SIZE_MAX
#else
#error RTOS is not known, PW_IOTSDK_SEMAPHORE_MAX must be user-supplied
#endif // PW_IOTSDK_CMSIS_RTX
#endif // ! PW_IOTSDK_SEMAPHORE_MAX

/// PW_IOTSDK_THREAD_NOTIFICATION_WAIT_FLAG
/// CMSIS RTOS thread flag to use for thread notifications. Must be set to a bit not used elsewhere in the application.
#ifndef PW_IOTSDK_THREAD_NOTIFICATION_WAIT_FLAG
#define PW_IOTSDK_THREAD_NOTIFICATION_WAIT_FLAG 13
#endif // PW_IOTSDK_THREAD_NOTIFICATION_WAIT_FLAG

/// PW_IOTSDK_MUTEX_WAIT_FLAG
/// CMSIS RTOS thread flag to use for mutex unlock notifications.
#ifndef PW_IOTSDK_MUTEX_WAIT_FLAG
#define PW_IOTSDK_MUTEX_WAIT_FLAG 14
#endif // PW_IOTSDK_MUTEX_WAIT_FLAG

////////////////////////////////////
// SYS IO
////////////////////////////////////

/// PW_IOTSDK_SYS_IO_TX_BUFFER_SIZE
/// Size of TX buffer in bytes.
/// PW_IOTSDK_SYS_IO_TX_BUFFER_SIZE > 0
#ifndef PW_IOTSDK_SYS_IO_TX_BUFFER_SIZE
#define PW_IOTSDK_SYS_IO_TX_BUFFER_SIZE 100
#endif

/// PW_IOTSDK_SYS_IO_RX_BUFFER_SIZE
/// Size of RX buffer in bytes.
/// PW_IOTSDK_SYS_IO_RX_BUFFER_SIZE > 0
#ifndef PW_IOTSDK_SYS_IO_RX_BUFFER_SIZE
#define PW_IOTSDK_SYS_IO_RX_BUFFER_SIZE 100
#endif

/// PW_IOTSDK_SYS_IO_PERIPHERAL_RX_BUFFER_SIZE
/// Size of the peripheral RX buffer in bytes.
/// PW_IOTSDK_SYS_IO_RX_BUFFER_SIZE > 0
#ifndef PW_IOTSDK_SYS_IO_PERIPHERAL_RX_BUFFER_SIZE
#define PW_IOTSDK_SYS_IO_PERIPHERAL_RX_BUFFER_SIZE 64
#endif

/// PW_IOTSDK_SYS_IO_ABORT_ON_OVERFLOW
/// Abort when an overflow is detected.
/// PW_IOTSDK_SYS_IO_ABORT_ON_OVERFLOW 1 | 0
#ifndef PW_IOTSDK_SYS_IO_ABORT_ON_OVERFLOW
#define PW_IOTSDK_SYS_IO_ABORT_ON_OVERFLOW 0
#endif

#endif // ! PW_IOT_SDK_CONFIG_DEFAULT_H
