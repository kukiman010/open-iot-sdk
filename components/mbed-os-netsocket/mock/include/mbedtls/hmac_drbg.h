/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MBEDTLS_HMAC_DRBG_H
#define MBEDTLS_HMAC_DRBG_H

#include "mbedtls/mbedtls_threading_cmsis_rtos.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_SHA512_C)
#define MBEDTLS_MD_MAX_SIZE 64 /* longest known is SHA512 */
#else
#define MBEDTLS_MD_MAX_SIZE 32 /* longest known is SHA256 or less */
#endif

/**
 * \brief     Supported message digests.
 *
 * \warning   MD5 and SHA-1 are considered weak message digests and
 *            their use constitutes a security risk. We recommend considering
 *            stronger message digests instead.
 *
 */
typedef enum {
    MBEDTLS_MD_NONE = 0,  /**< None. */
    MBEDTLS_MD_MD5,       /**< The MD5 message digest. */
    MBEDTLS_MD_SHA1,      /**< The SHA-1 message digest. */
    MBEDTLS_MD_SHA224,    /**< The SHA-224 message digest. */
    MBEDTLS_MD_SHA256,    /**< The SHA-256 message digest. */
    MBEDTLS_MD_SHA384,    /**< The SHA-384 message digest. */
    MBEDTLS_MD_SHA512,    /**< The SHA-512 message digest. */
    MBEDTLS_MD_RIPEMD160, /**< The RIPEMD-160 message digest. */
} mbedtls_md_type_t;

struct mbedtls_md_info_t {
    /** Name of the message digest */
    const char *name;

    /** Digest identifier */
    mbedtls_md_type_t type;

    /** Output length of the digest function in bytes */
    unsigned char size;

    /** Block length of the digest function in bytes */
    unsigned char block_size;
};

/**
 * Opaque struct.
 *
 * Constructed using either #mbedtls_md_info_from_string or
 * #mbedtls_md_info_from_type.
 *
 * Fields can be accessed with #mbedtls_md_get_size,
 * #mbedtls_md_get_type and #mbedtls_md_get_name.
 */
/* Defined internally in library/md_wrap.h. */
typedef struct mbedtls_md_info_t mbedtls_md_info_t;

/**
 * The generic message-digest context.
 */
typedef struct mbedtls_md_context_t {
    /** Information about the associated message digest. */
    const mbedtls_md_info_t *MBEDTLS_PRIVATE(md_info);

    /** The digest-specific context. */
    void *MBEDTLS_PRIVATE(md_ctx);

    /** The HMAC part of the context. */
    void *MBEDTLS_PRIVATE(hmac_ctx);
} mbedtls_md_context_t;

/**
 * HMAC_DRBG context.
 */
typedef struct mbedtls_hmac_drbg_context {
    /* Working state: the key K is not stored explicitly,
     * but is implied by the HMAC context */
    mbedtls_md_context_t MBEDTLS_PRIVATE(md_ctx);          /*!< HMAC context (inc. K)  */
    unsigned char MBEDTLS_PRIVATE(V)[MBEDTLS_MD_MAX_SIZE]; /*!< V in the spec          */
    int MBEDTLS_PRIVATE(reseed_counter);                   /*!< reseed counter         */

    /* Administrative state */
    size_t MBEDTLS_PRIVATE(entropy_len);        /*!< entropy bytes grabbed on each (re)seed */
    int MBEDTLS_PRIVATE(prediction_resistance); /*!< enable prediction resistance (Automatic
                                    reseed before every random generation) */
    int MBEDTLS_PRIVATE(reseed_interval);       /*!< reseed interval   */

    /* Callbacks */
    int (*MBEDTLS_PRIVATE(f_entropy))(void *, unsigned char *, size_t); /*!< entropy function */
    void *MBEDTLS_PRIVATE(p_entropy);                                   /*!< context for the entropy function        */

#if defined(MBEDTLS_THREADING_C)
    /* Invariant: the mutex is initialized if and only if
     * md_ctx->md_info != NULL. This means that the mutex is initialized
     * during the initial seeding in mbedtls_hmac_drbg_seed() or
     * mbedtls_hmac_drbg_seed_buf() and freed in mbedtls_ctr_drbg_free().
     *
     * Note that this invariant may change without notice. Do not rely on it
     * and do not access the mutex directly in application code.
     */
    mbedtls_threading_mutex_t MBEDTLS_PRIVATE(mutex);
#endif
} mbedtls_hmac_drbg_context;

#ifdef __cplusplus
}
#endif

#endif /* hmac_drbg.h */
