/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "pw_log_sys_io_mdh/config.h"
#include "pw_log_sys_io_mdh/util.h"
#include "pw_sys_io/sys_io.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern "C" {
#include "hal/serial_api.h"
}

// Output the message from pw_log
extern "C" void _pw_log_sys_io_mdh(const char *message, ...)
{
    char buffer[PW_LOG_MDH_LINE_LENGTH];

    va_list args;
    va_start(args, message);
    int result = vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);

    if (result < 0) {
        return;
    }

    const char *ptr = buffer;

    pw::sys_io::WriteLine(ptr);
}
