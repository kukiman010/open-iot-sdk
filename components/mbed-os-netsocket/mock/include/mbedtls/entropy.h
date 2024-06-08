/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MBEDTLS_ENTROPY_H
#define MBEDTLS_ENTROPY_H

#include "mbedtls/mbedtls_threading_cmsis_rtos.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MBEDTLS_ENTROPY_MAX_SOURCES)
#define MBEDTLS_ENTROPY_MAX_SOURCES 20 /**< Maximum number of sources supported */
#endif

/**
 * \brief           Entropy poll callback pointer
 *
 * \param data      Callback-specific data pointer
 * \param output    Data to fill
 * \param len       Maximum size to provide
 * \param olen      The actual amount of bytes put into the buffer (Can be 0)
 *
 * \return          0 if no critical failures occurred,
 *                  MBEDTLS_ERR_ENTROPY_SOURCE_FAILED otherwise
 */
typedef int (*mbedtls_entropy_f_source_ptr)(void *data, unsigned char *output, size_t len, size_t *olen);

/**
 * \brief           Entropy source state
 */
typedef struct mbedtls_entropy_source_state {
    mbedtls_entropy_f_source_ptr MBEDTLS_PRIVATE(f_source); /**< The entropy source callback */
    void *MBEDTLS_PRIVATE(p_source);                        /**< The callback data pointer */
    size_t MBEDTLS_PRIVATE(size);                           /**< Amount received in bytes */
    size_t MBEDTLS_PRIVATE(threshold);                      /**< Minimum bytes required before release */
    int MBEDTLS_PRIVATE(strong);                            /**< Is the source strong? */
} mbedtls_entropy_source_state;

/**
 * \brief           Entropy context structure
 */
typedef struct mbedtls_entropy_context {
    int MBEDTLS_PRIVATE(accumulator_started); /* 0 after init.
                                               * 1 after the first update.
                                               * -1 after free. */
    int MBEDTLS_PRIVATE(source_count);        /* Number of entries used in source. */
    mbedtls_entropy_source_state MBEDTLS_PRIVATE(source)[MBEDTLS_ENTROPY_MAX_SOURCES];
#if defined(MBEDTLS_THREADING_C)
    mbedtls_threading_mutex_t MBEDTLS_PRIVATE(mutex); /*!< mutex                  */
#endif
#if defined(MBEDTLS_ENTROPY_NV_SEED)
    int MBEDTLS_PRIVATE(initial_entropy_run);
#endif
} mbedtls_entropy_context;

DECLARE_FAKE_VALUE_FUNC(int, mbedtls_entropy_func, void *, unsigned char *, size_t);

DECLARE_FAKE_VOID_FUNC(mbedtls_entropy_init, mbedtls_entropy_context *);
DECLARE_FAKE_VOID_FUNC(mbedtls_entropy_free, mbedtls_entropy_context *);

#ifdef __cplusplus
}
#endif

#endif /* entropy.h */
