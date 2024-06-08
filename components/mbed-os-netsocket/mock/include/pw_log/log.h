/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PW_LOG_INFO    printf
#define PW_LOG_DEBUG   printf
#define PW_LOG_ERROR   printf
#define PW_LOG_WARNING printf

#ifdef __cplusplus
}
#endif

#endif // LOG_H_
