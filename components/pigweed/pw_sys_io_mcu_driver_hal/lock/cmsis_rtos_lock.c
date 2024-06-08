/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "assert.h"
#include "cmsis_os2.h"

// Note: the attribute osMutexPrioInherit is defined to prevent higher priority
// threads being blocked for too much time when a lower priority thread is
// logging.
// Note: The mutex is allocated dynamically as CMSIS-RTOS doesn't expose a
// common macro for the mutex size.

static osMutexId_t read_lock = NULL;
static osMutexId_t write_lock = NULL;
static const osMutexAttr_t read_lock_config = {"sys_io_rx_mutex",                     // name
                                               osMutexRecursive | osMutexPrioInherit, // attr_bits
                                               NULL,
                                               0};
static const osMutexAttr_t write_lock_config = {"sys_io_tx_mutex",                     // name
                                                osMutexRecursive | osMutexPrioInherit, // attr_bits
                                                NULL,
                                                0};

void _pw_sys_io_lock_init(void)
{
    read_lock = osMutexNew(&read_lock_config);
    assert(read_lock);

    write_lock = osMutexNew(&write_lock_config);
    assert(write_lock);
}

void _pw_sys_io_lock_read(void)
{
    osMutexAcquire(read_lock, osWaitForever);
}

void _pw_sys_io_lock_write(void)
{
    osMutexAcquire(write_lock, osWaitForever);
}

void _pw_sys_io_unlock_read(void)
{
    osMutexRelease(read_lock);
}

void _pw_sys_io_unlock_write(void)
{
    osMutexRelease(write_lock);
}
