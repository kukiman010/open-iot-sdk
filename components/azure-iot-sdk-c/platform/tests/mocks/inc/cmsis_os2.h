/*
 * Copyright (c) 2022 - 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CMSIS_OS2_H_
#define CMSIS_OS2_H_

#include "fff.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define osFlagsWaitAny 0x00000000U ///< Wait for any flag (default).
#define osFlagsWaitAll 0x00000001U ///< Wait for all flags.
#define osFlagsNoClear 0x00000002U ///< Do not clear flags which have been specified to wait for.
#define osWaitForever  0xFFFFFFFFU

typedef enum {
    osOK = 0,              ///< Operation completed successfully.
    osError = -1,          ///< Unspecified RTOS error: run-time error but no other error message fits.
    osErrorTimeout = -2,   ///< Operation not completed within the timeout period.
    osErrorResource = -3,  ///< Resource not available.
    osErrorParameter = -4, ///< Parameter error.
    osErrorNoMemory =
        -5,          ///< System is out of memory: it was impossible to allocate or reserve memory for the operation.
    osErrorISR = -6, ///< Not allowed in ISR context: the function cannot be called from interrupt service routines.
    osStatusReserved = 0x7FFFFFFF ///< Prevents enum down-size compiler optimization.
} osStatus_t;

typedef void *osEventFlagsId_t;
typedef struct {
    const char *name;   ///< name of the event flags
    uint32_t attr_bits; ///< attribute bits
    void *cb_mem;       ///< memory for control block
    uint32_t cb_size;   ///< size of provided memory for control block
} osEventFlagsAttr_t;

typedef struct {
    const char *name;   ///< name of the mutex
    uint32_t attr_bits; ///< attribute bits
    void *cb_mem;       ///< memory for control block
    uint32_t cb_size;   ///< size of provided memory for control block
} osMutexAttr_t;

typedef void *osMutexId_t;

DECLARE_FAKE_VALUE_FUNC(osStatus_t, osEventFlagsDelete, osEventFlagsId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsSet, osEventFlagsId_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(osEventFlagsId_t, osEventFlagsNew, const osEventFlagsAttr_t *);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsClear, osEventFlagsId_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsWait, osEventFlagsId_t, uint32_t, uint32_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osKernelGetTickCount);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osKernelGetTickFreq);

DECLARE_FAKE_VALUE_FUNC(osMutexId_t, osMutexNew, const osMutexAttr_t *);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMutexRelease, osMutexId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMutexAcquire, osMutexId_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMutexDelete, osMutexId_t);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CMSIS_OS2_H_
