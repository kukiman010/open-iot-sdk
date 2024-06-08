/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MBEDTLS_SSL_H
#define MBEDTLS_SSL_H

#include "fff.h"
#include "mbedtls/ecp_types.h"
#include "mbedtls/x509_types.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

/*
 * SSL Error codes
 */
/** A cryptographic operation is in progress. Try again later. */
#define MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS -0x7000
/** The requested feature is not available. */
#define MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE -0x7080
/** Bad input parameters to function. */
#define MBEDTLS_ERR_SSL_BAD_INPUT_DATA -0x7100
/** Verification of the message MAC failed. */
#define MBEDTLS_ERR_SSL_INVALID_MAC -0x7180
/** An invalid SSL record was received. */
#define MBEDTLS_ERR_SSL_INVALID_RECORD -0x7200
/** The connection indicated an EOF. */
#define MBEDTLS_ERR_SSL_CONN_EOF -0x7280
/** A message could not be parsed due to a syntactic error. */
#define MBEDTLS_ERR_SSL_DECODE_ERROR -0x7300
/* Error space gap */
/** No RNG was provided to the SSL module. */
#define MBEDTLS_ERR_SSL_NO_RNG -0x7400
/** No client certification received from the client, but required by the authentication mode. */
#define MBEDTLS_ERR_SSL_NO_CLIENT_CERTIFICATE -0x7480
/** Client received an extended server hello containing an unsupported extension */
#define MBEDTLS_ERR_SSL_UNSUPPORTED_EXTENSION -0x7500
/** No ALPN protocols supported that the client advertises */
#define MBEDTLS_ERR_SSL_NO_APPLICATION_PROTOCOL -0x7580
/** The own private key or pre-shared key is not set, but needed. */
#define MBEDTLS_ERR_SSL_PRIVATE_KEY_REQUIRED -0x7600
/** No CA Chain is set, but required to operate. */
#define MBEDTLS_ERR_SSL_CA_CHAIN_REQUIRED -0x7680
/** An unexpected message was received from our peer. */
#define MBEDTLS_ERR_SSL_UNEXPECTED_MESSAGE -0x7700
/** A fatal alert message was received from our peer. */
#define MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE -0x7780
/** No server could be identified matching the client's SNI. */
#define MBEDTLS_ERR_SSL_UNRECOGNIZED_NAME -0x7800
/** The peer notified us that the connection is going to be closed. */
#define MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY -0x7880
/* Error space gap */
/* Error space gap */
/** Processing of the Certificate handshake message failed. */
#define MBEDTLS_ERR_SSL_BAD_CERTIFICATE -0x7A00
/** Received NewSessionTicket Post Handshake Message */
#define MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET -0x7B00
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/** Memory allocation failed */
#define MBEDTLS_ERR_SSL_ALLOC_FAILED -0x7F00
/** Hardware acceleration function returned with error */
#define MBEDTLS_ERR_SSL_HW_ACCEL_FAILED -0x7F80
/** Hardware acceleration function skipped / left alone data */
#define MBEDTLS_ERR_SSL_HW_ACCEL_FALLTHROUGH -0x6F80
/** Handshake protocol not within min/max boundaries */
#define MBEDTLS_ERR_SSL_BAD_PROTOCOL_VERSION -0x6E80
/** The handshake negotiation failed. */
#define MBEDTLS_ERR_SSL_HANDSHAKE_FAILURE -0x6E00
/** Session ticket has expired. */
#define MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED -0x6D80
/** Public key type mismatch (eg, asked for RSA key exchange and presented EC key) */
#define MBEDTLS_ERR_SSL_PK_TYPE_MISMATCH -0x6D00
/** Unknown identity received (eg, PSK identity) */
#define MBEDTLS_ERR_SSL_UNKNOWN_IDENTITY -0x6C80
/** Internal error (eg, unexpected failure in lower-level module) */
#define MBEDTLS_ERR_SSL_INTERNAL_ERROR -0x6C00
/** A counter would wrap (eg, too many messages exchanged). */
#define MBEDTLS_ERR_SSL_COUNTER_WRAPPING -0x6B80
/** Unexpected message at ServerHello in renegotiation. */
#define MBEDTLS_ERR_SSL_WAITING_SERVER_HELLO_RENEGO -0x6B00
/** DTLS client must retry for hello verification */
#define MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED -0x6A80
/** A buffer is too small to receive or write a message */
#define MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL -0x6A00
/* Error space gap */
/** No data of requested type currently available on underlying transport. */
#define MBEDTLS_ERR_SSL_WANT_READ -0x6900
/** Connection requires a write call. */
#define MBEDTLS_ERR_SSL_WANT_WRITE -0x6880
/** The operation timed out. */
#define MBEDTLS_ERR_SSL_TIMEOUT -0x6800
/** The client initiated a reconnect from the same port. */
#define MBEDTLS_ERR_SSL_CLIENT_RECONNECT -0x6780
/** Record header looks valid but is not expected. */
#define MBEDTLS_ERR_SSL_UNEXPECTED_RECORD -0x6700
/** The alert message received indicates a non-fatal error. */
#define MBEDTLS_ERR_SSL_NON_FATAL -0x6680
/** A field in a message was incorrect or inconsistent with other fields. */
#define MBEDTLS_ERR_SSL_ILLEGAL_PARAMETER -0x6600
/** Internal-only message signaling that further message-processing should be done */
#define MBEDTLS_ERR_SSL_CONTINUE_PROCESSING -0x6580
/** The asynchronous operation is not completed yet. */
#define MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS -0x6500
/** Internal-only message signaling that a message arrived early. */
#define MBEDTLS_ERR_SSL_EARLY_MESSAGE -0x6480
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/* Error space gap */
/** An encrypted DTLS-frame with an unexpected CID was received. */
#define MBEDTLS_ERR_SSL_UNEXPECTED_CID -0x6000
/** An operation failed due to an unexpected version or configuration. */
#define MBEDTLS_ERR_SSL_VERSION_MISMATCH -0x5F00
/** Invalid value in SSL config */
#define MBEDTLS_ERR_SSL_BAD_CONFIG -0x5E80

/*
 * Constants from RFC 8446 for TLS 1.3 PSK modes
 *
 * Those are used in the Pre-Shared Key Exchange Modes extension.
 * See Section 4.2.9 in RFC 8446.
 */
#define MBEDTLS_SSL_TLS1_3_PSK_MODE_PURE  0 /* Pure PSK-based exchange  */
#define MBEDTLS_SSL_TLS1_3_PSK_MODE_ECDHE 1 /* PSK+ECDHE-based exchange */

/*
 * TLS 1.3 NamedGroup values
 *
 * From RF 8446
 *    enum {
 *         // Elliptic Curve Groups (ECDHE)
 *         secp256r1(0x0017), secp384r1(0x0018), secp521r1(0x0019),
 *         x25519(0x001D), x448(0x001E),
 *         // Finite Field Groups (DHE)
 *         ffdhe2048(0x0100), ffdhe3072(0x0101), ffdhe4096(0x0102),
 *         ffdhe6144(0x0103), ffdhe8192(0x0104),
 *         // Reserved Code Points
 *         ffdhe_private_use(0x01FC..0x01FF),
 *         ecdhe_private_use(0xFE00..0xFEFF),
 *         (0xFFFF)
 *     } NamedGroup;
 *
 */

/* Elliptic Curve Groups (ECDHE) */
#define MBEDTLS_SSL_IANA_TLS_GROUP_NONE      0
#define MBEDTLS_SSL_IANA_TLS_GROUP_SECP192K1 0x0012
#define MBEDTLS_SSL_IANA_TLS_GROUP_SECP192R1 0x0013
#define MBEDTLS_SSL_IANA_TLS_GROUP_SECP224K1 0x0014
#define MBEDTLS_SSL_IANA_TLS_GROUP_SECP224R1 0x0015
#define MBEDTLS_SSL_IANA_TLS_GROUP_SECP256K1 0x0016
#define MBEDTLS_SSL_IANA_TLS_GROUP_SECP256R1 0x0017
#define MBEDTLS_SSL_IANA_TLS_GROUP_SECP384R1 0x0018
#define MBEDTLS_SSL_IANA_TLS_GROUP_SECP521R1 0x0019
#define MBEDTLS_SSL_IANA_TLS_GROUP_BP256R1   0x001A
#define MBEDTLS_SSL_IANA_TLS_GROUP_BP384R1   0x001B
#define MBEDTLS_SSL_IANA_TLS_GROUP_BP512R1   0x001C
#define MBEDTLS_SSL_IANA_TLS_GROUP_X25519    0x001D
#define MBEDTLS_SSL_IANA_TLS_GROUP_X448      0x001E
/* Finite Field Groups (DHE) */
#define MBEDTLS_SSL_IANA_TLS_GROUP_FFDHE2048 0x0100
#define MBEDTLS_SSL_IANA_TLS_GROUP_FFDHE3072 0x0101
#define MBEDTLS_SSL_IANA_TLS_GROUP_FFDHE4096 0x0102
#define MBEDTLS_SSL_IANA_TLS_GROUP_FFDHE6144 0x0103
#define MBEDTLS_SSL_IANA_TLS_GROUP_FFDHE8192 0x0104

/*
 * TLS 1.3 Key Exchange Modes
 *
 * Mbed TLS internal identifiers for use with the SSL configuration API
 * mbedtls_ssl_conf_tls13_key_exchange_modes().
 */

#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK            \
    (1u << 0) /*!< Pure-PSK TLS 1.3 key exchange,           \
               *   encompassing both externally agreed PSKs \
               *   as well as resumption PSKs. */
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL   \
    (1u << 1) /*!< Pure-Ephemeral TLS 1.3 key exchanges, \
               *   including for example ECDHE and DHE   \
               *   key exchanges. */
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL \
    (1u << 2) /*!< PSK-Ephemeral TLS 1.3 key exchanges,    \
               *   using both a PSK and an ephemeral       \
               *   key exchange. */

/* Convenience macros for sets of key exchanges. */
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_ALL                                                   \
    (MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK | MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL \
     | MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL) /*!< All TLS 1.3 key exchanges           */
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ALL \
    (MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK        \
     | MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL) /*!< All PSK-based TLS 1.3 key exchanges */
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ALL \
    (MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL        \
     | MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL) /*!< All ephemeral TLS 1.3 key exchanges */

#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_NONE (0)

/*
 * Various constants
 */

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
/* These are the high and low bytes of ProtocolVersion as defined by:
 * - RFC 5246: ProtocolVersion version = { 3, 3 };     // TLS v1.2
 * - RFC 8446: see section 4.2.1
 */
#define MBEDTLS_SSL_MAJOR_VERSION_3 3
#define MBEDTLS_SSL_MINOR_VERSION_3 3 /*!< TLS v1.2 */
#define MBEDTLS_SSL_MINOR_VERSION_4 4 /*!< TLS v1.3 */
#endif                                /* MBEDTLS_DEPRECATED_REMOVED */

#define MBEDTLS_SSL_TRANSPORT_STREAM   0 /*!< TLS      */
#define MBEDTLS_SSL_TRANSPORT_DATAGRAM 1 /*!< DTLS     */

#define MBEDTLS_SSL_MAX_HOST_NAME_LEN 255 /*!< Maximum host name defined in RFC 1035 */
#define MBEDTLS_SSL_MAX_ALPN_NAME_LEN 255 /*!< Maximum size in bytes of a protocol name in alpn ext., RFC 7301 */

