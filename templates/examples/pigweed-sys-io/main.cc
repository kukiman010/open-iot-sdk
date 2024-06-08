// Copyright (c) 2021-2023, Arm Limited and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cmsis_os2.h"

#include <cinttypes>
#include <cstdarg>
#include <cstdio>

#define PW_LOG_MODULE_NAME "main"
#define MAX_LINE_LEN       100
#define FMT_BUF_SIZE       128

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_sys_io/sys_io.h"
#include "pw_sys_io_mdh/backend.h"

extern void serial_setup();

static void sys_io_printf(const char *fmt, ...)
{
    static std::byte buffer[FMT_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    size_t count = vsnprintf(reinterpret_cast<char *>(buffer), sizeof(buffer) - 1, fmt, args);
    va_end(args);

    pw::sys_io::WriteBytes(pw::ConstByteSpan(buffer, count));
}

static void main_task_cb(void *ignored)
{
    (void)ignored;

    static char buffer[MAX_LINE_LEN + 1];
    static int count;
    static bool cr = false;

    sys_io_printf("Type something at the prompt\n");

    // Read a byte at a time. When a newline is encountered or the buffer gets full, log buffer's contents.
    while (true) {
        char c;
        pw::Status status = pw::sys_io::ReadByte(reinterpret_cast<std::byte *>(&c));
        if (!status.ok()) {
            if (status.code() != PW_STATUS_UNAVAILABLE) {
                PW_LOG_ERROR("ReadByte returned error - %" PRIx32, uint32_t(status.code()));
            }

            continue;
        }

        bool print_line = false;
        // telnet may send cr, lf or even crlf.
        // If cr is received, print the new line and retain that information.
        // If a lf follows, the character is dropped.
        if (c == '\r') {
            print_line = true;
            cr = true;
        } else if (c == '\n') {
            if (!cr) {
                print_line = true;
            }
            cr = false;
        } else {
            buffer[count] = c;
            count++;
            print_line = count == MAX_LINE_LEN;
            cr = false;
        }

        if (print_line) {
            buffer[count] = 0;
            sys_io_printf("%s\n", buffer);
            count = 0;
        }
    }
}

int main()
{
    int32_t res = osKernelInitialize();
    if (res != osOK) {
        printf("osKernelInitialize failed: %" PRId32 "\n", res);
        return res;
    }

    serial_setup();
    sys_io_printf("Kernel initialised\n");

    osThreadId_t main_thread = osThreadNew(main_task_cb, nullptr, nullptr);
    if (!main_thread) {
        sys_io_printf("osThreadNew failed\n");
        return -1;
    }

    osKernelState_t state = osKernelGetState();
    if (state != osKernelReady) {
        sys_io_printf("Kernel not ready\n");
        return -1;
    }

    sys_io_printf("Starting kernel\n");

    res = osKernelStart();
    if (res != osOK) {
        sys_io_printf("osKernelStart failed\n");
        return -1;
    }

    PW_UNREACHABLE;
}
