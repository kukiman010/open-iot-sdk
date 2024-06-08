/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CMSIS_OS2_H_
#define CMSIS_OS2_H_

#include "fff.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Version information.
typedef struct {
    uint32_t api;    ///< API version (major.minor.rev: mmnnnrrrr dec).
    uint32_t kernel; ///< Kernel version (major.minor.rev: mmnnnrrrr dec).
} osVersion_t;

/// Kernel state.
typedef enum {
    osKernelInactive = 0,          ///< Inactive.
    osKernelReady = 1,             ///< Ready.
    osKernelRunning = 2,           ///< Running.
    osKernelLocked = 3,            ///< Locked.
    osKernelSuspended = 4,         ///< Suspended.
    osKernelError = -1,            ///< Error.
    osKernelReserved = 0x7FFFFFFFU ///< Prevents enum down-size compiler optimization.
} osKernelState_t;

/// Thread state.
typedef enum {
    osThreadInactive = 0,         ///< Inactive.
    osThreadReady = 1,            ///< Ready.
    osThreadRunning = 2,          ///< Running.
    osThreadBlocked = 3,          ///< Blocked.
    osThreadTerminated = 4,       ///< Terminated.
    osThreadError = -1,           ///< Error.
    osThreadReserved = 0x7FFFFFFF ///< Prevents enum down-size compiler optimization.
} osThreadState_t;

/// Priority values.
typedef enum {
    osPriorityNone = 0,              ///< No priority (not initialized).
    osPriorityIdle = 1,              ///< Reserved for Idle thread.
    osPriorityLow = 8,               ///< Priority: low
    osPriorityLow1 = 8 + 1,          ///< Priority: low + 1
    osPriorityLow2 = 8 + 2,          ///< Priority: low + 2
    osPriorityLow3 = 8 + 3,          ///< Priority: low + 3
    osPriorityLow4 = 8 + 4,          ///< Priority: low + 4
    osPriorityLow5 = 8 + 5,          ///< Priority: low + 5
    osPriorityLow6 = 8 + 6,          ///< Priority: low + 6
    osPriorityLow7 = 8 + 7,          ///< Priority: low + 7
    osPriorityBelowNormal = 16,      ///< Priority: below normal
    osPriorityBelowNormal1 = 16 + 1, ///< Priority: below normal + 1
    osPriorityBelowNormal2 = 16 + 2, ///< Priority: below normal + 2
    osPriorityBelowNormal3 = 16 + 3, ///< Priority: below normal + 3
    osPriorityBelowNormal4 = 16 + 4, ///< Priority: below normal + 4
    osPriorityBelowNormal5 = 16 + 5, ///< Priority: below normal + 5
    osPriorityBelowNormal6 = 16 + 6, ///< Priority: below normal + 6
    osPriorityBelowNormal7 = 16 + 7, ///< Priority: below normal + 7
    osPriorityNormal = 24,           ///< Priority: normal
    osPriorityNormal1 = 24 + 1,      ///< Priority: normal + 1
    osPriorityNormal2 = 24 + 2,      ///< Priority: normal + 2
    osPriorityNormal3 = 24 + 3,      ///< Priority: normal + 3
    osPriorityNormal4 = 24 + 4,      ///< Priority: normal + 4
    osPriorityNormal5 = 24 + 5,      ///< Priority: normal + 5
    osPriorityNormal6 = 24 + 6,      ///< Priority: normal + 6
    osPriorityNormal7 = 24 + 7,      ///< Priority: normal + 7
    osPriorityAboveNormal = 32,      ///< Priority: above normal
    osPriorityAboveNormal1 = 32 + 1, ///< Priority: above normal + 1
    osPriorityAboveNormal2 = 32 + 2, ///< Priority: above normal + 2
    osPriorityAboveNormal3 = 32 + 3, ///< Priority: above normal + 3
    osPriorityAboveNormal4 = 32 + 4, ///< Priority: above normal + 4
    osPriorityAboveNormal5 = 32 + 5, ///< Priority: above normal + 5
    osPriorityAboveNormal6 = 32 + 6, ///< Priority: above normal + 6
    osPriorityAboveNormal7 = 32 + 7, ///< Priority: above normal + 7
    osPriorityHigh = 40,             ///< Priority: high
    osPriorityHigh1 = 40 + 1,        ///< Priority: high + 1
    osPriorityHigh2 = 40 + 2,        ///< Priority: high + 2
    osPriorityHigh3 = 40 + 3,        ///< Priority: high + 3
    osPriorityHigh4 = 40 + 4,        ///< Priority: high + 4
    osPriorityHigh5 = 40 + 5,        ///< Priority: high + 5
    osPriorityHigh6 = 40 + 6,        ///< Priority: high + 6
    osPriorityHigh7 = 40 + 7,        ///< Priority: high + 7
    osPriorityRealtime = 48,         ///< Priority: realtime
    osPriorityRealtime1 = 48 + 1,    ///< Priority: realtime + 1
    osPriorityRealtime2 = 48 + 2,    ///< Priority: realtime + 2
    osPriorityRealtime3 = 48 + 3,    ///< Priority: realtime + 3
    osPriorityRealtime4 = 48 + 4,    ///< Priority: realtime + 4
    osPriorityRealtime5 = 48 + 5,    ///< Priority: realtime + 5
    osPriorityRealtime6 = 48 + 6,    ///< Priority: realtime + 6
    osPriorityRealtime7 = 48 + 7,    ///< Priority: realtime + 7
    osPriorityISR = 56,              ///< Reserved for ISR deferred thread.
    osPriorityError = -1,            ///< System cannot determine priority or illegal priority.
    osPriorityReserved = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
} osPriority_t;

