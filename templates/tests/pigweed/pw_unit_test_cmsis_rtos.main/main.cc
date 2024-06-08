/* Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/serial_api.h"
#include "cmsis_os2.h"

#define PW_LOG_MODULE_NAME "test"

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_sys_io/sys_io.h"
#include "pw_sys_io_mdh/backend.h"
#include "pw_unit_test/internal/framework.h"
#include "pw_unit_test/simple_printing_event_handler.h"

extern "C" mdh_serial_t *get_example_serial();

// Initialise the kernel & serial before static constructors get called as Pigweed's facade tests statically declare
// things like semaphores.
extern "C" void mbed_sdk_init()
{
    osKernelInitialize();
}

static void handler_write_cb(const std::string_view &s, bool append_newline)
{
    if (append_newline) {
        pw::sys_io::WriteLine(s);
    } else {
        pw::sys_io::WriteBytes(pw::as_bytes(pw::span(s)));
    }
}

static void main_task(void *ignored)
{
    (void)ignored;

    PW_LOG_INFO("Starting Pigweed Tests");
    PW_LOG_INFO("\u2702 --------------------");

    pw::unit_test::SimplePrintingEventHandler handler(handler_write_cb);
    pw::unit_test::RegisterEventHandler(&handler);

    int res = RUN_ALL_TESTS();

    PW_LOG_INFO("\u2702 ------------------------");
    PW_LOG_INFO("Pigweed reported status %d", res);

    osThreadExit();
}

int main()
{
    mdh_serial_t *serial = get_example_serial();
    mdh_serial_set_baud(serial, 115200);

    pw_sys_io_init(serial);

    osThreadId_t thread = osThreadNew(main_task, nullptr, nullptr);
    PW_CHECK_NOTNULL(thread, "osThreadNew failed");

    osKernelState_t state = osKernelGetState();
    PW_CHECK_INT_EQ(state, osKernelReady, "Kernel is not ready");

    osKernelStart();
    PW_LOG_ERROR("osKernelStart returned");
    while (true)
        ;
}
