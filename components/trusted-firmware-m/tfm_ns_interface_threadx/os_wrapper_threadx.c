/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "os_wrapper/mutex.h"
#include "tx_api.h"

#include <stdbool.h>

/* OS wrapper mutex implementation based on ThreadX.
 * Note: Arguments such as NULL pointers are checked by ThreadX and not duplicated here. */

static TX_MUTEX mutex = {0};
static bool was_mutex_created = false;

void *os_wrapper_mutex_create(void)
{
    if (!was_mutex_created && (tx_mutex_create(&mutex, "tfm_ns_interface_mutex", TX_NO_INHERIT) == TX_SUCCESS)) {
        was_mutex_created = true;
        return &mutex;
    }

    return NULL;
}

uint32_t os_wrapper_mutex_acquire(void *handle, uint32_t timeout)
{
    if (timeout == OS_WRAPPER_WAIT_FOREVER) {
        timeout = TX_WAIT_FOREVER;
    }

    if ((handle == &mutex) && (was_mutex_created) && tx_mutex_get((TX_MUTEX *)handle, (ULONG)timeout) == TX_SUCCESS) {
        return OS_WRAPPER_SUCCESS;
    }

    return OS_WRAPPER_ERROR;
}

uint32_t os_wrapper_mutex_release(void *handle)
{
    if ((handle == &mutex) && (was_mutex_created) && tx_mutex_put((TX_MUTEX *)handle) == TX_SUCCESS) {
        return OS_WRAPPER_SUCCESS;
    }

    return OS_WRAPPER_ERROR;
}

uint32_t os_wrapper_mutex_delete(void *handle)
{
    if ((handle == &mutex) && (was_mutex_created) && (tx_mutex_delete((TX_MUTEX *)handle) == TX_SUCCESS)) {
        was_mutex_created = false;
        return OS_WRAPPER_SUCCESS;
    }

    return OS_WRAPPER_ERROR;
}
