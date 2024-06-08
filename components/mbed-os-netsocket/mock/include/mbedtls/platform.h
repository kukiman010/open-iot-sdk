/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MBEDTLS_PLATFORM_H
#define MBEDTLS_PLATFORM_H

#include "fff.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   The platform context structure.
 *
 * \note    This structure may be used to assist platform-specific
 *          setup or teardown operations.
 */
typedef struct mbedtls_platform_context {
    int dummy; /**< A placeholder member, as empty structs are not portable. */
} mbedtls_platform_context;

DECLARE_FAKE_VALUE_FUNC(int, mbedtls_platform_setup, mbedtls_platform_context *);

DECLARE_FAKE_VOID_FUNC(mbedtls_platform_teardown, mbedtls_platform_context *);

#ifdef __cplusplus
}
#endif

#endif /* platform.h */
