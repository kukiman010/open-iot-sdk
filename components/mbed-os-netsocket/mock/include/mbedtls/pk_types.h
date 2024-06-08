/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PK_TYPES_H
#define PK_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MBEDTLS_ALLOW_PRIVATE_ACCESS
#define MBEDTLS_PRIVATE(member) private_##member
#else
#define MBEDTLS_PRIVATE(member) member
#endif

/**
 * \brief          Public key types
 */
typedef enum {
    MBEDTLS_PK_NONE = 0,
    MBEDTLS_PK_RSA,
    MBEDTLS_PK_ECKEY,
    MBEDTLS_PK_ECKEY_DH,
    MBEDTLS_PK_ECDSA,
    MBEDTLS_PK_RSA_ALT,
    MBEDTLS_PK_RSASSA_PSS,
    MBEDTLS_PK_OPAQUE,
} mbedtls_pk_type_t;

/**
 * \brief           Public key information and operations
 *
 * \note        The library does not support custom pk info structures,
 *              only built-in structures returned by
 *              mbedtls_cipher_info_from_type().
 */
typedef struct mbedtls_pk_info_t mbedtls_pk_info_t;

typedef struct mbedtls_pk_context {
    const mbedtls_pk_info_t *MBEDTLS_PRIVATE(pk_info); /**< Public key information         */
    void *MBEDTLS_PRIVATE(pk_ctx);                     /**< Underlying public key context  */
} mbedtls_pk_context;

#ifdef __cplusplus
}
#endif

#endif // PK_TYPES_H