#define MBEDTLS_SSL_MAX_ALPN_LIST_LEN 65535 /*!< Maximum size in bytes of list in alpn ext., RFC 7301          */

/* RFC 6066 section 4, see also mfl_code_to_length in ssl_tls.c
 * NONE must be zero so that memset()ing structure to zero works */
#define MBEDTLS_SSL_MAX_FRAG_LEN_NONE    0 /*!< don't use this extension   */
#define MBEDTLS_SSL_MAX_FRAG_LEN_512     1 /*!< MaxFragmentLength 2^9      */
#define MBEDTLS_SSL_MAX_FRAG_LEN_1024    2 /*!< MaxFragmentLength 2^10     */
#define MBEDTLS_SSL_MAX_FRAG_LEN_2048    3 /*!< MaxFragmentLength 2^11     */
#define MBEDTLS_SSL_MAX_FRAG_LEN_4096    4 /*!< MaxFragmentLength 2^12     */
#define MBEDTLS_SSL_MAX_FRAG_LEN_INVALID 5 /*!< first invalid value        */

#define MBEDTLS_SSL_IS_CLIENT 0
#define MBEDTLS_SSL_IS_SERVER 1

#define MBEDTLS_SSL_EXTENDED_MS_DISABLED 0
#define MBEDTLS_SSL_EXTENDED_MS_ENABLED  1

#define MBEDTLS_SSL_CID_DISABLED 0
#define MBEDTLS_SSL_CID_ENABLED  1

#define MBEDTLS_SSL_ETM_DISABLED 0
#define MBEDTLS_SSL_ETM_ENABLED  1

#define MBEDTLS_SSL_COMPRESS_NULL 0

#define MBEDTLS_SSL_VERIFY_NONE     0
#define MBEDTLS_SSL_VERIFY_OPTIONAL 1
#define MBEDTLS_SSL_VERIFY_REQUIRED 2
#define MBEDTLS_SSL_VERIFY_UNSET    3 /* Used only for sni_authmode */

#define MBEDTLS_SSL_LEGACY_RENEGOTIATION 0
#define MBEDTLS_SSL_SECURE_RENEGOTIATION 1

#define MBEDTLS_SSL_RENEGOTIATION_DISABLED 0
#define MBEDTLS_SSL_RENEGOTIATION_ENABLED  1

#define MBEDTLS_SSL_ANTI_REPLAY_DISABLED 0
#define MBEDTLS_SSL_ANTI_REPLAY_ENABLED  1

#define MBEDTLS_SSL_RENEGOTIATION_NOT_ENFORCED -1
#define MBEDTLS_SSL_RENEGO_MAX_RECORDS_DEFAULT 16

#define MBEDTLS_SSL_LEGACY_NO_RENEGOTIATION    0
#define MBEDTLS_SSL_LEGACY_ALLOW_RENEGOTIATION 1
#define MBEDTLS_SSL_LEGACY_BREAK_HANDSHAKE     2

#define MBEDTLS_SSL_TRUNC_HMAC_DISABLED 0
#define MBEDTLS_SSL_TRUNC_HMAC_ENABLED  1
#define MBEDTLS_SSL_TRUNCATED_HMAC_LEN  10 /* 80 bits, rfc 6066 section 7 */

#define MBEDTLS_SSL_SESSION_TICKETS_DISABLED 0
#define MBEDTLS_SSL_SESSION_TICKETS_ENABLED  1

#define MBEDTLS_SSL_PRESET_DEFAULT 0
#define MBEDTLS_SSL_PRESET_SUITEB  2

#define MBEDTLS_SSL_CERT_REQ_CA_LIST_ENABLED  1
#define MBEDTLS_SSL_CERT_REQ_CA_LIST_DISABLED 0

#define MBEDTLS_SSL_DTLS_SRTP_MKI_UNSUPPORTED 0
#define MBEDTLS_SSL_DTLS_SRTP_MKI_SUPPORTED   1

#define MBEDTLS_SSL_SRV_CIPHERSUITE_ORDER_CLIENT 1
#define MBEDTLS_SSL_SRV_CIPHERSUITE_ORDER_SERVER 0

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(MBEDTLS_SSL_SESSION_TICKETS)
#if defined(MBEDTLS_SHA384_C)
#define MBEDTLS_SSL_TLS1_3_TICKET_RESUMPTION_KEY_LEN 48
#elif defined(MBEDTLS_SHA256_C)
#define MBEDTLS_SSL_TLS1_3_TICKET_RESUMPTION_KEY_LEN 32
#endif /* MBEDTLS_SHA256_C  */
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 && MBEDTLS_SSL_SESSION_TICKETS */
/*
 * Default range for DTLS retransmission timer value, in milliseconds.
 * RFC 6347 4.2.4.1 says from 1 second to 60 seconds.
 */
#define MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MIN 1000
#define MBEDTLS_SSL_DTLS_TIMEOUT_DFL_MAX 60000

/**
 * \name SECTION: Module settings
 *
 * The configuration options you can set for this module are in this section.
 * Either change them in mbedtls_config.h or define them on the compiler command line.
 * \{
 */

/*
 * Maximum fragment length in bytes,
 * determines the size of each of the two internal I/O buffers.
 *
 * Note: the RFC defines the default size of SSL / TLS messages. If you
 * change the value here, other clients / servers may not be able to
 * communicate with you anymore. Only change this value if you control
 * both sides of the connection and have it reduced at both sides, or
 * if you're using the Max Fragment Length extension and you know all your
 * peers are using it too!
 */
#if !defined(MBEDTLS_SSL_IN_CONTENT_LEN)
#define MBEDTLS_SSL_IN_CONTENT_LEN 16384
#endif

#if !defined(MBEDTLS_SSL_OUT_CONTENT_LEN)
#define MBEDTLS_SSL_OUT_CONTENT_LEN 16384
#endif

/*
 * Maximum number of heap-allocated bytes for the purpose of
 * DTLS handshake message reassembly and future message buffering.
 */
#if !defined(MBEDTLS_SSL_DTLS_MAX_BUFFERING)
#define MBEDTLS_SSL_DTLS_MAX_BUFFERING 32768
#endif

/*
 * Maximum length of CIDs for incoming and outgoing messages.
 */
#if !defined(MBEDTLS_SSL_CID_IN_LEN_MAX)
#define MBEDTLS_SSL_CID_IN_LEN_MAX 32
#endif

#if !defined(MBEDTLS_SSL_CID_OUT_LEN_MAX)
#define MBEDTLS_SSL_CID_OUT_LEN_MAX 32
#endif

#if !defined(MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY)
#define MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY 16
#endif

/** \} name SECTION: Module settings */

/*
 * Length of the verify data for secure renegotiation
 */
#define MBEDTLS_SSL_VERIFY_DATA_MAX_LEN 12

/*
 * Signaling ciphersuite values (SCSV)
 */
#define MBEDTLS_SSL_EMPTY_RENEGOTIATION_INFO 0xFF /**< renegotiation info ext */

/*
 * Supported Signature and Hash algorithms (For TLS 1.2)
 * RFC 5246 section 7.4.1.4.1
 */
#define MBEDTLS_SSL_HASH_NONE   0
#define MBEDTLS_SSL_HASH_MD5    1
#define MBEDTLS_SSL_HASH_SHA1   2
#define MBEDTLS_SSL_HASH_SHA224 3
#define MBEDTLS_SSL_HASH_SHA256 4
#define MBEDTLS_SSL_HASH_SHA384 5
#define MBEDTLS_SSL_HASH_SHA512 6

#define MBEDTLS_SSL_SIG_ANON  0
#define MBEDTLS_SSL_SIG_RSA   1
#define MBEDTLS_SSL_SIG_ECDSA 3

/*
 * TLS 1.3 signature algorithms
 * RFC 8446, Section 4.2.2
 */

/* RSASSA-PKCS1-v1_5 algorithms */
#define MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA256 0x0401
#define MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA384 0x0501
#define MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA512 0x0601

/* ECDSA algorithms */
#define MBEDTLS_TLS1_3_SIG_ECDSA_SECP256R1_SHA256 0x0403
#define MBEDTLS_TLS1_3_SIG_ECDSA_SECP384R1_SHA384 0x0503
#define MBEDTLS_TLS1_3_SIG_ECDSA_SECP521R1_SHA512 0x0603

/* RSASSA-PSS algorithms with public key OID rsaEncryption */
#define MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA256 0x0804
#define MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA384 0x0805
#define MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA512 0x0806

/* EdDSA algorithms */
#define MBEDTLS_TLS1_3_SIG_ED25519 0x0807
#define MBEDTLS_TLS1_3_SIG_ED448   0x0808

/* RSASSA-PSS algorithms with public key OID RSASSA-PSS  */
#define MBEDTLS_TLS1_3_SIG_RSA_PSS_PSS_SHA256 0x0809
#define MBEDTLS_TLS1_3_SIG_RSA_PSS_PSS_SHA384 0x080A
#define MBEDTLS_TLS1_3_SIG_RSA_PSS_PSS_SHA512 0x080B

/* LEGACY ALGORITHMS */
#define MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA1 0x0201
#define MBEDTLS_TLS1_3_SIG_ECDSA_SHA1     0x0203

#define MBEDTLS_TLS1_3_SIG_NONE 0x0

/*
 * Client Certificate Types
 * RFC 5246 section 7.4.4 plus RFC 4492 section 5.5
 */
#define MBEDTLS_SSL_CERT_TYPE_RSA_SIGN   1
#define MBEDTLS_SSL_CERT_TYPE_ECDSA_SIGN 64

/*
 * Message, alert and handshake types
 */
#define MBEDTLS_SSL_MSG_CHANGE_CIPHER_SPEC 20
#define MBEDTLS_SSL_MSG_ALERT              21
#define MBEDTLS_SSL_MSG_HANDSHAKE          22
#define MBEDTLS_SSL_MSG_APPLICATION_DATA   23
#define MBEDTLS_SSL_MSG_CID                25

#define MBEDTLS_SSL_ALERT_LEVEL_WARNING 1
#define MBEDTLS_SSL_ALERT_LEVEL_FATAL   2

