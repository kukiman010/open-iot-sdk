/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MBEDTLS_CTR_DRBG_H
#define MBEDTLS_CTR_DRBG_H

#include "fff.h"
#include "mbedtls/mbedtls_threading_cmsis_rtos.h"
#include "mbedtls/pk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief The AES context-type definition.
 */
typedef struct mbedtls_aes_context {
    int MBEDTLS_PRIVATE(nr);           /*!< The number of rounds. */
    size_t MBEDTLS_PRIVATE(rk_offset); /*!< The offset in array elements to AES
                           round keys in the buffer. */
    uint32_t MBEDTLS_PRIVATE(buf)[68]; /*!< Unaligned data buffer. This buffer can
                           hold 32 extra Bytes, which can be used for
                           one of the following purposes:
                           <ul><li>Alignment if VIA padlock is
                                   used.</li>
                           <li>Simplifying key expansion in the 256-bit
                               case by generating an extra round key.
                               </li></ul> */
} mbedtls_aes_context;

typedef struct mbedtls_ctr_drbg_context {
    unsigned char MBEDTLS_PRIVATE(counter)[16]; /*!< The counter (V). */
    int MBEDTLS_PRIVATE(reseed_counter);        /*!< The reseed counter.
                                                 * This is the number of requests that have
                                                 * been made since the last (re)seeding,
                                                 * minus one.
                                                 * Before the initial seeding, this field
                                                 * contains the amount of entropy in bytes
                                                 * to use as a nonce for the initial seeding,
                                                 * or -1 if no nonce length has been explicitly
                                                 * set (see mbedtls_ctr_drbg_set_nonce_len()).
                                                 */
    int MBEDTLS_PRIVATE(prediction_resistance); /*!< This determines whether prediction
                                    resistance is enabled, that is
                                    whether to systematically reseed before
                                    each random generation. */
    size_t MBEDTLS_PRIVATE(entropy_len);        /*!< The amount of entropy grabbed on each
                                    seed or reseed operation, in bytes. */
    int MBEDTLS_PRIVATE(reseed_interval);       /*!< The reseed interval.
                                                 * This is the maximum number of requests
                                                 * that can be made between reseedings. */

    mbedtls_aes_context MBEDTLS_PRIVATE(aes_ctx); /*!< The AES context. */

    /*
     * Callbacks (Entropy)
     */
    int (*MBEDTLS_PRIVATE(f_entropy))(void *, unsigned char *, size_t);
    /*!< The entropy callback function. */

    void *MBEDTLS_PRIVATE(p_entropy); /*!< The context for the entropy function. */

#if defined(MBEDTLS_THREADING_C)
    /* Invariant: the mutex is initialized if and only if f_entropy != NULL.
     * This means that the mutex is initialized during the initial seeding
     * in mbedtls_ctr_drbg_seed() and freed in mbedtls_ctr_drbg_free().
     *
     * Note that this invariant may change without notice. Do not rely on it
     * and do not access the mutex directly in application code.
     */
    mbedtls_threading_mutex_t MBEDTLS_PRIVATE(mutex);
#endif
} mbedtls_ctr_drbg_context;

typedef int (*func_type)(void *, unsigned char *, size_t);

DECLARE_FAKE_VALUE_FUNC(
    int, mbedtls_ctr_drbg_seed, mbedtls_ctr_drbg_context *, func_type, void *, const unsigned char *, size_t);
DECLARE_FAKE_VALUE_FUNC(int, mbedtls_ctr_drbg_random, void *, unsigned char *, size_t);

DECLARE_FAKE_VOID_FUNC(mbedtls_ctr_drbg_init, mbedtls_ctr_drbg_context *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ctr_drbg_free, mbedtls_ctr_drbg_context *);

#ifdef __cplusplus
}
#endif

#endif /* ctr_drbg.h */
