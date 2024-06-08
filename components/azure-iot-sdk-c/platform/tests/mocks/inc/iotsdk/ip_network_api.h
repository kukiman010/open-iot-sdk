/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IP_NETWORK_API_H_
#define IP_NETWORK_API_H_

#include "cmsis_os2.h"

#include "fff.h"
#include <stdint.h>

typedef enum network_state_callback_event_t {
    NETWORK_UP,  /* The network is configured. */
    NETWORK_DOWN /* The network connection has been lost. */
} network_state_callback_event_t;

typedef void (*network_state_callback_t)(network_state_callback_event_t event);

DECLARE_FAKE_VALUE_FUNC(osStatus_t, start_network_task, network_state_callback_t, uint32_t);

#endif // IP_NETWORK_API_H_