#define MBEDTLS_SSL_ALERT_MSG_CLOSE_NOTIFY            0   /* 0x00 */
#define MBEDTLS_SSL_ALERT_MSG_UNEXPECTED_MESSAGE      10  /* 0x0A */
#define MBEDTLS_SSL_ALERT_MSG_BAD_RECORD_MAC          20  /* 0x14 */
#define MBEDTLS_SSL_ALERT_MSG_DECRYPTION_FAILED       21  /* 0x15 */
#define MBEDTLS_SSL_ALERT_MSG_RECORD_OVERFLOW         22  /* 0x16 */
#define MBEDTLS_SSL_ALERT_MSG_DECOMPRESSION_FAILURE   30  /* 0x1E */
#define MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE       40  /* 0x28 */
#define MBEDTLS_SSL_ALERT_MSG_NO_CERT                 41  /* 0x29 */
#define MBEDTLS_SSL_ALERT_MSG_BAD_CERT                42  /* 0x2A */
#define MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT        43  /* 0x2B */
#define MBEDTLS_SSL_ALERT_MSG_CERT_REVOKED            44  /* 0x2C */
#define MBEDTLS_SSL_ALERT_MSG_CERT_EXPIRED            45  /* 0x2D */
#define MBEDTLS_SSL_ALERT_MSG_CERT_UNKNOWN            46  /* 0x2E */
#define MBEDTLS_SSL_ALERT_MSG_ILLEGAL_PARAMETER       47  /* 0x2F */
#define MBEDTLS_SSL_ALERT_MSG_UNKNOWN_CA              48  /* 0x30 */
#define MBEDTLS_SSL_ALERT_MSG_ACCESS_DENIED           49  /* 0x31 */
#define MBEDTLS_SSL_ALERT_MSG_DECODE_ERROR            50  /* 0x32 */
#define MBEDTLS_SSL_ALERT_MSG_DECRYPT_ERROR           51  /* 0x33 */
#define MBEDTLS_SSL_ALERT_MSG_EXPORT_RESTRICTION      60  /* 0x3C */
#define MBEDTLS_SSL_ALERT_MSG_PROTOCOL_VERSION        70  /* 0x46 */
#define MBEDTLS_SSL_ALERT_MSG_INSUFFICIENT_SECURITY   71  /* 0x47 */
#define MBEDTLS_SSL_ALERT_MSG_INTERNAL_ERROR          80  /* 0x50 */
#define MBEDTLS_SSL_ALERT_MSG_INAPROPRIATE_FALLBACK   86  /* 0x56 */
#define MBEDTLS_SSL_ALERT_MSG_USER_CANCELED           90  /* 0x5A */
#define MBEDTLS_SSL_ALERT_MSG_NO_RENEGOTIATION        100 /* 0x64 */
#define MBEDTLS_SSL_ALERT_MSG_MISSING_EXTENSION       109 /* 0x6d -- new in TLS 1.3 */
#define MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_EXT         110 /* 0x6E */
#define MBEDTLS_SSL_ALERT_MSG_UNRECOGNIZED_NAME       112 /* 0x70 */
#define MBEDTLS_SSL_ALERT_MSG_UNKNOWN_PSK_IDENTITY    115 /* 0x73 */
#define MBEDTLS_SSL_ALERT_MSG_CERT_REQUIRED           116 /* 0x74 */
#define MBEDTLS_SSL_ALERT_MSG_NO_APPLICATION_PROTOCOL 120 /* 0x78 */

#define MBEDTLS_SSL_HS_HELLO_REQUEST        0
#define MBEDTLS_SSL_HS_CLIENT_HELLO         1
#define MBEDTLS_SSL_HS_SERVER_HELLO         2
#define MBEDTLS_SSL_HS_HELLO_VERIFY_REQUEST 3
#define MBEDTLS_SSL_HS_NEW_SESSION_TICKET   4
#define MBEDTLS_SSL_HS_ENCRYPTED_EXTENSIONS 8 // NEW IN TLS 1.3
#define MBEDTLS_SSL_HS_CERTIFICATE          11
#define MBEDTLS_SSL_HS_SERVER_KEY_EXCHANGE  12
#define MBEDTLS_SSL_HS_CERTIFICATE_REQUEST  13
#define MBEDTLS_SSL_HS_SERVER_HELLO_DONE    14
#define MBEDTLS_SSL_HS_CERTIFICATE_VERIFY   15
#define MBEDTLS_SSL_HS_CLIENT_KEY_EXCHANGE  16
#define MBEDTLS_SSL_HS_FINISHED             20
#define MBEDTLS_SSL_HS_MESSAGE_HASH         254

/*
 * TLS extensions
 */
#define MBEDTLS_TLS_EXT_SERVERNAME          0
#define MBEDTLS_TLS_EXT_SERVERNAME_HOSTNAME 0

#define MBEDTLS_TLS_EXT_MAX_FRAGMENT_LENGTH 1

#define MBEDTLS_TLS_EXT_TRUNCATED_HMAC 4
#define MBEDTLS_TLS_EXT_STATUS_REQUEST 5 /* RFC 6066 TLS 1.2 and 1.3 */

#define MBEDTLS_TLS_EXT_SUPPORTED_ELLIPTIC_CURVES 10
#define MBEDTLS_TLS_EXT_SUPPORTED_GROUPS          10 /* RFC 8422,7919 TLS 1.2 and 1.3 */
#define MBEDTLS_TLS_EXT_SUPPORTED_POINT_FORMATS   11

#define MBEDTLS_TLS_EXT_SIG_ALG   13 /* RFC 8446 TLS 1.3 */
#define MBEDTLS_TLS_EXT_USE_SRTP  14
#define MBEDTLS_TLS_EXT_HEARTBEAT 15 /* RFC 6520 TLS 1.2 and 1.3 */
#define MBEDTLS_TLS_EXT_ALPN      16

#define MBEDTLS_TLS_EXT_SCT                    18     /* RFC 6962 TLS 1.2 and 1.3 */
#define MBEDTLS_TLS_EXT_CLI_CERT_TYPE          19     /* RFC 7250 TLS 1.2 and 1.3 */
#define MBEDTLS_TLS_EXT_SERV_CERT_TYPE         20     /* RFC 7250 TLS 1.2 and 1.3 */
#define MBEDTLS_TLS_EXT_PADDING                21     /* RFC 7685 TLS 1.2 and 1.3 */
#define MBEDTLS_TLS_EXT_ENCRYPT_THEN_MAC       22     /* 0x16 */
#define MBEDTLS_TLS_EXT_EXTENDED_MASTER_SECRET 0x0017 /* 23 */

#define MBEDTLS_TLS_EXT_SESSION_TICKET 35

#define MBEDTLS_TLS_EXT_PRE_SHARED_KEY         41 /* RFC 8446 TLS 1.3 */
#define MBEDTLS_TLS_EXT_EARLY_DATA             42 /* RFC 8446 TLS 1.3 */
#define MBEDTLS_TLS_EXT_SUPPORTED_VERSIONS     43 /* RFC 8446 TLS 1.3 */
#define MBEDTLS_TLS_EXT_COOKIE                 44 /* RFC 8446 TLS 1.3 */
#define MBEDTLS_TLS_EXT_PSK_KEY_EXCHANGE_MODES 45 /* RFC 8446 TLS 1.3 */

#define MBEDTLS_TLS_EXT_CERT_AUTH           47 /* RFC 8446 TLS 1.3 */
#define MBEDTLS_TLS_EXT_OID_FILTERS         48 /* RFC 8446 TLS 1.3 */
#define MBEDTLS_TLS_EXT_POST_HANDSHAKE_AUTH 49 /* RFC 8446 TLS 1.3 */
#define MBEDTLS_TLS_EXT_SIG_ALG_CERT        50 /* RFC 8446 TLS 1.3 */
#define MBEDTLS_TLS_EXT_KEY_SHARE           51 /* RFC 8446 TLS 1.3 */

/* The value of the CID extension is still TBD as of
 * draft-ietf-tls-dtls-connection-id-05
 * (https://tools.ietf.org/html/draft-ietf-tls-dtls-connection-id-05).
 *
 * A future minor revision of Mbed TLS may change the default value of
 * this option to match evolving standards and usage.
 */
#if !defined(MBEDTLS_TLS_EXT_CID)
#define MBEDTLS_TLS_EXT_CID 254 /* TBD */
#endif

#define MBEDTLS_TLS_EXT_ECJPAKE_KKPP 256 /* experimental */

#define MBEDTLS_TLS_EXT_RENEGOTIATION_INFO 0xFF01

/*
 * Size defines
 */
#if !defined(MBEDTLS_PSK_MAX_LEN)
#define MBEDTLS_PSK_MAX_LEN 32 /* 256 bits */
#endif

/* Dummy type used only for its size */
union mbedtls_ssl_premaster_secret {
#if defined(MBEDTLS_KEY_EXCHANGE_RSA_ENABLED)
    unsigned char _pms_rsa[48]; /* RFC 5246 8.1.1 */
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED)
    unsigned char _pms_dhm[MBEDTLS_MPI_MAX_SIZE]; /* RFC 5246 8.1.2 */
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED) || defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED) \
    || defined(MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED) || defined(MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED)
    unsigned char _pms_ecdh[MBEDTLS_ECP_MAX_BYTES]; /* RFC 4492 5.10 */
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_PSK_ENABLED)
    unsigned char _pms_psk[4 + 2 * MBEDTLS_PSK_MAX_LEN]; /* RFC 4279 2 */
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED)
    unsigned char _pms_dhe_psk[4 + MBEDTLS_MPI_MAX_SIZE + MBEDTLS_PSK_MAX_LEN]; /* RFC 4279 3 */
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED)
    unsigned char _pms_rsa_psk[52 + MBEDTLS_PSK_MAX_LEN]; /* RFC 4279 4 */
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED)
    unsigned char _pms_ecdhe_psk[4 + MBEDTLS_ECP_MAX_BYTES + MBEDTLS_PSK_MAX_LEN]; /* RFC 5489 2 */
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    unsigned char _pms_ecjpake[32]; /* Thread spec: SHA-256 output */
#endif
};

#define MBEDTLS_PREMASTER_SIZE sizeof(union mbedtls_ssl_premaster_secret)

#define MBEDTLS_TLS1_3_MD_MAX_SIZE MBEDTLS_MD_MAX_SIZE

/* Length in number of bytes of the TLS sequence number */
#define MBEDTLS_SSL_SEQUENCE_NUMBER_LEN 8

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SSL state machine
 */
typedef enum {
    MBEDTLS_SSL_HELLO_REQUEST,
    MBEDTLS_SSL_CLIENT_HELLO,
    MBEDTLS_SSL_SERVER_HELLO,
    MBEDTLS_SSL_SERVER_CERTIFICATE,
    MBEDTLS_SSL_SERVER_KEY_EXCHANGE,
    MBEDTLS_SSL_CERTIFICATE_REQUEST,
    MBEDTLS_SSL_SERVER_HELLO_DONE,
    MBEDTLS_SSL_CLIENT_CERTIFICATE,
    MBEDTLS_SSL_CLIENT_KEY_EXCHANGE,
    MBEDTLS_SSL_CERTIFICATE_VERIFY,
    MBEDTLS_SSL_CLIENT_CHANGE_CIPHER_SPEC,
    MBEDTLS_SSL_CLIENT_FINISHED,
    MBEDTLS_SSL_SERVER_CHANGE_CIPHER_SPEC,
    MBEDTLS_SSL_SERVER_FINISHED,
    MBEDTLS_SSL_FLUSH_BUFFERS,
    MBEDTLS_SSL_HANDSHAKE_WRAPUP,
    MBEDTLS_SSL_HANDSHAKE_OVER,
    MBEDTLS_SSL_NEW_SESSION_TICKET,
    MBEDTLS_SSL_SERVER_HELLO_VERIFY_REQUEST_SENT,
    MBEDTLS_SSL_HELLO_RETRY_REQUEST,
    MBEDTLS_SSL_ENCRYPTED_EXTENSIONS,
    MBEDTLS_SSL_CLIENT_CERTIFICATE_VERIFY,
    MBEDTLS_SSL_CLIENT_CCS_AFTER_SERVER_FINISHED,
    MBEDTLS_SSL_CLIENT_CCS_BEFORE_2ND_CLIENT_HELLO,
    MBEDTLS_SSL_SERVER_CCS_AFTER_SERVER_HELLO,
    MBEDTLS_SSL_SERVER_CCS_AFTER_HELLO_RETRY_REQUEST,
    MBEDTLS_SSL_NEW_SESSION_TICKET_FLUSH,
} mbedtls_ssl_states;

