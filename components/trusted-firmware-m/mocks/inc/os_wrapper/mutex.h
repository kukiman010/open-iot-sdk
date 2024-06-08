/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_MUTEX_H__
#define __OS_WRAPPER_MUTEX_H__

#include "fff.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OS_WRAPPER_SUCCESS            (0x0)
#define OS_WRAPPER_ERROR              (0xFFFFFFFFU)
#define OS_WRAPPER_WAIT_FOREVER       (0xFFFFFFFFU)
#define OS_WRAPPER_DEFAULT_STACK_SIZE (-1)

DECLARE_FAKE_VALUE_FUNC(void *, os_wrapper_mutex_create);
DECLARE_FAKE_VALUE_FUNC(uint32_t, os_wrapper_mutex_acquire, void *, uint32_t);
DECLARE_FAKE_VALUE_FUNC(uint32_t, os_wrapper_mutex_release, void *);
DECLARE_FAKE_VALUE_FUNC(uint32_t, os_wrapper_mutex_delete, void *);

#ifdef __cplusplus
}
#endif

#endif // __OS_WRAPPER_MUTEX_H__
