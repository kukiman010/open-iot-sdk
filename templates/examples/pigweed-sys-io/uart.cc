// Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

extern "C" {
#include "hal/serial_api.h"
#include "mps3_uart.h"

mdh_serial_t *get_example_serial();

void pw_sys_io_init(mdh_serial_t *);
}

void serial_setup()
{
    mdh_serial_t *serial = get_example_serial();
    mdh_serial_set_baud(serial, 9600);

    pw_sys_io_init(serial);
}