/**
 * \brief          Callback type: send data on the network.
 *
 * \note           That callback may be either blocking or non-blocking.
 *
 * \param ctx      Context for the send callback (typically a file descriptor)
 * \param buf      Buffer holding the data to send
 * \param len      Length of the data to send
 *
 * \return         The callback must return the number of bytes sent if any,
 *                 or a non-zero error code.
 *                 If performing non-blocking I/O, \c MBEDTLS_ERR_SSL_WANT_WRITE
 *                 must be returned when the operation would block.
 *
 * \note           The callback is allowed to send fewer bytes than requested.
 *                 It must always return the number of bytes actually sent.
 */
typedef int mbedtls_ssl_send_t(void *ctx, const unsigned char *buf, size_t len);

/**
 * \brief          Callback type: receive data from the network.
 *
 * \note           That callback may be either blocking or non-blocking.
 *
 * \param ctx      Context for the receive callback (typically a file
 *                 descriptor)
 * \param buf      Buffer to write the received data to
 * \param len      Length of the receive buffer
 *
 * \returns        If data has been received, the positive number of bytes received.
 * \returns        \c 0 if the connection has been closed.
 * \returns        If performing non-blocking I/O, \c MBEDTLS_ERR_SSL_WANT_READ
 *                 must be returned when the operation would block.
 * \returns        Another negative error code on other kinds of failures.
 *
 * \note           The callback may receive fewer bytes than the length of the
 *                 buffer. It must always return the number of bytes actually
 *                 received and written to the buffer.
 */
typedef int mbedtls_ssl_recv_t(void *ctx, unsigned char *buf, size_t len);

/**
 * \brief          Callback type: receive data from the network, with timeout
 *
 * \note           That callback must block until data is received, or the
 *                 timeout delay expires, or the operation is interrupted by a
 *                 signal.
 *
 * \param ctx      Context for the receive callback (typically a file descriptor)
 * \param buf      Buffer to write the received data to
 * \param len      Length of the receive buffer
 * \param timeout  Maximum number of milliseconds to wait for data
 *                 0 means no timeout (potentially waiting forever)
 *
 * \return         The callback must return the number of bytes received,
 *                 or a non-zero error code:
 *                 \c MBEDTLS_ERR_SSL_TIMEOUT if the operation timed out,
 *                 \c MBEDTLS_ERR_SSL_WANT_READ if interrupted by a signal.
 *
 * \note           The callback may receive fewer bytes than the length of the
 *                 buffer. It must always return the number of bytes actually
 *                 received and written to the buffer.
 */
typedef int mbedtls_ssl_recv_timeout_t(void *ctx, unsigned char *buf, size_t len, uint32_t timeout);
/**
 * \brief          Callback type: set a pair of timers/delays to watch
 *
 * \param ctx      Context pointer
 * \param int_ms   Intermediate delay in milliseconds
 * \param fin_ms   Final delay in milliseconds
 *                 0 cancels the current timer.
 *
 * \note           This callback must at least store the necessary information
 *                 for the associated \c mbedtls_ssl_get_timer_t callback to
 *                 return correct information.
 *
 * \note           If using an event-driven style of programming, an event must
 *                 be generated when the final delay is passed. The event must
 *                 cause a call to \c mbedtls_ssl_handshake() with the proper
 *                 SSL context to be scheduled. Care must be taken to ensure
 *                 that at most one such call happens at a time.
 *
 * \note           Only one timer at a time must be running. Calling this
 *                 function while a timer is running must cancel it. Cancelled
 *                 timers must not generate any event.
 */
typedef void mbedtls_ssl_set_timer_t(void *ctx, uint32_t int_ms, uint32_t fin_ms);

/**
 * \brief          Callback type: get status of timers/delays
 *
 * \param ctx      Context pointer
 *
 * \return         This callback must return:
 *                 -1 if cancelled (fin_ms == 0),
 *                  0 if none of the delays have passed,
 *                  1 if only the intermediate delay has passed,
 *                  2 if the final delay has passed.
 */
typedef int mbedtls_ssl_get_timer_t(void *ctx);

/* Defined below */
typedef struct mbedtls_ssl_session mbedtls_ssl_session;
typedef struct mbedtls_ssl_context mbedtls_ssl_context;
typedef struct mbedtls_ssl_config mbedtls_ssl_config;

/* Defined in library/ssl_misc.h */
typedef struct mbedtls_ssl_transform mbedtls_ssl_transform;
typedef struct mbedtls_ssl_handshake_params mbedtls_ssl_handshake_params;
typedef struct mbedtls_ssl_sig_hash_set_t mbedtls_ssl_sig_hash_set_t;
#if defined(MBEDTLS_X509_CRT_PARSE_C)
typedef struct mbedtls_ssl_key_cert mbedtls_ssl_key_cert;
#endif
#if defined(MBEDTLS_SSL_PROTO_DTLS)
typedef struct mbedtls_ssl_flight_item mbedtls_ssl_flight_item;
#endif

/**
 * \brief          Callback type: server-side session cache getter
 *
 *                 The session cache is logically a key value store, with
 *                 keys being session IDs and values being instances of
 *                 mbedtls_ssl_session.
 *
 *                 This callback retrieves an entry in this key-value store.
 *
 * \param data            The address of the session cache structure to query.
 * \param session_id      The buffer holding the session ID to query.
 * \param session_id_len  The length of \p session_id in Bytes.
 * \param session         The address of the session structure to populate.
 *                        It is initialized with mbdtls_ssl_session_init(),
 *                        and the callback must always leave it in a state
 *                        where it can safely be freed via
 *                        mbedtls_ssl_session_free() independent of the
 *                        return code of this function.
 *
 * \return                \c 0 on success
 * \return                A non-zero return value on failure.
 *
 */
typedef int mbedtls_ssl_cache_get_t(void *data,
                                    unsigned char const *session_id,
                                    size_t session_id_len,
                                    mbedtls_ssl_session *session);
/**
 * \brief          Callback type: server-side session cache setter
 *
 *                 The session cache is logically a key value store, with
 *                 keys being session IDs and values being instances of
 *                 mbedtls_ssl_session.
 *
 *                 This callback sets an entry in this key-value store.
 *
 * \param data            The address of the session cache structure to modify.
 * \param session_id      The buffer holding the session ID to query.
 * \param session_id_len  The length of \p session_id in Bytes.
 * \param session         The address of the session to be stored in the
 *                        session cache.
 *
 * \return                \c 0 on success
 * \return                A non-zero return value on failure.
 */
typedef int mbedtls_ssl_cache_set_t(void *data,
                                    unsigned char const *session_id,
                                    size_t session_id_len,
                                    const mbedtls_ssl_session *session);

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
/**
 * \brief           Callback type: start external signature operation.
 *
 *                  This callback is called during an SSL handshake to start
 *                  a signature decryption operation using an
 *                  external processor. The parameter \p cert contains
 *                  the public key; it is up to the callback function to
 *                  determine how to access the associated private key.
 *
 *                  This function typically sends or enqueues a request, and
 *                  does not wait for the operation to complete. This allows
 *                  the handshake step to be non-blocking.
 *
 *                  The parameters \p ssl and \p cert are guaranteed to remain
 *                  valid throughout the handshake. On the other hand, this
 *                  function must save the contents of \p hash if the value
 *                  is needed for later processing, because the \p hash buffer
 *                  is no longer valid after this function returns.
 *
 *                  This function may call mbedtls_ssl_set_async_operation_data()
 *                  to store an operation context for later retrieval
 *                  by the resume or cancel callback.
 *
 * \note            For RSA signatures, this function must produce output
 *                  that is consistent with PKCS#1 v1.5 in the same way as
 *                  mbedtls_rsa_pkcs1_sign(). Before the private key operation,
 *                  apply the padding steps described in RFC 8017, section 9.2
 *                  "EMSA-PKCS1-v1_5" as follows.
 *                  - If \p md_alg is #MBEDTLS_MD_NONE, apply the PKCS#1 v1.5
 *                    encoding, treating \p hash as the DigestInfo to be
 *                    padded. In other words, apply EMSA-PKCS1-v1_5 starting
 *                    from step 3, with `T = hash` and `tLen = hash_len`.
 *                  - If `md_alg != MBEDTLS_MD_NONE`, apply the PKCS#1 v1.5
 *                    encoding, treating \p hash as the hash to be encoded and
 *                    padded. In other words, apply EMSA-PKCS1-v1_5 starting
 *                    from step 2, with `digestAlgorithm` obtained by calling
 *                    mbedtls_oid_get_oid_by_md() on \p md_alg.
 *
 * \note            For ECDSA signatures, the output format is the DER encoding
 *                  `Ecdsa-Sig-Value` defined in
 *                  [RFC 4492 section 5.4](https://tools.ietf.org/html/rfc4492#section-5.4).
 *
 * \param ssl             The SSL connection instance. It should not be
 *                        modified other than via
 *                        mbedtls_ssl_set_async_operation_data().
 * \param cert            Certificate containing the public key.
 *                        In simple cases, this is one of the pointers passed to
 *                        mbedtls_ssl_conf_own_cert() when configuring the SSL
 *                        connection. However, if other callbacks are used, this
 *                        property may not hold. For example, if an SNI callback
 *                        is registered with mbedtls_ssl_conf_sni(), then
 *                        this callback determines what certificate is used.
 * \param md_alg          Hash algorithm.
 * \param hash            Buffer containing the hash. This buffer is
 *                        no longer valid when the function returns.
 * \param hash_len        Size of the \c hash buffer in bytes.
 *
 * \return          0 if the operation was started successfully and the SSL
 *                  stack should call the resume callback immediately.
 * \return          #MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS if the operation
 *                  was started successfully and the SSL stack should return
 *                  immediately without calling the resume callback yet.
 * \return          #MBEDTLS_ERR_SSL_HW_ACCEL_FALLTHROUGH if the external
 *                  processor does not support this key. The SSL stack will
 *                  use the private key object instead.
 * \return          Any other error indicates a fatal failure and is
 *                  propagated up the call chain. The callback should
 *                  use \c MBEDTLS_ERR_PK_xxx error codes, and <b>must not</b>
 *                  use \c MBEDTLS_ERR_SSL_xxx error codes except as
 *                  directed in the documentation of this callback.
 */
typedef int mbedtls_ssl_async_sign_t(mbedtls_ssl_context *ssl,
                                     mbedtls_x509_crt *cert,
                                     mbedtls_md_type_t md_alg,
                                     const unsigned char *hash,
                                     size_t hash_len);

