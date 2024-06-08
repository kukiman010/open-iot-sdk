/*
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbedtls/threading.h"

DEFINE_FAKE_VOID_FUNC(mbedtls_threading_set_alt, mutex_init_f, mutex_free_f, mutex_lock_f, mutex_unlock_f);
