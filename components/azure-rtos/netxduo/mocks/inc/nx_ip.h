/*
 * Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NX_IP_H
#define NX_IP_H

#include "fff.h"
#include "nx_api.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VOID_FUNC(_nx_ip_packet_deferred_receive, NX_IP *, NX_PACKET *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // NX_IP_H