/**
 * \brief           Callback type: start external decryption operation.
 *
 *                  This callback is called during an SSL handshake to start
 *                  an RSA decryption operation using an
 *                  external processor. The parameter \p cert contains
 *                  the public key; it is up to the callback function to
 *                  determine how to access the associated private key.
 *
 *                  This function typically sends or enqueues a request, and
 *                  does not wait for the operation to complete. This allows
 *                  the handshake step to be non-blocking.
 *
 *                  The parameters \p ssl and \p cert are guaranteed to remain
 *                  valid throughout the handshake. On the other hand, this
 *                  function must save the contents of \p input if the value
 *                  is needed for later processing, because the \p input buffer
 *                  is no longer valid after this function returns.
 *
 *                  This function may call mbedtls_ssl_set_async_operation_data()
 *                  to store an operation context for later retrieval
 *                  by the resume or cancel callback.
 *
 * \warning         RSA decryption as used in TLS is subject to a potential
 *                  timing side channel attack first discovered by Bleichenbacher
 *                  in 1998. This attack can be remotely exploitable
 *                  in practice. To avoid this attack, you must ensure that
 *                  if the callback performs an RSA decryption, the time it
 *                  takes to execute and return the result does not depend
 *                  on whether the RSA decryption succeeded or reported
 *                  invalid padding.
 *
 * \param ssl             The SSL connection instance. It should not be
 *                        modified other than via
 *                        mbedtls_ssl_set_async_operation_data().
 * \param cert            Certificate containing the public key.
 *                        In simple cases, this is one of the pointers passed to
 *                        mbedtls_ssl_conf_own_cert() when configuring the SSL
 *                        connection. However, if other callbacks are used, this
 *                        property may not hold. For example, if an SNI callback
 *                        is registered with mbedtls_ssl_conf_sni(), then
 *                        this callback determines what certificate is used.
 * \param input           Buffer containing the input ciphertext. This buffer
 *                        is no longer valid when the function returns.
 * \param input_len       Size of the \p input buffer in bytes.
 *
 * \return          0 if the operation was started successfully and the SSL
 *                  stack should call the resume callback immediately.
 * \return          #MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS if the operation
 *                  was started successfully and the SSL stack should return
 *                  immediately without calling the resume callback yet.
 * \return          #MBEDTLS_ERR_SSL_HW_ACCEL_FALLTHROUGH if the external
 *                  processor does not support this key. The SSL stack will
 *                  use the private key object instead.
 * \return          Any other error indicates a fatal failure and is
 *                  propagated up the call chain. The callback should
 *                  use \c MBEDTLS_ERR_PK_xxx error codes, and <b>must not</b>
 *                  use \c MBEDTLS_ERR_SSL_xxx error codes except as
 *                  directed in the documentation of this callback.
 */
typedef int mbedtls_ssl_async_decrypt_t(mbedtls_ssl_context *ssl,
                                        mbedtls_x509_crt *cert,
                                        const unsigned char *input,
                                        size_t input_len);
#endif /* MBEDTLS_X509_CRT_PARSE_C */

/**
 * \brief           Callback type: resume external operation.
 *
 *                  This callback is called during an SSL handshake to resume
 *                  an external operation started by the
 *                  ::mbedtls_ssl_async_sign_t or
 *                  ::mbedtls_ssl_async_decrypt_t callback.
 *
 *                  This function typically checks the status of a pending
 *                  request or causes the request queue to make progress, and
 *                  does not wait for the operation to complete. This allows
 *                  the handshake step to be non-blocking.
 *
 *                  This function may call mbedtls_ssl_get_async_operation_data()
 *                  to retrieve an operation context set by the start callback.
 *                  It may call mbedtls_ssl_set_async_operation_data() to modify
 *                  this context.
 *
 *                  Note that when this function returns a status other than
 *                  #MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS, it must free any
 *                  resources associated with the operation.
 *
 * \param ssl             The SSL connection instance. It should not be
 *                        modified other than via
 *                        mbedtls_ssl_set_async_operation_data().
 * \param output          Buffer containing the output (signature or decrypted
 *                        data) on success.
 * \param output_len      On success, number of bytes written to \p output.
 * \param output_size     Size of the \p output buffer in bytes.
 *
 * \return          0 if output of the operation is available in the
 *                  \p output buffer.
 * \return          #MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS if the operation
 *                  is still in progress. Subsequent requests for progress
 *                  on the SSL connection will call the resume callback
 *                  again.
 * \return          Any other error means that the operation is aborted.
 *                  The SSL handshake is aborted. The callback should
 *                  use \c MBEDTLS_ERR_PK_xxx error codes, and <b>must not</b>
 *                  use \c MBEDTLS_ERR_SSL_xxx error codes except as
 *                  directed in the documentation of this callback.
 */
typedef int
mbedtls_ssl_async_resume_t(mbedtls_ssl_context *ssl, unsigned char *output, size_t *output_len, size_t output_size);

/**
 * \brief           Callback type: cancel external operation.
 *
 *                  This callback is called if an SSL connection is closed
 *                  while an asynchronous operation is in progress. Note that
 *                  this callback is not called if the
 *                  ::mbedtls_ssl_async_resume_t callback has run and has
 *                  returned a value other than
 *                  #MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS, since in that case
 *                  the asynchronous operation has already completed.
 *
 *                  This function may call mbedtls_ssl_get_async_operation_data()
 *                  to retrieve an operation context set by the start callback.
 *
 * \param ssl             The SSL connection instance. It should not be
 *                        modified.
 */
typedef void mbedtls_ssl_async_cancel_t(mbedtls_ssl_context *ssl);
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */

#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED) && !defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define MBEDTLS_SSL_PEER_CERT_DIGEST_MAX_LEN 48
#if defined(MBEDTLS_SHA256_C)
#define MBEDTLS_SSL_PEER_CERT_DIGEST_DFL_TYPE MBEDTLS_MD_SHA256
#define MBEDTLS_SSL_PEER_CERT_DIGEST_DFL_LEN  32
#elif defined(MBEDTLS_SHA384_C)
#define MBEDTLS_SSL_PEER_CERT_DIGEST_DFL_TYPE MBEDTLS_MD_SHA384
#define MBEDTLS_SSL_PEER_CERT_DIGEST_DFL_LEN  48
#elif defined(MBEDTLS_SHA1_C)
#define MBEDTLS_SSL_PEER_CERT_DIGEST_DFL_TYPE MBEDTLS_MD_SHA1
#define MBEDTLS_SSL_PEER_CERT_DIGEST_DFL_LEN  20
#else
/* This is already checked in check_config.h, but be sure. */
#error "Bad configuration - need SHA-1, SHA-256 or SHA-512 enabled to compute digest of peer CRT."
#endif
#endif /* MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED && \
          !MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */

typedef struct {
    unsigned char client_application_traffic_secret_N[MBEDTLS_TLS1_3_MD_MAX_SIZE];
    unsigned char server_application_traffic_secret_N[MBEDTLS_TLS1_3_MD_MAX_SIZE];
    unsigned char exporter_master_secret[MBEDTLS_TLS1_3_MD_MAX_SIZE];
    unsigned char resumption_master_secret[MBEDTLS_TLS1_3_MD_MAX_SIZE];
} mbedtls_ssl_tls13_application_secrets;

#if defined(MBEDTLS_SSL_DTLS_SRTP)

#define MBEDTLS_TLS_SRTP_MAX_MKI_LENGTH          255
#define MBEDTLS_TLS_SRTP_MAX_PROFILE_LIST_LENGTH 4
/*
 * For code readability use a typedef for DTLS-SRTP profiles
 *
 * Use_srtp extension protection profiles values as defined in
 * http://www.iana.org/assignments/srtp-protection/srtp-protection.xhtml
 *
 * Reminder: if this list is expanded mbedtls_ssl_check_srtp_profile_value
 * must be updated too.
 */
#define MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80 ((uint16_t)0x0001)
#define MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32 ((uint16_t)0x0002)
#define MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_80      ((uint16_t)0x0005)
#define MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_32      ((uint16_t)0x0006)
/* This one is not iana defined, but for code readability. */
#define MBEDTLS_TLS_SRTP_UNSET ((uint16_t)0x0000)

typedef uint16_t mbedtls_ssl_srtp_profile;

typedef struct mbedtls_dtls_srtp_info_t {
    /*! The SRTP profile that was negotiated. */
    mbedtls_ssl_srtp_profile MBEDTLS_PRIVATE(chosen_dtls_srtp_profile);
    /*! The length of mki_value. */
    uint16_t MBEDTLS_PRIVATE(mki_len);
    /*! The mki_value used, with max size of 256 bytes. */
    unsigned char MBEDTLS_PRIVATE(mki_value)[MBEDTLS_TLS_SRTP_MAX_MKI_LENGTH];
} mbedtls_dtls_srtp_info;

#endif /* MBEDTLS_SSL_DTLS_SRTP */

/** Human-friendly representation of the (D)TLS protocol version. */
typedef enum {
    MBEDTLS_SSL_VERSION_UNKNOWN,         /*!< Context not in use or version not yet negotiated. */
    MBEDTLS_SSL_VERSION_TLS1_2 = 0x0303, /*!< (D)TLS 1.2 */
    MBEDTLS_SSL_VERSION_TLS1_3 = 0x0304, /*!< (D)TLS 1.3 */
} mbedtls_ssl_protocol_version;

/*
 * This structure is used for storing current session data.
 *
 * Note: when changing this definition, we need to check and update:
 *  - in tests/suites/test_suite_ssl.function:
 *      ssl_populate_session() and ssl_serialize_session_save_load()
 *  - in library/ssl_tls.c:
 *      mbedtls_ssl_session_init() and mbedtls_ssl_session_free()
 *      mbedtls_ssl_session_save() and ssl_session_load()
 *      ssl_session_copy()
 */
struct mbedtls_ssl_session {
#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    unsigned char MBEDTLS_PRIVATE(mfl_code); /*!< MaxFragmentLength negotiated by peer */
#endif                                       /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */

    unsigned char MBEDTLS_PRIVATE(exported);

    /** TLS version negotiated in the session. Used if and when renegotiating
     *  or resuming a session instead of the configured minor TLS version.
     */
    mbedtls_ssl_protocol_version MBEDTLS_PRIVATE(tls_version);

#if defined(MBEDTLS_HAVE_TIME)
    mbedtls_time_t MBEDTLS_PRIVATE(start); /*!< starting time      */
#endif
    int MBEDTLS_PRIVATE(ciphersuite);          /*!< chosen ciphersuite */
    size_t MBEDTLS_PRIVATE(id_len);            /*!< session id length  */
    unsigned char MBEDTLS_PRIVATE(id)[32];     /*!< session identifier */
    unsigned char MBEDTLS_PRIVATE(master)[48]; /*!< the master secret  */

#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
    mbedtls_x509_crt *MBEDTLS_PRIVATE(peer_cert); /*!< peer X.509 cert chain */
#else                                             /* MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */
    /*! The digest of the peer's end-CRT. This must be kept to detect CRT
     *  changes during renegotiation, mitigating the triple handshake attack. */
    unsigned char *MBEDTLS_PRIVATE(peer_cert_digest);
    size_t MBEDTLS_PRIVATE(peer_cert_digest_len);
    mbedtls_md_type_t MBEDTLS_PRIVATE(peer_cert_digest_type);
#endif                                            /* !MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */
#endif                                            /* MBEDTLS_X509_CRT_PARSE_C */
    uint32_t MBEDTLS_PRIVATE(verify_result);      /*!<  verification result     */

#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_CLI_C)
    unsigned char *MBEDTLS_PRIVATE(ticket);    /*!< RFC 5077 session ticket */
    size_t MBEDTLS_PRIVATE(ticket_len);        /*!< session ticket length   */
    uint32_t MBEDTLS_PRIVATE(ticket_lifetime); /*!< ticket lifetime hint    */
#endif                                         /* MBEDTLS_SSL_SESSION_TICKETS && MBEDTLS_SSL_CLI_C */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(MBEDTLS_SSL_SESSION_TICKETS)
    uint8_t MBEDTLS_PRIVATE(endpoint);           /*!< 0: client, 1: server */
    uint8_t MBEDTLS_PRIVATE(ticket_flags);       /*!< Ticket flags */
    uint32_t MBEDTLS_PRIVATE(ticket_age_add);    /*!< Randomly generated value used to obscure the age of the ticket */
    uint8_t MBEDTLS_PRIVATE(resumption_key_len); /*!< resumption_key length */
    unsigned char MBEDTLS_PRIVATE(resumption_key)[MBEDTLS_SSL_TLS1_3_TICKET_RESUMPTION_KEY_LEN];

#if defined(MBEDTLS_HAVE_TIME) && defined(MBEDTLS_SSL_CLI_C)
    mbedtls_time_t MBEDTLS_PRIVATE(ticket_received); /*!< time ticket was received */
#endif                                               /* MBEDTLS_HAVE_TIME && MBEDTLS_SSL_CLI_C */

#endif /*  MBEDTLS_SSL_PROTO_TLS1_3 && MBEDTLS_SSL_SESSION_TICKETS */

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    int MBEDTLS_PRIVATE(encrypt_then_mac); /*!< flag for EtM activation                */
#endif

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    mbedtls_ssl_tls13_application_secrets MBEDTLS_PRIVATE(app_secrets);
#endif
};

