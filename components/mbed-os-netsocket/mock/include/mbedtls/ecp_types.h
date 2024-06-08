/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ECP_TYPES_H
#define ECP_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The maximum size of the groups, that is, of \c N and \c P.
 */
#if !defined(MBEDTLS_ECP_C)
/* Dummy definition to help code that has optional ECP support and
 * defines an MBEDTLS_ECP_MAX_BYTES-sized array unconditionally. */
#define MBEDTLS_ECP_MAX_BITS 1
/* Note: the curves must be listed in DECREASING size! */
#elif defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 521
#elif defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 512
#elif defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 448
#elif defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 384
#elif defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 384
#elif defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 256
#elif defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 256
#elif defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 256
#elif defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 255
#elif defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 225 // n is slightly above 2^224
#elif defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 224
#elif defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 192
#elif defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
#define MBEDTLS_ECP_MAX_BITS 192
#else
#error "Missing definition of MBEDTLS_ECP_MAX_BITS"
#endif

#define MBEDTLS_ECP_MAX_BYTES  ((MBEDTLS_ECP_MAX_BITS + 7) / 8)
#define MBEDTLS_ECP_MAX_PT_LEN (2 * MBEDTLS_ECP_MAX_BYTES + 1)

/**
 * Domain-parameter identifiers: curve, subgroup, and generator.
 *
 * \note Only curves over prime fields are supported.
 *
 * \warning This library does not support validation of arbitrary domain
 * parameters. Therefore, only standardized domain parameters from trusted
 * sources should be used. See mbedtls_ecp_group_load().
 */
/* Note: when adding a new curve:
 * - Add it at the end of this enum, otherwise you'll break the ABI by
 *   changing the numerical value for existing curves.
 * - Increment MBEDTLS_ECP_DP_MAX below if needed.
 * - Update the calculation of MBEDTLS_ECP_MAX_BITS below.
 * - Add the corresponding MBEDTLS_ECP_DP_xxx_ENABLED macro definition to
 *   mbedtls_config.h.
 * - List the curve as a dependency of MBEDTLS_ECP_C and
 *   MBEDTLS_ECDSA_C if supported in check_config.h.
 * - Add the curve to the appropriate curve type macro
 *   MBEDTLS_ECP_yyy_ENABLED above.
 * - Add the necessary definitions to ecp_curves.c.
 * - Add the curve to the ecp_supported_curves array in ecp.c.
 * - Add the curve to applicable profiles in x509_crt.c.
 * - Add the curve to applicable presets in ssl_tls.c.
 */
typedef enum {
    MBEDTLS_ECP_DP_NONE = 0,   /*!< Curve not defined. */
    MBEDTLS_ECP_DP_SECP192R1,  /*!< Domain parameters for the 192-bit curve defined by FIPS 186-4 and SEC1. */
    MBEDTLS_ECP_DP_SECP224R1,  /*!< Domain parameters for the 224-bit curve defined by FIPS 186-4 and SEC1. */
    MBEDTLS_ECP_DP_SECP256R1,  /*!< Domain parameters for the 256-bit curve defined by FIPS 186-4 and SEC1. */
    MBEDTLS_ECP_DP_SECP384R1,  /*!< Domain parameters for the 384-bit curve defined by FIPS 186-4 and SEC1. */
    MBEDTLS_ECP_DP_SECP521R1,  /*!< Domain parameters for the 521-bit curve defined by FIPS 186-4 and SEC1. */
    MBEDTLS_ECP_DP_BP256R1,    /*!< Domain parameters for 256-bit Brainpool curve. */
    MBEDTLS_ECP_DP_BP384R1,    /*!< Domain parameters for 384-bit Brainpool curve. */
    MBEDTLS_ECP_DP_BP512R1,    /*!< Domain parameters for 512-bit Brainpool curve. */
    MBEDTLS_ECP_DP_CURVE25519, /*!< Domain parameters for Curve25519. */
    MBEDTLS_ECP_DP_SECP192K1,  /*!< Domain parameters for 192-bit "Koblitz" curve. */
    MBEDTLS_ECP_DP_SECP224K1,  /*!< Domain parameters for 224-bit "Koblitz" curve. */
    MBEDTLS_ECP_DP_SECP256K1,  /*!< Domain parameters for 256-bit "Koblitz" curve. */
    MBEDTLS_ECP_DP_CURVE448,   /*!< Domain parameters for Curve448. */
} mbedtls_ecp_group_id;

#ifdef __cplusplus
}
#endif

#endif // ECP_TYPES_H
