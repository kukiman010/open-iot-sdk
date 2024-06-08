// Copyright (c) 2021-2023, Arm Limited and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include "hal/serial_api.h"
#include "cmsis_os2.h"

#include <array>
#include <cassert>
#include <cinttypes>
#include <cstdarg>
#include <cstdio>

#define PW_LOG_MODULE_NAME "main"

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_log_mdh/backend.h"

extern "C" {
void serial_setup(mdh_serial_t **, size_t);
void pw_sys_io_init(mdh_serial_t *);
}

extern void start_rpc_app();

static void main_task_cb(void *ignored)
{
    (void)ignored;

    start_rpc_app();
}

int main()
{
    std::array<mdh_serial_t *, 2> serials;

    serial_setup(serials.data(), serials.size());

    int32_t res = osKernelInitialize();
    assert(res == osOK);

    puts("Kernel initialised");

    pw_log_mdh_init(serials[0]);
    pw_sys_io_init(serials[1]);

    osThreadId_t main_thread = osThreadNew(main_task_cb, nullptr, nullptr);
    PW_CHECK_NOTNULL(main_thread, "osThreadNew failed");

    auto state = osKernelGetState();
    PW_CHECK_INT_EQ(state, osKernelReady, "kernel is not ready");

    PW_LOG_INFO("Starting kernel");

    res = osKernelStart();
    PW_CHECK_INT_EQ(res, osOK, "osKernelStart failed");

    PW_LOG_CRITICAL("osKernelStart should not have returned but did");
    PW_UNREACHABLE;
}
