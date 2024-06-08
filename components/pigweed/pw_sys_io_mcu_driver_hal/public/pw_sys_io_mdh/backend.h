/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYS_IO_MDH_BACKEND_H
#define PW_SYS_IO_MDH_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/serial_api.h"

/// Initialise the sys_io subsystem.
/// @note NB: when the CMSIS RTOS lock is in use, this function must be called after osKernelInitialize as the IRQ
/// handler becomes enabled and will make use of the locks.
void pw_sys_io_init(mdh_serial_t *serial);

#ifdef __cplusplus
}
#endif

#endif // ! PW_SYS_IO_MDH_BACKEND_H