/// Entry point of a thread.
typedef void (*osThreadFunc_t)(void *argument);

/// Timer callback function.
typedef void (*osTimerFunc_t)(void *argument);

/// Timer type.
typedef enum {
    osTimerOnce = 0,    ///< One-shot timer.
    osTimerPeriodic = 1 ///< Repeating timer.
} osTimerType_t;

// Timeout value.
#define osWaitForever 0xFFFFFFFFU ///< Wait forever timeout value.

// Flags options (\ref osThreadFlagsWait and \ref osEventFlagsWait).
#define osFlagsWaitAny 0x00000000U ///< Wait for any flag (default).
#define osFlagsWaitAll 0x00000001U ///< Wait for all flags.
#define osFlagsNoClear 0x00000002U ///< Do not clear flags which have been specified to wait for.

// Flags errors (returned by osThreadFlagsXxxx and osEventFlagsXxxx).
#define osFlagsError          0x80000000U ///< Error indicator.
#define osFlagsErrorUnknown   0xFFFFFFFFU ///< osError (-1).
#define osFlagsErrorTimeout   0xFFFFFFFEU ///< osErrorTimeout (-2).
#define osFlagsErrorResource  0xFFFFFFFDU ///< osErrorResource (-3).
#define osFlagsErrorParameter 0xFFFFFFFCU ///< osErrorParameter (-4).
#define osFlagsErrorISR       0xFFFFFFFAU ///< osErrorISR (-6).

// Thread attributes (attr_bits in \ref osThreadAttr_t).
#define osThreadDetached 0x00000000U ///< Thread created in detached mode (default)
#define osThreadJoinable 0x00000001U ///< Thread created in joinable mode

// Mutex attributes (attr_bits in \ref osMutexAttr_t).
#define osMutexRecursive   0x00000001U ///< Recursive mutex.
#define osMutexPrioInherit 0x00000002U ///< Priority inherit protocol.
#define osMutexRobust      0x00000008U ///< Robust mutex.

/// Status code values returned by CMSIS-RTOS functions.
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

/// \details Thread ID identifies the thread.
typedef void *osThreadId_t;

/// \details Timer ID identifies the timer.
typedef void *osTimerId_t;

/// \details Event Flags ID identifies the event flags.
typedef void *osEventFlagsId_t;

/// \details Mutex ID identifies the mutex.
typedef void *osMutexId_t;

/// \details Semaphore ID identifies the semaphore.
typedef void *osSemaphoreId_t;

/// \details Memory Pool ID identifies the memory pool.
typedef void *osMemoryPoolId_t;

/// \details Message Queue ID identifies the message queue.
typedef void *osMessageQueueId_t;

#ifndef TZ_MODULEID_T
#define TZ_MODULEID_T
/// \details Data type that identifies secure software modules called by a process.
typedef uint32_t TZ_ModuleId_t;
#endif

