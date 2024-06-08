/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MBEDTLS_THREADING_H
#define MBEDTLS_THREADING_H

#include "fff.h"

#include "mbedtls_threading_cmsis_rtos.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mutex_init_f)(mbedtls_threading_mutex_t *);
typedef void (*mutex_free_f)(mbedtls_threading_mutex_t *);
typedef int (*mutex_lock_f)(mbedtls_threading_mutex_t *);
typedef int (*mutex_unlock_f)(mbedtls_threading_mutex_t *);

/** Bad input parameters to function. */
#define MBEDTLS_ERR_THREADING_BAD_INPUT_DATA -0x001C
/** Locking / unlocking / free failed with error code. */
#define MBEDTLS_ERR_THREADING_MUTEX_ERROR -0x001E

DECLARE_FAKE_VOID_FUNC(mbedtls_threading_set_alt, mutex_init_f, mutex_free_f, mutex_lock_f, mutex_unlock_f);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MBEDTLS_THREADING_H
