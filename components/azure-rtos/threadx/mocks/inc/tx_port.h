/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TX_PORT_MOCK_H
#define TX_PORT_MOCK_H

#include "tx_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TX_INTERRUPT_SAVE_AREA UINT tx_saved_posture;
#define TX_DISABLE             tx_saved_posture = _tx_thread_interrupt_disable();
#define TX_RESTORE             _tx_thread_interrupt_restore(tx_saved_posture);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TX_PORT_MOCK_H