/*
 * Identifiers for PRFs used in various versions of TLS.
 */
typedef enum {
    MBEDTLS_SSL_TLS_PRF_NONE,
    MBEDTLS_SSL_TLS_PRF_SHA384,
    MBEDTLS_SSL_TLS_PRF_SHA256,
    MBEDTLS_SSL_HKDF_EXPAND_SHA384,
    MBEDTLS_SSL_HKDF_EXPAND_SHA256
} mbedtls_tls_prf_types;

typedef enum {
    MBEDTLS_SSL_KEY_EXPORT_TLS12_MASTER_SECRET = 0,
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    MBEDTLS_SSL_KEY_EXPORT_TLS1_3_CLIENT_EARLY_SECRET,
    MBEDTLS_SSL_KEY_EXPORT_TLS1_3_EARLY_EXPORTER_SECRET,
    MBEDTLS_SSL_KEY_EXPORT_TLS1_3_CLIENT_HANDSHAKE_TRAFFIC_SECRET,
    MBEDTLS_SSL_KEY_EXPORT_TLS1_3_SERVER_HANDSHAKE_TRAFFIC_SECRET,
    MBEDTLS_SSL_KEY_EXPORT_TLS1_3_CLIENT_APPLICATION_TRAFFIC_SECRET,
    MBEDTLS_SSL_KEY_EXPORT_TLS1_3_SERVER_APPLICATION_TRAFFIC_SECRET,
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */
} mbedtls_ssl_key_export_type;

/**
 * \brief           Callback type: Export key alongside random values for
 *                                 session identification, and PRF for
 *                                 implementation of TLS key exporters.
 *
 * \param p_expkey   Context for the callback.
 * \param type       The type of the key that is being exported.
 * \param secret     The address of the buffer holding the secret
 *                   that's being exporterd.
 * \param secret_len The length of \p secret in bytes.
 * \param client_random The client random bytes.
 * \param server_random The server random bytes.
 * \param tls_prf_type The identifier for the PRF used in the handshake
 *                     to which the key belongs.
 */
typedef void mbedtls_ssl_export_keys_t(void *p_expkey,
                                       mbedtls_ssl_key_export_type type,
                                       const unsigned char *secret,
                                       size_t secret_len,
                                       const unsigned char client_random[32],
                                       const unsigned char server_random[32],
                                       mbedtls_tls_prf_types tls_prf_type);

#if defined(MBEDTLS_SSL_SRV_C)
/**
 * \brief           Callback type: generic handshake callback
 *
 * \note            Callbacks may use user_data funcs to set/get app user data.
 *                  See \c mbedtls_ssl_get_user_data_p()
 *                      \c mbedtls_ssl_get_user_data_n()
 *                      \c mbedtls_ssl_conf_get_user_data_p()
 *                      \c mbedtls_ssl_conf_get_user_data_n()
 *
 * \param ssl       \c mbedtls_ssl_context on which the callback is run
 *
 * \return          The return value of the callback is 0 if successful,
 *                  or a specific MBEDTLS_ERR_XXX code, which will cause
 *                  the handshake to be aborted.
 */
typedef int (*mbedtls_ssl_hs_cb_t)(mbedtls_ssl_context *ssl);
#endif

/* A type for storing user data in a library structure.
 *
 * The representation of type may change in future versions of the library.
 * Only the behaviors guaranteed by documented accessor functions are
 * guaranteed to remain stable.
 */
typedef union {
    uintptr_t n; /* typically a handle to an associated object */
    void *p;     /* typically a pointer to extra data */
} mbedtls_ssl_user_data_t;

/**
 * SSL/TLS configuration to be shared between mbedtls_ssl_context structures.
 */
struct mbedtls_ssl_config {
    /* Group items mostly by size. This helps to reduce memory wasted to
     * padding. It also helps to keep smaller fields early in the structure,
     * so that elements tend to be in the 128-element direct access window
     * on Arm Thumb, which reduces the code size. */

    mbedtls_ssl_protocol_version MBEDTLS_PRIVATE(max_tls_version); /*!< max. TLS version used */
    mbedtls_ssl_protocol_version MBEDTLS_PRIVATE(min_tls_version); /*!< min. TLS version used */

    /*
     * Flags (could be bit-fields to save RAM, but separate bytes make
     * the code smaller on architectures with an instruction for direct
     * byte access).
     */

    uint8_t MBEDTLS_PRIVATE(endpoint);  /*!< 0: client, 1: server               */
    uint8_t MBEDTLS_PRIVATE(transport); /*!< 0: stream (TLS), 1: datagram (DTLS)    */
    uint8_t MBEDTLS_PRIVATE(authmode);  /*!< MBEDTLS_SSL_VERIFY_XXX             */
    /* needed even with renego disabled for LEGACY_BREAK_HANDSHAKE          */
    uint8_t MBEDTLS_PRIVATE(allow_legacy_renegotiation); /*!< MBEDTLS_LEGACY_XXX   */
#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    uint8_t MBEDTLS_PRIVATE(mfl_code); /*!< desired fragment length indicator
                                            (MBEDTLS_SSL_MAX_FRAG_LEN_XXX) */
#endif
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    uint8_t MBEDTLS_PRIVATE(encrypt_then_mac); /*!< negotiate encrypt-then-mac?    */
#endif
#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
    uint8_t MBEDTLS_PRIVATE(extended_ms); /*!< negotiate extended master secret?  */
#endif
#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
    uint8_t MBEDTLS_PRIVATE(anti_replay); /*!< detect and prevent replay?         */
#endif
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    uint8_t MBEDTLS_PRIVATE(disable_renegotiation); /*!< disable renegotiation?     */
#endif
#if defined(MBEDTLS_SSL_SESSION_TICKETS)
    uint8_t MBEDTLS_PRIVATE(session_tickets); /*!< use session tickets?           */
#endif
#if defined(MBEDTLS_SSL_SRV_C)
    uint8_t MBEDTLS_PRIVATE(cert_req_ca_list); /*!< enable sending CA list in
                                                    Certificate Request messages? */
    uint8_t MBEDTLS_PRIVATE(respect_cli_pref); /*!< pick the ciphersuite according to
                                                    the client's preferences rather
                                                    than ours? */
#endif
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    uint8_t MBEDTLS_PRIVATE(ignore_unexpected_cid); /*!< Should DTLS record with
                                                     *   unexpected CID
                                                     *   lead to failure? */
#endif                                              /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
#if defined(MBEDTLS_SSL_DTLS_SRTP)
    uint8_t MBEDTLS_PRIVATE(dtls_srtp_mki_support); /* support having mki_value
                                                       in the use_srtp extension? */
#endif

    /*
     * Pointers
     */

    /** Allowed ciphersuites for (D)TLS 1.2 (0-terminated)                  */
    const int *MBEDTLS_PRIVATE(ciphersuite_list);

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    /** Allowed TLS 1.3 key exchange modes.                                 */
    int MBEDTLS_PRIVATE(tls13_kex_modes);
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

    /** Callback for printing debug output                                  */
    void (*MBEDTLS_PRIVATE(f_dbg))(void *, int, const char *, int, const char *);
    void *MBEDTLS_PRIVATE(p_dbg); /*!< context for the debug function     */

    /** Callback for getting (pseudo-)random numbers                        */
    int (*MBEDTLS_PRIVATE(f_rng))(void *, unsigned char *, size_t);
    void *MBEDTLS_PRIVATE(p_rng); /*!< context for the RNG function       */

    /** Callback to retrieve a session from the cache                       */
    mbedtls_ssl_cache_get_t *MBEDTLS_PRIVATE(f_get_cache);
    /** Callback to store a session into the cache                          */
    mbedtls_ssl_cache_set_t *MBEDTLS_PRIVATE(f_set_cache);
    void *MBEDTLS_PRIVATE(p_cache); /*!< context for cache callbacks        */

#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
    /** Callback for setting cert according to SNI extension                */
    int (*MBEDTLS_PRIVATE(f_sni))(void *, mbedtls_ssl_context *, const unsigned char *, size_t);
    void *MBEDTLS_PRIVATE(p_sni); /*!< context for SNI callback           */
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    /** Callback to customize X.509 certificate chain verification          */
    int (*MBEDTLS_PRIVATE(f_vrfy))(void *, mbedtls_x509_crt *, int, uint32_t *);
    void *MBEDTLS_PRIVATE(p_vrfy); /*!< context for X.509 verify calllback */
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
    /** Callback to retrieve PSK key from identity                          */
    int (*MBEDTLS_PRIVATE(f_psk))(void *, mbedtls_ssl_context *, const unsigned char *, size_t);
    void *MBEDTLS_PRIVATE(p_psk); /*!< context for PSK callback           */
#endif

#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY) && defined(MBEDTLS_SSL_SRV_C)
    /** Callback to create & write a cookie for ClientHello verification    */
    int (*MBEDTLS_PRIVATE(f_cookie_write))(void *, unsigned char **, unsigned char *, const unsigned char *, size_t);
    /** Callback to verify validity of a ClientHello cookie                 */
    int (*MBEDTLS_PRIVATE(f_cookie_check))(void *, const unsigned char *, size_t, const unsigned char *, size_t);
    void *MBEDTLS_PRIVATE(p_cookie); /*!< context for the cookie callbacks   */
#endif

#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_SRV_C)
    /** Callback to create & write a session ticket                         */
    int (*MBEDTLS_PRIVATE(f_ticket_write))(
        void *, const mbedtls_ssl_session *, unsigned char *, const unsigned char *, size_t *, uint32_t *);
    /** Callback to parse a session ticket into a session structure         */
    int (*MBEDTLS_PRIVATE(f_ticket_parse))(void *, mbedtls_ssl_session *, unsigned char *, size_t);
    void *MBEDTLS_PRIVATE(p_ticket); /*!< context for the ticket callbacks   */
#endif                               /* MBEDTLS_SSL_SESSION_TICKETS && MBEDTLS_SSL_SRV_C */
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    size_t MBEDTLS_PRIVATE(cid_len); /*!< The length of CIDs for incoming DTLS records.      */
#endif                               /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    const mbedtls_x509_crt_profile *MBEDTLS_PRIVATE(cert_profile); /*!< verification profile */
    mbedtls_ssl_key_cert *MBEDTLS_PRIVATE(key_cert);               /*!< own certificate/key pair(s)        */
    mbedtls_x509_crt *MBEDTLS_PRIVATE(ca_chain);                   /*!< trusted CAs                        */
    mbedtls_x509_crl *MBEDTLS_PRIVATE(ca_crl);                     /*!< trusted CAs CRLs                   */
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    mbedtls_x509_crt_ca_cb_t MBEDTLS_PRIVATE(f_ca_cb);
    void *MBEDTLS_PRIVATE(p_ca_cb);
#endif /* MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK */
#endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_async_sign_t *MBEDTLS_PRIVATE(f_async_sign_start);       /*!< start asynchronous signature operation */
    mbedtls_ssl_async_decrypt_t *MBEDTLS_PRIVATE(f_async_decrypt_start); /*!< start asynchronous decryption operation */
#endif                                                                   /* MBEDTLS_X509_CRT_PARSE_C */
    mbedtls_ssl_async_resume_t *MBEDTLS_PRIVATE(f_async_resume);         /*!< resume asynchronous operation */
    mbedtls_ssl_async_cancel_t *MBEDTLS_PRIVATE(f_async_cancel);         /*!< cancel asynchronous operation */
    void *MBEDTLS_PRIVATE(p_async_config_data); /*!< Configuration data set by mbedtls_ssl_conf_async_private_cb(). */
#endif                                          /* MBEDTLS_SSL_ASYNC_PRIVATE */

#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
    const int *MBEDTLS_PRIVATE(sig_hashes); /*!< allowed signature hashes           */
#endif
    const uint16_t *MBEDTLS_PRIVATE(sig_algs); /*!< allowed signature algorithms       */
#endif

#if defined(MBEDTLS_ECP_C) && !defined(MBEDTLS_DEPRECATED_REMOVED)
    const mbedtls_ecp_group_id *MBEDTLS_PRIVATE(curve_list); /*!< allowed curves             */
#endif

