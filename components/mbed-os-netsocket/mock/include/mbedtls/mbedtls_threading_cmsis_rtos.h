/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MBEDTLS_THREADING_CMSIS_RTOS_H
#define MBEDTLS_THREADING_CMSIS_RTOS_H

#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

/** This is an internal Mbed TLS structure used by the threading module.
 *  It's required in alternative threading support.
 */
typedef struct {
    osMutexId_t mutex;
} mbedtls_threading_mutex_t;

#ifdef __cplusplus
}
#endif

#endif // ! MBEDTLS_THREADING_CMSIS_RTOS_H
