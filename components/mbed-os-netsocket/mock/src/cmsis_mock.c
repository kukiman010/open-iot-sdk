/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis_os2.h"

DEFINE_FAKE_VALUE_FUNC(osMutexId_t, osMutexNew, const osMutexAttr_t *);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osMutexAcquire, osMutexId_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC(osStatus_t, osMutexRelease, osMutexId_t);

DEFINE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsSet, osEventFlagsId_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC(uint32_t, osEventFlagsWait, osEventFlagsId_t, uint32_t, uint32_t, uint32_t);
