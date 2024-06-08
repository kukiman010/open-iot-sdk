/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis_os2.h"
#include "fff.h"

DEFINE_FAKE_VALUE_FUNC(osStatus_t, osKernelInitialize);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osKernelGetInfo, osVersion_t *, char *, uint32_t);
DEFINE_FAKE_VALUE_FUNC(osKernelState_t, osKernelGetState);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osKernelStart);
DEFINE_FAKE_VALUE_FUNC(int32_t, osKernelLock);
DEFINE_FAKE_VALUE_FUNC(int32_t, osKernelUnlock);
DEFINE_FAKE_VALUE_FUNC(int32_t, osKernelRestoreLock, int32_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osKernelSuspend);
DEFINE_FAKE_VOID_FUNC(osKernelResume, int32_t);
DEFINE_FAKE_VALUE_FUNC(int32_t, osKernelGetTickCount);
DEFINE_FAKE_VALUE_FUNC(int32_t, osKernelGetTickFreq);
DEFINE_FAKE_VALUE_FUNC(int32_t, osKernelGetSysTimerCount);
DEFINE_FAKE_VALUE_FUNC(int32_t, osKernelGetSysTimerFreq);

DEFINE_FAKE_VALUE_FUNC(osThreadId_t, osThreadNew, osThreadFunc_t, void *, const osThreadAttr_t *);
DEFINE_FAKE_VALUE_FUNC(const char *, osThreadGetName, osThreadId_t);
DEFINE_FAKE_VALUE_FUNC(osThreadId_t, osThreadGetId);
DEFINE_FAKE_VALUE_FUNC(osThreadState_t, osThreadGetState, osThreadId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osThreadGetStackSize, osThreadId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osThreadGetStackSpace, osThreadId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osThreadSetPriority, osThreadId_t, osPriority_t);
DEFINE_FAKE_VALUE_FUNC(osPriority_t, osThreadGetPriority, osThreadId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osThreadYield);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osThreadSuspend, osThreadId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osThreadResume, osThreadId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osThreadDetach, osThreadId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osThreadJoin, osThreadId_t);
DEFINE_FAKE_VOID_FUNC(osThreadExit);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osThreadTerminate, osThreadId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osThreadGetCount);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osThreadEnumerate, osThreadId_t *, uint32_t);

DEFINE_FAKE_VALUE_FUNC(uint32_t, osThreadFlagsSet, osThreadId_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osThreadFlagsClear, uint32_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osThreadFlagsGet);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osThreadFlagsWait, uint32_t, uint32_t, uint32_t);

DEFINE_FAKE_VALUE_FUNC(osStatus_t, osDelay, uint32_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osDelayUntil, uint32_t);

DEFINE_FAKE_VALUE_FUNC(osTimerId_t, osTimerNew, osTimerFunc_t, osTimerType_t, void *, const osTimerAttr_t *);
DEFINE_FAKE_VALUE_FUNC(const char *, osTimerGetName, osTimerId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osTimerStart, osTimerId_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osTimerStop, osTimerId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osTimerIsRunning, osTimerId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osTimerDelete, osTimerId_t);

DEFINE_FAKE_VALUE_FUNC(osEventFlagsId_t, osEventFlagsNew, const osEventFlagsAttr_t);
DEFINE_FAKE_VALUE_FUNC(const char *, osEventFlagsGetName, osEventFlagsId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsSet, osEventFlagsId_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsClear, osEventFlagsId_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsGet, osEventFlagsId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsWait, osEventFlagsId_t, uint32_t, uint32_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osEventFlagsDelete, osEventFlagsId_t);

DEFINE_FAKE_VALUE_FUNC(osMutexId_t, osMutexNew, const osMutexAttr_t *);
DEFINE_FAKE_VALUE_FUNC(const char *, osMutexGetName, osMutexId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osMutexAcquire, osMutexId_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osMutexRelease, osMutexId_t);
DEFINE_FAKE_VALUE_FUNC(osThreadId_t, osMutexGetOwner, osMutexId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osMutexDelete, osMutexId_t);

DEFINE_FAKE_VALUE_FUNC(osSemaphoreId_t, osSemaphoreNew, uint32_t, uint32_t, const osSemaphoreAttr_t *);
DEFINE_FAKE_VALUE_FUNC(const char *, osSemaphoreGetName, osSemaphoreId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osSemaphoreAcquire, osSemaphoreId_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osSemaphoreRelease, osSemaphoreId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osSemaphoreGetCount, osSemaphoreId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osSemaphoreDelete, osSemaphoreId_t);

DEFINE_FAKE_VALUE_FUNC(osMemoryPoolId_t, osMemoryPoolNew, uint32_t, uint32_t, const osMemoryPoolAttr_t *);
DEFINE_FAKE_VALUE_FUNC(const char *, osMemoryPoolGetName, osMemoryPoolId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osMemoryPoolFree, osMemoryPoolId_t, void *);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osMemoryPoolGetCapacity, osMemoryPoolId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osMemoryPoolGetBlockSize, osMemoryPoolId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osMemoryPoolGetCount, osMemoryPoolId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osMemoryPoolGetSpace, osMemoryPoolId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osMemoryPoolDelete, osMemoryPoolId_t);

DEFINE_FAKE_VALUE_FUNC(osMessageQueueId_t, osMessageQueueNew, uint32_t, uint32_t, const osMessageQueueAttr_t *);
DEFINE_FAKE_VALUE_FUNC(const char *, osMessageQueueGetName, osMessageQueueId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osMessageQueuePut, osMessageQueueId_t, const void *, uint8_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osMessageQueueGet, osMessageQueueId_t, void *, uint8_t *, uint32_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osMessageQueueGetCapacity, osMessageQueueId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osMessageQueueGetMsgSize, osMessageQueueId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osMessageQueueGetCount, osMessageQueueId_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osMessageQueueGetSpace, osMessageQueueId_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osMessageQueueDelete, osMessageQueueId_t);