    const uint16_t *MBEDTLS_PRIVATE(group_list); /*!< allowed IANA NamedGroups */

#if defined(MBEDTLS_DHM_C)
    mbedtls_mpi MBEDTLS_PRIVATE(dhm_P); /*!< prime modulus for DHM              */
    mbedtls_mpi MBEDTLS_PRIVATE(dhm_G); /*!< generator for DHM                  */
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    mbedtls_svc_key_id_t MBEDTLS_PRIVATE(psk_opaque); /*!< PSA key slot holding opaque PSK. This field
                                                       *   should only be set via
                                                       *   mbedtls_ssl_conf_psk_opaque().
                                                       *   If either no PSK or a raw PSK have been
                                                       *   configured, this has value \c 0.
                                                       */
#endif                                                /* MBEDTLS_USE_PSA_CRYPTO */
    unsigned char *MBEDTLS_PRIVATE(psk);              /*!< The raw pre-shared key. This field should
                                                       *   only be set via mbedtls_ssl_conf_psk().
                                                       *   If either no PSK or an opaque PSK
                                                       *   have been configured, this has value NULL. */
    size_t MBEDTLS_PRIVATE(psk_len);                  /*!< The length of the raw pre-shared key.
                                                       *   This field should only be set via
                                                       *   mbedtls_ssl_conf_psk().
                                                       *   Its value is non-zero if and only if
                                                       *   \c psk is not \c NULL. */

    unsigned char *MBEDTLS_PRIVATE(psk_identity); /*!< The PSK identity for PSK negotiation.
                                                   *   This field should only be set via
                                                   *   mbedtls_ssl_conf_psk().
                                                   *   This is set if and only if either
                                                   *   \c psk or \c psk_opaque are set. */
    size_t MBEDTLS_PRIVATE(psk_identity_len);     /*!< The length of PSK identity.
                                                   *   This field should only be set via
                                                   *   mbedtls_ssl_conf_psk().
                                                   *   Its value is non-zero if and only if
                                                   *   \c psk is not \c NULL or \c psk_opaque
                                                   *   is not \c 0. */
#endif                                            /* MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED */

#if defined(MBEDTLS_SSL_ALPN)
    const char **MBEDTLS_PRIVATE(alpn_list); /*!< ordered list of protocols          */
#endif

#if defined(MBEDTLS_SSL_DTLS_SRTP)
    /*! ordered list of supported srtp profile */
    const mbedtls_ssl_srtp_profile *MBEDTLS_PRIVATE(dtls_srtp_profile_list);
    /*! number of supported profiles */
    size_t MBEDTLS_PRIVATE(dtls_srtp_profile_list_len);
#endif /* MBEDTLS_SSL_DTLS_SRTP */

    /*
     * Numerical settings (int)
     */

    uint32_t MBEDTLS_PRIVATE(read_timeout); /*!< timeout for mbedtls_ssl_read (ms)  */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    uint32_t MBEDTLS_PRIVATE(hs_timeout_min); /*!< initial value of the handshake
                                  retransmission timeout (ms)        */
    uint32_t MBEDTLS_PRIVATE(hs_timeout_max); /*!< maximum value of the handshake
                                  retransmission timeout (ms)        */
#endif

#if defined(MBEDTLS_SSL_RENEGOTIATION)
    int MBEDTLS_PRIVATE(renego_max_records);         /*!< grace period for renegotiation     */
    unsigned char MBEDTLS_PRIVATE(renego_period)[8]; /*!< value of the record counters
                                         that triggers renegotiation        */
#endif

    unsigned int MBEDTLS_PRIVATE(badmac_limit); /*!< limit of records with a bad MAC    */

#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_SSL_CLI_C)
    unsigned int MBEDTLS_PRIVATE(dhm_min_bitlen); /*!< min. bit length of the DHM prime   */
#endif

    /** User data pointer or handle.
     *
     * The library sets this to \p 0 when creating a context and does not
     * access it afterwards.
     */
    mbedtls_ssl_user_data_t MBEDTLS_PRIVATE(user_data);

#if defined(MBEDTLS_SSL_SRV_C)
    mbedtls_ssl_hs_cb_t MBEDTLS_PRIVATE(f_cert_cb); /*!< certificate selection callback */
#endif                                              /* MBEDTLS_SSL_SRV_C */

#if defined(MBEDTLS_KEY_EXCHANGE_CERT_REQ_ALLOWED_ENABLED)
    const mbedtls_x509_crt *MBEDTLS_PRIVATE(dn_hints); /*!< acceptable client cert issuers    */
#endif
};

struct mbedtls_ssl_context {
    const mbedtls_ssl_config *MBEDTLS_PRIVATE(conf); /*!< configuration information          */

    /*
     * Miscellaneous
     */
    int MBEDTLS_PRIVATE(state); /*!< SSL handshake: current state     */
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    int MBEDTLS_PRIVATE(renego_status);       /*!< Initial, in progress, pending?   */
    int MBEDTLS_PRIVATE(renego_records_seen); /*!< Records since renego request, or with DTLS,
                               number of retransmissions of request if
                               renego_max_records is < 0           */
#endif                                        /* MBEDTLS_SSL_RENEGOTIATION */

    /** Server: Negotiated TLS protocol version.
     *  Client: Maximum TLS version to be negotiated, then negotiated TLS
     *          version.
     *
     *  It is initialized as the maximum TLS version to be negotiated in the
     *  ClientHello writing preparation stage and used throughout the
     *  ClientHello writing. For a fresh handshake not linked to any previous
     *  handshake, it is initialized to the configured maximum TLS version
     *  to be negotiated. When renegotiating or resuming a session, it is
     *  initialized to the previously negotiated TLS version.
     *
     *  Updated to the negotiated TLS version as soon as the ServerHello is
     *  received.
     */
    mbedtls_ssl_protocol_version MBEDTLS_PRIVATE(tls_version);

    unsigned MBEDTLS_PRIVATE(badmac_seen); /*!< records with a bad MAC received    */

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    /** Callback to customize X.509 certificate chain verification          */
    int (*MBEDTLS_PRIVATE(f_vrfy))(void *, mbedtls_x509_crt *, int, uint32_t *);
    void *MBEDTLS_PRIVATE(p_vrfy); /*!< context for X.509 verify callback */
#endif

    mbedtls_ssl_send_t *MBEDTLS_PRIVATE(f_send); /*!< Callback for network send */
    mbedtls_ssl_recv_t *MBEDTLS_PRIVATE(f_recv); /*!< Callback for network receive */
    mbedtls_ssl_recv_timeout_t *MBEDTLS_PRIVATE(f_recv_timeout);
    /*!< Callback for network receive with timeout */

    void *MBEDTLS_PRIVATE(p_bio); /*!< context for I/O operations   */

    /*
     * Session layer
     */
    mbedtls_ssl_session *MBEDTLS_PRIVATE(session_in);        /*!<  current session data (in)   */
    mbedtls_ssl_session *MBEDTLS_PRIVATE(session_out);       /*!<  current session data (out)  */
    mbedtls_ssl_session *MBEDTLS_PRIVATE(session);           /*!<  negotiated session data     */
    mbedtls_ssl_session *MBEDTLS_PRIVATE(session_negotiate); /*!<  session data in negotiation */

    mbedtls_ssl_handshake_params *MBEDTLS_PRIVATE(handshake); /*!<  params required only during
                                           the handshake process        */

    /*
     * Record layer transformations
     */
    mbedtls_ssl_transform *MBEDTLS_PRIVATE(transform_in);        /*!<  current transform params (in)
                                                                  *    This is always a reference,
                                                                  *    never an owning pointer.        */
    mbedtls_ssl_transform *MBEDTLS_PRIVATE(transform_out);       /*!<  current transform params (out)
                                                                  *    This is always a reference,
                                                                  *    never an owning pointer.        */
    mbedtls_ssl_transform *MBEDTLS_PRIVATE(transform);           /*!<  negotiated transform params
                                                                  *    This pointer owns the transform
                                                                  *    it references.                  */
    mbedtls_ssl_transform *MBEDTLS_PRIVATE(transform_negotiate); /*!<  transform params in negotiation
                                                                  *    This pointer owns the transform
                                                                  *    it references.                  */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    /*! The application data transform in TLS 1.3.
     *  This pointer owns the transform it references. */
    mbedtls_ssl_transform *MBEDTLS_PRIVATE(transform_application);
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

    /*
     * Timers
     */
    void *MBEDTLS_PRIVATE(p_timer); /*!< context for the timer callbacks */

    mbedtls_ssl_set_timer_t *MBEDTLS_PRIVATE(f_set_timer); /*!< set timer callback */
    mbedtls_ssl_get_timer_t *MBEDTLS_PRIVATE(f_get_timer); /*!< get timer callback */

    /*
     * Record layer (incoming data)
     */
    unsigned char *MBEDTLS_PRIVATE(in_buf); /*!< input buffer                     */
    unsigned char *MBEDTLS_PRIVATE(in_ctr); /*!< 64-bit incoming message counter
                                TLS: maintained by us
                                DTLS: read from peer             */
    unsigned char *MBEDTLS_PRIVATE(in_hdr); /*!< start of record header           */
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    unsigned char *MBEDTLS_PRIVATE(in_cid);  /*!< The start of the CID;
                                              *   (the end is marked by in_len).   */
#endif                                       /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
    unsigned char *MBEDTLS_PRIVATE(in_len);  /*!< two-bytes message length field   */
    unsigned char *MBEDTLS_PRIVATE(in_iv);   /*!< ivlen-byte IV                    */
    unsigned char *MBEDTLS_PRIVATE(in_msg);  /*!< message contents (in_iv+ivlen)   */
    unsigned char *MBEDTLS_PRIVATE(in_offt); /*!< read offset in application data  */

