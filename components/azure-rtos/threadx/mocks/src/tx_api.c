/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tx_api.h"

DEFINE_FAKE_VALUE_FUNC(UINT, tx_thread_preemption_change, TX_THREAD *, UINT, UINT *);
DEFINE_FAKE_VALUE_FUNC(TX_THREAD *, tx_thread_identify);
DEFINE_FAKE_VALUE_FUNC(UINT, _tx_thread_interrupt_disable);
DEFINE_FAKE_VOID_FUNC(_tx_thread_interrupt_restore, UINT);

DEFINE_FAKE_VALUE_FUNC(UINT, tx_mutex_create, TX_MUTEX *, CHAR *, UINT);
DEFINE_FAKE_VALUE_FUNC(UINT, tx_mutex_delete, TX_MUTEX *);
DEFINE_FAKE_VALUE_FUNC(UINT, tx_mutex_get, TX_MUTEX *, ULONG);
DEFINE_FAKE_VALUE_FUNC(UINT, tx_mutex_put, TX_MUTEX *);
