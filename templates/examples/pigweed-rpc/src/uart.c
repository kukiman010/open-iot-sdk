// Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include "hal/serial_api.h"
#include "mps3_uart.h"

static uint32_t BAUD_RATE = 9600;

static mps3_uart_t *init_uart(size_t i)
{
    static struct uart_cmsdk_dev_t *devs[] = {&UART0_CMSDK_DEV_NS, &UART1_CMSDK_DEV_NS};

    mps3_uart_t *uart;

    mps3_uart_init(&uart, devs[i]);
    mdh_serial_set_baud(&(uart->serial), BAUD_RATE);

    return uart;
}

void serial_setup(mdh_serial_t **serials, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        mps3_uart_t *uart = init_uart(i);
        serials[i] = &(uart->serial);
    }
}