/// Attributes structure for thread.
typedef struct {
    const char *name;        ///< name of the thread
    uint32_t attr_bits;      ///< attribute bits
    void *cb_mem;            ///< memory for control block
    uint32_t cb_size;        ///< size of provided memory for control block
    void *stack_mem;         ///< memory for stack
    uint32_t stack_size;     ///< size of stack
    osPriority_t priority;   ///< initial thread priority (default: osPriorityNormal)
    TZ_ModuleId_t tz_module; ///< TrustZone module identifier
    uint32_t reserved;       ///< reserved (must be 0)
} osThreadAttr_t;

/// Attributes structure for timer.
typedef struct {
    const char *name;   ///< name of the timer
    uint32_t attr_bits; ///< attribute bits
    void *cb_mem;       ///< memory for control block
    uint32_t cb_size;   ///< size of provided memory for control block
} osTimerAttr_t;

/// Attributes structure for event flags.
typedef struct {
    const char *name;   ///< name of the event flags
    uint32_t attr_bits; ///< attribute bits
    void *cb_mem;       ///< memory for control block
    uint32_t cb_size;   ///< size of provided memory for control block
} osEventFlagsAttr_t;

/// Attributes structure for mutex.
typedef struct {
    const char *name;   ///< name of the mutex
    uint32_t attr_bits; ///< attribute bits
    void *cb_mem;       ///< memory for control block
    uint32_t cb_size;   ///< size of provided memory for control block
} osMutexAttr_t;

/// Attributes structure for semaphore.
typedef struct {
    const char *name;   ///< name of the semaphore
    uint32_t attr_bits; ///< attribute bits
    void *cb_mem;       ///< memory for control block
    uint32_t cb_size;   ///< size of provided memory for control block
} osSemaphoreAttr_t;

/// Attributes structure for memory pool.
typedef struct {
    const char *name;   ///< name of the memory pool
    uint32_t attr_bits; ///< attribute bits
    void *cb_mem;       ///< memory for control block
    uint32_t cb_size;   ///< size of provided memory for control block
    void *mp_mem;       ///< memory for data storage
    uint32_t mp_size;   ///< size of provided memory for data storage
} osMemoryPoolAttr_t;

/// Attributes structure for message queue.
typedef struct {
    const char *name;   ///< name of the message queue
    uint32_t attr_bits; ///< attribute bits
    void *cb_mem;       ///< memory for control block
    uint32_t cb_size;   ///< size of provided memory for control block
    void *mq_mem;       ///< memory for data storage
    uint32_t mq_size;   ///< size of provided memory for data storage
} osMessageQueueAttr_t;

//  ==== Kernel Management Functions ====

DECLARE_FAKE_VALUE_FUNC(osStatus_t, osKernelInitialize);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osKernelGetInfo, osVersion_t *, char *, uint32_t);
DECLARE_FAKE_VALUE_FUNC(osKernelState_t, osKernelGetState);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osKernelStart);
DECLARE_FAKE_VALUE_FUNC(int32_t, osKernelLock);
DECLARE_FAKE_VALUE_FUNC(int32_t, osKernelUnlock);
DECLARE_FAKE_VALUE_FUNC(int32_t, osKernelRestoreLock, int32_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osKernelSuspend);
DECLARE_FAKE_VOID_FUNC(osKernelResume, int32_t);
DECLARE_FAKE_VALUE_FUNC(int32_t, osKernelGetTickCount);
DECLARE_FAKE_VALUE_FUNC(int32_t, osKernelGetTickFreq);
DECLARE_FAKE_VALUE_FUNC(int32_t, osKernelGetSysTimerCount);
DECLARE_FAKE_VALUE_FUNC(int32_t, osKernelGetSysTimerFreq);

//  ==== Thread Management Functions ====

DECLARE_FAKE_VALUE_FUNC(osThreadId_t, osThreadNew, osThreadFunc_t, void *, const osThreadAttr_t *);
DECLARE_FAKE_VALUE_FUNC(const char *, osThreadGetName, osThreadId_t);
DECLARE_FAKE_VALUE_FUNC(osThreadId_t, osThreadGetId);
DECLARE_FAKE_VALUE_FUNC(osThreadState_t, osThreadGetState, osThreadId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osThreadGetStackSize, osThreadId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osThreadGetStackSpace, osThreadId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osThreadSetPriority, osThreadId_t, osPriority_t);
DECLARE_FAKE_VALUE_FUNC(osPriority_t, osThreadGetPriority, osThreadId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osThreadYield);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osThreadSuspend, osThreadId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osThreadResume, osThreadId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osThreadDetach, osThreadId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osThreadJoin, osThreadId_t);
DECLARE_FAKE_VOID_FUNC(osThreadExit);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osThreadTerminate, osThreadId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osThreadGetCount);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osThreadEnumerate, osThreadId_t *, uint32_t);

