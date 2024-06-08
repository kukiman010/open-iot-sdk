/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef XIO_H
#define XIO_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "azure_c_shared_utility/optionhandler.h"

enum IO_SEND_RESULT_VALUES { IO_SEND_OK, IO_SEND_ERROR, IO_SEND_CANCELLED };
typedef enum IO_SEND_RESULT_VALUES IO_SEND_RESULT;

enum IO_OPEN_RESULT_VALUES { IO_OPEN_OK, IO_OPEN_ERROR, IO_OPEN_CANCELLED };
typedef enum IO_OPEN_RESULT_VALUES IO_OPEN_RESULT;

typedef void *CONCRETE_IO_HANDLE;

typedef void (*ON_BYTES_RECEIVED)(void *context, const unsigned char *buffer, size_t size);
typedef void (*ON_SEND_COMPLETE)(void *context, IO_SEND_RESULT send_result);
typedef void (*ON_IO_OPEN_COMPLETE)(void *context, IO_OPEN_RESULT open_result);
typedef void (*ON_IO_CLOSE_COMPLETE)(void *context);
typedef void (*ON_IO_ERROR)(void *context);

typedef OPTIONHANDLER_HANDLE (*IO_RETRIEVEOPTIONS)(CONCRETE_IO_HANDLE concrete_io);
typedef CONCRETE_IO_HANDLE (*IO_CREATE)(void *io_create_parameters);
typedef void (*IO_DESTROY)(CONCRETE_IO_HANDLE concrete_io);
typedef int (*IO_OPEN)(CONCRETE_IO_HANDLE concrete_io,
                       ON_IO_OPEN_COMPLETE on_io_open_complete,
                       void *on_io_open_complete_context,
                       ON_BYTES_RECEIVED on_bytes_received,
                       void *on_bytes_received_context,
                       ON_IO_ERROR on_io_error,
                       void *on_io_error_context);
typedef int (*IO_CLOSE)(CONCRETE_IO_HANDLE concrete_io,
                        ON_IO_CLOSE_COMPLETE on_io_close_complete,
                        void *callback_context);
typedef int (*IO_SEND)(CONCRETE_IO_HANDLE concrete_io,
                       const void *buffer,
                       size_t size,
                       ON_SEND_COMPLETE on_send_complete,
                       void *callback_context);
typedef void (*IO_DOWORK)(CONCRETE_IO_HANDLE concrete_io);
typedef int (*IO_SETOPTION)(CONCRETE_IO_HANDLE concrete_io, const char *optionName, const void *value);

typedef struct IO_INTERFACE_DESCRIPTION_TAG {
    IO_RETRIEVEOPTIONS concrete_io_retrieveoptions;
    IO_CREATE concrete_io_create;
    IO_DESTROY concrete_io_destroy;
    IO_OPEN concrete_io_open;
    IO_CLOSE concrete_io_close;
    IO_SEND concrete_io_send;
    IO_DOWORK concrete_io_dowork;
    IO_SETOPTION concrete_io_setoption;
} IO_INTERFACE_DESCRIPTION;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // XIO_H
