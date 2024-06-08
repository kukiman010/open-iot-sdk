/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/serial_api.h"
#include "cmsis_os2.h"

#include <cinttypes>
#include <cstdarg>
#include <cstdio>

#define PW_LOG_MODULE_NAME "main"

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_log_mdh/backend.h"
#include "pw_thread/id.h"
#include "pw_thread/thread.h"
#include "pw_thread/thread_core.h"
#include "pw_thread/yield.h"
#include "pw_thread_cmsis_rtos/options.h"

using pw::thread::Thread;
using pw::thread::ThreadCore;
using pw::thread::cmsis_rtos::Options;
namespace this_thread = pw::this_thread;

extern "C" mdh_serial_t *get_example_serial();

static mdh_serial_t *my_serial;

static void serial_setup(void)
{
    my_serial = get_example_serial();
    mdh_serial_set_baud(my_serial, 115200);
}

struct ThreadCoreImpl : ThreadCore {
    void Run() override
    {
        osThreadId_t id = reinterpret_cast<osThreadId_t>(this_thread::get_id());
        PW_CHECK_NOTNULL(id, "this_thread::get_id failed");

        const char *name = osThreadGetName(id);
        PW_CHECK_NOTNULL(name, "osThreadGetName failed");

        PW_LOG_INFO("%s ran to completion", name);
    }
};

static void main_task_cb(void *ignored)
{
    PW_LOG_INFO("Main task started");

    ThreadCoreImpl thread_cb1, thread_cb2, thread_cb3;

    Options options1, options2, options3;
    options1.set_name("thread1");
    options2.set_name("thread2");
    options3.set_name("thread3");

    Thread thread1(options1, thread_cb1);
    Thread thread2(options2, thread_cb2);
    Thread thread3(options3, thread_cb3);

    this_thread::yield();

    thread1.detach();
    PW_LOG_INFO("thread1 detached");
    thread2.join();
    PW_LOG_INFO("thread2 complete");
    thread3.join();
    PW_LOG_INFO("thread3 complete");

    PW_LOG_INFO("Main task complete");

    (void)ignored;
}

int main(void)
{
    serial_setup();

    int32_t res = osKernelInitialize();
    if (res != osOK) {
        printf("osKernelInitialize failed: %" PRId32 "\n", res);
        return -1;
    }

    puts("Kernel initialised");

    pw_log_mdh_init(my_serial);

    // NB: pw::thread_cmsis_rtos requires the kernel to be started before initialising the first thread, so the first
    // thread should be created via CMSIS RTOS API directly.
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
