/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/ip_network_api.h"

DEFINE_FAKE_VALUE_FUNC(osStatus_t, start_network_task, network_state_callback_t, uint32_t);