    int MBEDTLS_PRIVATE(in_msgtype);   /*!< record header: message type      */
    size_t MBEDTLS_PRIVATE(in_msglen); /*!< record header: message length    */
    size_t MBEDTLS_PRIVATE(in_left);   /*!< amount of data read so far       */
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
    size_t MBEDTLS_PRIVATE(in_buf_len); /*!< length of input buffer           */
#endif
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    uint16_t MBEDTLS_PRIVATE(in_epoch);         /*!< DTLS epoch for incoming records  */
    size_t MBEDTLS_PRIVATE(next_record_offset); /*!< offset of the next record in datagram
                                    (equal to in_left if none)       */
#endif                                          /* MBEDTLS_SSL_PROTO_DTLS */
#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
    uint64_t MBEDTLS_PRIVATE(in_window_top); /*!< last validated record seq_num    */
    uint64_t MBEDTLS_PRIVATE(in_window);     /*!< bitmask for replay detection     */
#endif                                       /* MBEDTLS_SSL_DTLS_ANTI_REPLAY */

    size_t MBEDTLS_PRIVATE(in_hslen); /*!< current handshake message length,
                          including the handshake header   */
    int MBEDTLS_PRIVATE(nb_zero);     /*!< # of 0-length encrypted messages */

    int MBEDTLS_PRIVATE(keep_current_message); /*!< drop or reuse current message
                                   on next call to record layer? */

    /* The following three variables indicate if and, if yes,
     * what kind of alert is pending to be sent.
     */
    unsigned char MBEDTLS_PRIVATE(send_alert); /*!< Determines if a fatal alert
                                              should be sent. Values:
                                              - \c 0 , no alert is to be sent.
                                              - \c 1 , alert is to be sent. */
    unsigned char MBEDTLS_PRIVATE(alert_type); /*!< Type of alert if send_alert
                                               != 0 */
    int MBEDTLS_PRIVATE(alert_reason);         /*!< The error code to be returned
                                               to the user once the fatal alert
                                               has been sent. */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    uint8_t MBEDTLS_PRIVATE(disable_datagram_packing); /*!< Disable packing multiple records
                                                        *   within a single datagram.  */
#endif                                                 /* MBEDTLS_SSL_PROTO_DTLS */

    /*
     * Record layer (outgoing data)
     */
    unsigned char *MBEDTLS_PRIVATE(out_buf); /*!< output buffer                    */
    unsigned char *MBEDTLS_PRIVATE(out_ctr); /*!< 64-bit outgoing message counter  */
    unsigned char *MBEDTLS_PRIVATE(out_hdr); /*!< start of record header           */
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    unsigned char *MBEDTLS_PRIVATE(out_cid); /*!< The start of the CID;
                                              *   (the end is marked by in_len).   */
#endif                                       /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
    unsigned char *MBEDTLS_PRIVATE(out_len); /*!< two-bytes message length field   */
    unsigned char *MBEDTLS_PRIVATE(out_iv);  /*!< ivlen-byte IV                    */
    unsigned char *MBEDTLS_PRIVATE(out_msg); /*!< message contents (out_iv+ivlen)  */

    int MBEDTLS_PRIVATE(out_msgtype);   /*!< record header: message type      */
    size_t MBEDTLS_PRIVATE(out_msglen); /*!< record header: message length    */
    size_t MBEDTLS_PRIVATE(out_left);   /*!< amount of data not yet written   */
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
    size_t MBEDTLS_PRIVATE(out_buf_len); /*!< length of output buffer          */
#endif

    unsigned char
        MBEDTLS_PRIVATE(cur_out_ctr)[MBEDTLS_SSL_SEQUENCE_NUMBER_LEN]; /*!<  Outgoing record sequence  number. */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    uint16_t MBEDTLS_PRIVATE(mtu); /*!< path mtu, used to fragment outgoing messages */
#endif                             /* MBEDTLS_SSL_PROTO_DTLS */

    /*
     * User settings
     */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    char *hostname; /*!< expected peer CN for verification
                        (and SNI if available)                 */
#endif              /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_SSL_ALPN)
    const char *MBEDTLS_PRIVATE(alpn_chosen); /*!<  negotiated protocol                   */
#endif                                        /* MBEDTLS_SSL_ALPN */

#if defined(MBEDTLS_SSL_DTLS_SRTP)
    /*
     * use_srtp extension
     */
    mbedtls_dtls_srtp_info MBEDTLS_PRIVATE(dtls_srtp_info);
#endif /* MBEDTLS_SSL_DTLS_SRTP */

    /*
     * Information for DTLS hello verify
     */
#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY) && defined(MBEDTLS_SSL_SRV_C)
    unsigned char *MBEDTLS_PRIVATE(cli_id); /*!<  transport-level ID of the client  */
    size_t MBEDTLS_PRIVATE(cli_id_len);     /*!<  length of cli_id                  */
#endif                                      /* MBEDTLS_SSL_DTLS_HELLO_VERIFY && MBEDTLS_SSL_SRV_C */

    /*
     * Secure renegotiation
     */
    /* needed to know when to send extension on server */
    int MBEDTLS_PRIVATE(secure_renegotiation); /*!<  does peer support legacy or
                                    secure renegotiation           */
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    size_t MBEDTLS_PRIVATE(verify_data_len);                                 /*!<  length of verify data stored   */
    char MBEDTLS_PRIVATE(own_verify_data)[MBEDTLS_SSL_VERIFY_DATA_MAX_LEN];  /*!<  previous handshake verify data */
    char MBEDTLS_PRIVATE(peer_verify_data)[MBEDTLS_SSL_VERIFY_DATA_MAX_LEN]; /*!<  previous handshake verify data */
#endif                                                                       /* MBEDTLS_SSL_RENEGOTIATION */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    /* CID configuration to use in subsequent handshakes. */

    /*! The next incoming CID, chosen by the user and applying to
     *  all subsequent handshakes. This may be different from the
     *  CID currently used in case the user has re-configured the CID
     *  after an initial handshake. */
    unsigned char MBEDTLS_PRIVATE(own_cid)[MBEDTLS_SSL_CID_IN_LEN_MAX];
    uint8_t MBEDTLS_PRIVATE(own_cid_len);   /*!< The length of \c own_cid. */
    uint8_t MBEDTLS_PRIVATE(negotiate_cid); /*!< This indicates whether the CID extension should
                                             *   be negotiated in the next handshake or not.
                                             *   Possible values are #MBEDTLS_SSL_CID_ENABLED
                                             *   and #MBEDTLS_SSL_CID_DISABLED. */
#endif                                      /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

    /** Callback to export key block and master secret                      */
    mbedtls_ssl_export_keys_t *MBEDTLS_PRIVATE(f_export_keys);
    void *MBEDTLS_PRIVATE(p_export_keys); /*!< context for key export callback    */

    /** User data pointer or handle.
     *
     * The library sets this to \p 0 when creating a context and does not
     * access it afterwards.
     *
     * \warning Serializing and restoring an SSL context with
     *          mbedtls_ssl_context_save() and mbedtls_ssl_context_load()
     *          does not currently restore the user data.
     */
    mbedtls_ssl_user_data_t MBEDTLS_PRIVATE(user_data);
};

typedef int (*f_rng)(void *, unsigned char *, size_t);

DECLARE_FAKE_VALUE_FUNC(int, mbedtls_ssl_setup, mbedtls_ssl_context *, const mbedtls_ssl_config *);
DECLARE_FAKE_VALUE_FUNC(int, mbedtls_ssl_set_hostname, mbedtls_ssl_context *, const char *);
DECLARE_FAKE_VALUE_FUNC(int, mbedtls_ssl_conf_own_cert, mbedtls_ssl_config *, mbedtls_x509_crt *, mbedtls_pk_context *);
DECLARE_FAKE_VALUE_FUNC(int, mbedtls_ssl_handshake, mbedtls_ssl_context *);
DECLARE_FAKE_VALUE_FUNC(uint32_t, mbedtls_ssl_get_verify_result, const mbedtls_ssl_context *);
DECLARE_FAKE_VALUE_FUNC(const mbedtls_x509_crt *, mbedtls_ssl_get_peer_cert, const mbedtls_ssl_context *);
DECLARE_FAKE_VALUE_FUNC(int, mbedtls_ssl_write, mbedtls_ssl_context *, const unsigned char *, size_t);
DECLARE_FAKE_VALUE_FUNC(int, mbedtls_ssl_read, mbedtls_ssl_context *, unsigned char *, size_t);
DECLARE_FAKE_VALUE_FUNC(int, mbedtls_ssl_config_defaults, mbedtls_ssl_config *, int, int, int);
DECLARE_FAKE_VALUE_FUNC(int, mbedtls_ssl_close_notify, mbedtls_ssl_context *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_free, mbedtls_ssl_context *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_init, mbedtls_ssl_context *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_conf_rng, mbedtls_ssl_config *, f_rng, void *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_conf_verify, mbedtls_ssl_config *, int *, void *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_conf_ca_chain, mbedtls_ssl_config *, mbedtls_x509_crt *, mbedtls_x509_crl *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_conf_dbg, mbedtls_ssl_config *, void *, void *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_set_bio,
                       mbedtls_ssl_context *,
                       void *,
                       mbedtls_ssl_send_t *,
                       mbedtls_ssl_recv_t *,
                       mbedtls_ssl_recv_timeout_t *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_set_bio_ctx, mbedtls_ssl_context *, void *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_config_init, mbedtls_ssl_config *);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_conf_authmode, mbedtls_ssl_config *, int);
DECLARE_FAKE_VOID_FUNC(mbedtls_ssl_config_free, mbedtls_ssl_config *);

DECLARE_FAKE_VALUE_FUNC(int, mbedtls_x509_crt_parse, mbedtls_x509_crt *, const unsigned char *, size_t);
DECLARE_FAKE_VALUE_FUNC(int, mbedtls_x509_crt_info, char *, size_t, const char *, const mbedtls_x509_crt *);
DECLARE_FAKE_VALUE_FUNC(int, mbedtls_x509_crt_verify_info, char *, size_t, const char *, uint32_t);
DECLARE_FAKE_VOID_FUNC(mbedtls_x509_crt_init, mbedtls_x509_crt *);
DECLARE_FAKE_VOID_FUNC(mbedtls_x509_crt_free, mbedtls_x509_crt *);

DECLARE_FAKE_VALUE_FUNC(int,
                        mbedtls_hmac_drbg_seed,
                        mbedtls_hmac_drbg_context *,
                        const mbedtls_md_info_t *,
                        int *,
                        void *,
                        const unsigned char *,
                        size_t);

DECLARE_FAKE_VOID_FUNC(mbedtls_pk_init, mbedtls_pk_context *);
DECLARE_FAKE_VOID_FUNC(mbedtls_pk_free, mbedtls_pk_context *);

#if (MBEDTLS_VERSION_MAJOR >= 3)
DECLARE_FAKE_VALUE_FUNC(int,
                        mbedtls_pk_parse_key,
                        mbedtls_pk_context *,
                        const unsigned char *,
                        size_t,
                        const unsigned char *,
                        size_t,
                        int *,
                        void *);
#else
DECLARE_FAKE_VALUE_FUNC(
    int, mbedtls_pk_parse_key, mbedtls_pk_context *, const unsigned char *, size_t, const unsigned char *, size_t);
#endif

DECLARE_FAKE_VOID_FUNC(mbedtls_debug_set_threshold, int);
DECLARE_FAKE_VOID_FUNC(mbedtls_strerror, int, char *, size_t);

#ifdef __cplusplus
}
#endif

#endif /* ssl.h */
