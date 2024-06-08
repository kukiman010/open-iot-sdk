/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include "azure_c_shared_utility/strings_types.h"
#include "azure_c_shared_utility/xio.h"

enum PLATFORM_INFO_OPTION_VALUES { PLATFORM_INFO_OPTION_DEFAULT, PLATFORM_INFO_OPTION_RETRIEVE_SQM };
typedef enum PLATFORM_INFO_OPTION_VALUES PLATFORM_INFO_OPTION;

int platform_init(void);
void platform_deinit(void);
const IO_INTERFACE_DESCRIPTION *platform_get_default_tlsio(void);
STRING_HANDLE platform_get_platform_info(PLATFORM_INFO_OPTION options);

#endif // PLATFORM_H
