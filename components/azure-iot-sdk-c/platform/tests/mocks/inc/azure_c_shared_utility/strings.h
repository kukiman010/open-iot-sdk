/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef STRINGS_H
#define STRINGS_H

#include "fff.h"

#include "azure_c_shared_utility/strings_types.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC(STRING_HANDLE, STRING_construct, const char *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // STRINGS_H
