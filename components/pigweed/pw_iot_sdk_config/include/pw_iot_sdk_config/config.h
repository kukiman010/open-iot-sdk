/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_IOT_SDK_CONFIG_CONFIG_H
#define PW_IOT_SDK_CONFIG_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Define macros we want to remain constant (i.e. not user-controlled) first.
#include "pw_iot_sdk_config/internal/preamble.h"

// Include user-supplied header if any.
#ifdef pw_iot_sdk_config_FILE
#include pw_iot_sdk_config_FILE
#endif // pw_iot_sdk_config_FILE

// Provide defaults for any unsupplied settings.
#include "pw_iot_sdk_config/config_defaults.h"

// Validate the settings.
#include "pw_iot_sdk_config/internal/validation.h"

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ! PW_IOT_SDK_CONFIG_CONFIG_H
