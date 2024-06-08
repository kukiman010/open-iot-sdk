/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TLSIO_MBEDTLS_H
#define TLSIO_MBEDTLS_H

#include "fff.h"

#include "azure_c_shared_utility/xio.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC(IO_INTERFACE_DESCRIPTION *, tlsio_mbedtls_get_interface_description);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TLSIO_MBEDTLS_H
