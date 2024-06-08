/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef IOTSOCKETTOMBED_H_
#define IOTSOCKETTOMBED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iot_socket.h"
#include "netsocket/nsapi_types.h"

nsapi_error_t iotSocketErrorToMbed(int32_t iotSocketErrorCode);

#ifdef __cplusplus
}
#endif

#endif // IOTSOCKETTOMBED_H_