//  ==== Thread Flags Functions ====

DECLARE_FAKE_VALUE_FUNC(uint32_t, osThreadFlagsSet, osThreadId_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osThreadFlagsClear, uint32_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osThreadFlagsGet);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osThreadFlagsWait, uint32_t, uint32_t, uint32_t);

//  ==== Generic Wait Functions ====

DECLARE_FAKE_VALUE_FUNC(osStatus_t, osDelay, uint32_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osDelayUntil, uint32_t);

//  ==== Timer Management Functions ====

DECLARE_FAKE_VALUE_FUNC(osTimerId_t, osTimerNew, osTimerFunc_t, osTimerType_t, void *, const osTimerAttr_t *);
DECLARE_FAKE_VALUE_FUNC(const char *, osTimerGetName, osTimerId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osTimerStart, osTimerId_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osTimerStop, osTimerId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osTimerIsRunning, osTimerId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osTimerDelete, osTimerId_t);

//  ==== Event Flags Management Functions ====

DECLARE_FAKE_VALUE_FUNC(osEventFlagsId_t, osEventFlagsNew, const osEventFlagsAttr_t);
DECLARE_FAKE_VALUE_FUNC(const char *, osEventFlagsGetName, osEventFlagsId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsSet, osEventFlagsId_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsClear, osEventFlagsId_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsGet, osEventFlagsId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsWait, osEventFlagsId_t, uint32_t, uint32_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osEventFlagsDelete, osEventFlagsId_t);

//  ==== Mutex Management Functions ====

DECLARE_FAKE_VALUE_FUNC(osMutexId_t, osMutexNew, const osMutexAttr_t *);
DECLARE_FAKE_VALUE_FUNC(const char *, osMutexGetName, osMutexId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMutexAcquire, osMutexId_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMutexRelease, osMutexId_t);
DECLARE_FAKE_VALUE_FUNC(osThreadId_t, osMutexGetOwner, osMutexId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMutexDelete, osMutexId_t);

//  ==== Semaphore Management Functions ====

DECLARE_FAKE_VALUE_FUNC(osSemaphoreId_t, osSemaphoreNew, uint32_t, uint32_t, const osSemaphoreAttr_t *);
DECLARE_FAKE_VALUE_FUNC(const char *, osSemaphoreGetName, osSemaphoreId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osSemaphoreAcquire, osSemaphoreId_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osSemaphoreRelease, osSemaphoreId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osSemaphoreGetCount, osSemaphoreId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osSemaphoreDelete, osSemaphoreId_t);

//  ==== Memory Pool Management Functions ====

DECLARE_FAKE_VALUE_FUNC(osMemoryPoolId_t, osMemoryPoolNew, uint32_t, uint32_t, const osMemoryPoolAttr_t *);
DECLARE_FAKE_VALUE_FUNC(const char *, osMemoryPoolGetName, osMemoryPoolId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMemoryPoolFree, osMemoryPoolId_t, void *);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osMemoryPoolGetCapacity, osMemoryPoolId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osMemoryPoolGetBlockSize, osMemoryPoolId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osMemoryPoolGetCount, osMemoryPoolId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osMemoryPoolGetSpace, osMemoryPoolId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMemoryPoolDelete, osMemoryPoolId_t);

//  ==== Message Queue Management Functions ====

DECLARE_FAKE_VALUE_FUNC(osMessageQueueId_t, osMessageQueueNew, uint32_t, uint32_t, const osMessageQueueAttr_t *);
DECLARE_FAKE_VALUE_FUNC(const char *, osMessageQueueGetName, osMessageQueueId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMessageQueuePut, osMessageQueueId_t, const void *, uint8_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMessageQueueGet, osMessageQueueId_t, void *, uint8_t *, uint32_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osMessageQueueGetCapacity, osMessageQueueId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osMessageQueueGetMsgSize, osMessageQueueId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osMessageQueueGetCount, osMessageQueueId_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, osMessageQueueGetSpace, osMessageQueueId_t);
DECLARE_FAKE_VALUE_FUNC(osStatus_t, osMessageQueueDelete, osMessageQueueId_t);

#ifdef __cplusplus
}
#endif

#endif // CMSIS_OS2_H_
