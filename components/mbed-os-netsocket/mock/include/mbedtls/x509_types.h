/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbedtls/hmac_drbg.h"
#include "mbedtls/pk_types.h"

#ifndef X509_TYPES_H
#define X509_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Security profile for certificate verification.
 *
 * All lists are bitfields, built by ORing flags from MBEDTLS_X509_ID_FLAG().
 *
 * The fields of this structure are part of the public API and can be
 * manipulated directly by applications. Future versions of the library may
 * add extra fields or reorder existing fields.
 *
 * You can create custom profiles by starting from a copy of
 * an existing profile, such as mbedtls_x509_crt_profile_default or
 * mbedtls_x509_ctr_profile_none and then tune it to your needs.
 *
 * For example to allow SHA-224 in addition to the default:
 *
 *  mbedtls_x509_crt_profile my_profile = mbedtls_x509_crt_profile_default;
 *  my_profile.allowed_mds |= MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA224 );
 *
 * Or to allow only RSA-3072+ with SHA-256:
 *
 *  mbedtls_x509_crt_profile my_profile = mbedtls_x509_crt_profile_none;
 *  my_profile.allowed_mds = MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA256 );
 *  my_profile.allowed_pks = MBEDTLS_X509_ID_FLAG( MBEDTLS_PK_RSA );
 *  my_profile.rsa_min_bitlen = 3072;
 */
typedef struct mbedtls_x509_crt_profile {
    uint32_t allowed_mds;    /**< MDs for signatures         */
    uint32_t allowed_pks;    /**< PK algs for public keys;
                              *   this applies to all certificates
                              *   in the provided chain.     */
    uint32_t allowed_curves; /**< Elliptic curves for ECDSA  */
    uint32_t rsa_min_bitlen; /**< Minimum size for RSA keys  */
} mbedtls_x509_crt_profile;

/**
 * Type-length-value structure that allows for ASN1 using DER.
 */
typedef struct mbedtls_asn1_buf {
    int tag;          /**< ASN1 type, e.g. MBEDTLS_ASN1_UTF8_STRING. */
    size_t len;       /**< ASN1 length, in octets. */
    unsigned char *p; /**< ASN1 data, e.g. in ASCII. */
} mbedtls_asn1_buf;

/**
 * Container for a sequence of ASN.1 items
 */
typedef struct mbedtls_asn1_sequence {
    mbedtls_asn1_buf buf; /**< Buffer containing the given ASN.1 item. */

    /** The next entry in the sequence.
     *
     * The details of memory management for sequences are not documented and
     * may change in future versions. Set this field to \p NULL when
     * initializing a structure, and do not modify it except via Mbed TLS
     * library functions.
     */
    struct mbedtls_asn1_sequence *next;
} mbedtls_asn1_sequence;

/**
 * Container for a sequence or list of 'named' ASN.1 data items
 */
typedef struct mbedtls_asn1_named_data {
    mbedtls_asn1_buf oid; /**< The object identifier. */
    mbedtls_asn1_buf val; /**< The named value. */

    /** The next entry in the sequence.
     *
     * The details of memory management for named data sequences are not
     * documented and may change in future versions. Set this field to \p NULL
     * when initializing a structure, and do not modify it except via Mbed TLS
     * library functions.
     */
    struct mbedtls_asn1_named_data *next;

    /** Merge next item into the current one?
     *
     * This field exists for the sake of Mbed TLS's X.509 certificate parsing
     * code and may change in future versions of the library.
     */
    unsigned char MBEDTLS_PRIVATE(next_merged);
} mbedtls_asn1_named_data;

/**
 * Type-length-value structure that allows for ASN1 using DER.
 */
typedef mbedtls_asn1_buf mbedtls_x509_buf;

/**
 * Container for ASN1 named information objects.
 * It allows for Relative Distinguished Names (e.g. cn=localhost,ou=code,etc.).
 */
typedef mbedtls_asn1_named_data mbedtls_x509_name;

/**
 * Container for a sequence of ASN.1 items
 */
typedef mbedtls_asn1_sequence mbedtls_x509_sequence;

/** Container for date and time (precision in seconds). */
typedef struct mbedtls_x509_time {
    int year, mon, day; /**< Date. */
    int hour, min, sec; /**< Time. */
} mbedtls_x509_time;

