/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "IotSocketToMbed.h"

nsapi_error_t iotSocketErrorToMbed(int32_t iotSocketErrorCode)
{
    if (iotSocketErrorCode > 0) {
        return iotSocketErrorCode;
    }

    nsapi_error_t rc = 0;
    switch (iotSocketErrorCode) {
        case 0:
            rc = NSAPI_ERROR_OK;
            break;
        case IOT_SOCKET_ERROR:
            rc = NSAPI_ERROR_UNKNOWN;
            break;
        case IOT_SOCKET_ENOTSUP:
            rc = NSAPI_ERROR_UNSUPPORTED;
            break;
        case IOT_SOCKET_ENOMEM:
            rc = NSAPI_ERROR_NO_MEMORY;
            break;
        case IOT_SOCKET_EINPROGRESS:
            rc = NSAPI_ERROR_IN_PROGRESS;
            break;
        case IOT_SOCKET_EAGAIN:
            rc = NSAPI_ERROR_WOULD_BLOCK;
            break;
        case IOT_SOCKET_ETIMEDOUT:
            rc = NSAPI_ERROR_CONNECTION_TIMEOUT;
            break;
        case IOT_SOCKET_EISCONN:
            rc = NSAPI_ERROR_IS_CONNECTED;
            break;
        case IOT_SOCKET_EINVAL:
        case IOT_SOCKET_ESOCK:
            rc = NSAPI_ERROR_NO_SOCKET;
            break;
        case IOT_SOCKET_ENOTCONN:
            rc = NSAPI_ERROR_NO_CONNECTION;
            break;
        case IOT_SOCKET_ECONNRESET:
        case IOT_SOCKET_ECONNREFUSED:
            rc = NSAPI_ERROR_AUTH_FAILURE;
            break;
        case IOT_SOCKET_ECONNABORTED:
            rc = NSAPI_ERROR_CONNECTION_LOST;
            break;
        case IOT_SOCKET_EALREADY:
            rc = NSAPI_ERROR_ALREADY;
            break;
        case IOT_SOCKET_EADDRINUSE:
            rc = NSAPI_ERROR_ADDRESS_IN_USE;
            break;
        case IOT_SOCKET_EHOSTNOTFOUND:
            rc = NSAPI_ERROR_HOST_NOT_FOUND;
            break;
        default:
            rc = NSAPI_ERROR_UNKNOWN;
    }
    return rc;
}