typedef struct mbedtls_x509_crt {
    int MBEDTLS_PRIVATE(own_buffer); /**< Indicates if \c raw is owned
                                      *   by the structure or not.        */
    mbedtls_x509_buf raw;            /**< The raw certificate data (DER). */
    mbedtls_x509_buf tbs;            /**< The raw certificate body (DER). The part that is To Be Signed. */

    int version;              /**< The X.509 version. (1=v1, 2=v2, 3=v3) */
    mbedtls_x509_buf serial;  /**< Unique id for certificate issued by a specific CA. */
    mbedtls_x509_buf sig_oid; /**< Signature algorithm, e.g. sha1RSA */

    mbedtls_x509_buf issuer_raw;  /**< The raw issuer data (DER). Used for quick comparison. */
    mbedtls_x509_buf subject_raw; /**< The raw subject data (DER). Used for quick comparison. */

    mbedtls_x509_name issuer;  /**< The parsed issuer data (named information object). */
    mbedtls_x509_name subject; /**< The parsed subject data (named information object). */

    mbedtls_x509_time valid_from; /**< Start time of certificate validity. */
    mbedtls_x509_time valid_to;   /**< End time of certificate validity. */

    mbedtls_x509_buf pk_raw;
    mbedtls_pk_context pk; /**< Container for the public key context. */

    mbedtls_x509_buf issuer_id;              /**< Optional X.509 v2/v3 issuer unique identifier. */
    mbedtls_x509_buf subject_id;             /**< Optional X.509 v2/v3 subject unique identifier. */
    mbedtls_x509_buf v3_ext;                 /**< Optional X.509 v3 extensions.  */
    mbedtls_x509_sequence subject_alt_names; /**< Optional list of raw entries of Subject Alternative Names extension
                                                (currently only dNSName and OtherName are listed). */

    mbedtls_x509_sequence certificate_policies; /**< Optional list of certificate policies (Only anyPolicy is printed
                                                   and enforced, however the rest of the policies are still listed). */

    int MBEDTLS_PRIVATE(ext_types);   /**< Bit string containing detected and parsed extensions */
    int MBEDTLS_PRIVATE(ca_istrue);   /**< Optional Basic Constraint extension value: 1 if this certificate belongs to a
                                         CA, 0 otherwise. */
    int MBEDTLS_PRIVATE(max_pathlen); /**< Optional Basic Constraint extension value: The maximum path length to the
                                         root certificate. Path length is 1 higher than RFC 5280 'meaning', so 1+ */

    unsigned int MBEDTLS_PRIVATE(key_usage); /**< Optional key usage extension value: See the values in x509.h */

    mbedtls_x509_sequence ext_key_usage; /**< Optional list of extended key usage OIDs. */

    unsigned char MBEDTLS_PRIVATE(
        ns_cert_type); /**< Optional Netscape certificate type extension value: See the values in x509.h */

    mbedtls_x509_buf MBEDTLS_PRIVATE(sig); /**< Signature: hash of the tbs part signed with the private key. */
    mbedtls_md_type_t MBEDTLS_PRIVATE(
        sig_md); /**< Internal representation of the MD algorithm of the signature algorithm, e.g. MBEDTLS_MD_SHA256 */
    mbedtls_pk_type_t MBEDTLS_PRIVATE(sig_pk); /**< Internal representation of the Public Key algorithm of the signature
                                                  algorithm, e.g. MBEDTLS_PK_RSA */
    void *MBEDTLS_PRIVATE(
        sig_opts); /**< Signature options to be passed to mbedtls_pk_verify_ext(), e.g. for RSASSA-PSS */

    /** Next certificate in the linked list that constitutes the CA chain.
     * \p NULL indicates the end of the list.
     * Do not modify this field directly. */
    struct mbedtls_x509_crt *next;
} mbedtls_x509_crt;

typedef struct mbedtls_x509_crl_entry {
    /** Direct access to the whole entry inside the containing buffer. */
    mbedtls_x509_buf raw;
    /** The serial number of the revoked certificate. */
    mbedtls_x509_buf serial;
    /** The revocation date of this entry. */
    mbedtls_x509_time revocation_date;
    /** Direct access to the list of CRL entry extensions
     * (an ASN.1 constructed sequence).
     *
     * If there are no extensions, `entry_ext.len == 0` and
     * `entry_ext.p == NULL`. */
    mbedtls_x509_buf entry_ext;

    /** Next element in the linked list of entries.
     * \p NULL indicates the end of the list.
     * Do not modify this field directly. */
    struct mbedtls_x509_crl_entry *next;
} mbedtls_x509_crl_entry;

/**
 * Certificate revocation list structure.
 * Every CRL may have multiple entries.
 */
typedef struct mbedtls_x509_crl {
    mbedtls_x509_buf raw; /**< The raw certificate data (DER). */
    mbedtls_x509_buf tbs; /**< The raw certificate body (DER). The part that is To Be Signed. */

    int version;              /**< CRL version (1=v1, 2=v2) */
    mbedtls_x509_buf sig_oid; /**< CRL signature type identifier */

    mbedtls_x509_buf issuer_raw; /**< The raw issuer data (DER). */

    mbedtls_x509_name issuer; /**< The parsed issuer data (named information object). */

    mbedtls_x509_time this_update;
    mbedtls_x509_time next_update;

    mbedtls_x509_crl_entry entry; /**< The CRL entries containing the certificate revocation times for this CA. */

    mbedtls_x509_buf crl_ext;

    mbedtls_x509_buf MBEDTLS_PRIVATE(sig_oid2);
    mbedtls_x509_buf MBEDTLS_PRIVATE(sig);
    mbedtls_md_type_t MBEDTLS_PRIVATE(
        sig_md); /**< Internal representation of the MD algorithm of the signature algorithm, e.g. MBEDTLS_MD_SHA256 */
    mbedtls_pk_type_t MBEDTLS_PRIVATE(sig_pk); /**< Internal representation of the Public Key algorithm of the signature
                                                  algorithm, e.g. MBEDTLS_PK_RSA */
    void *MBEDTLS_PRIVATE(
        sig_opts); /**< Signature options to be passed to mbedtls_pk_verify_ext(), e.g. for RSASSA-PSS */

    /** Next element in the linked list of CRL.
     * \p NULL indicates the end of the list.
     * Do not modify this field directly. */
    struct mbedtls_x509_crl *next;
} mbedtls_x509_crl;

#ifdef __cplusplus
}
#endif

#endif // X509_TYPES_H
