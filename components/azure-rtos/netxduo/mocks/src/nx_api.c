/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nx_api.h"

DEFINE_FAKE_VALUE_FUNC(UINT, nx_ip_interface_mtu_set, NX_IP *, UINT, ULONG);
DEFINE_FAKE_VALUE_FUNC(UINT, nx_ip_interface_physical_address_set, NX_IP *, UINT, ULONG, ULONG, UINT);
DEFINE_FAKE_VALUE_FUNC(UINT, nx_ip_interface_address_mapping_configure, NX_IP *, UINT, UINT);
DEFINE_FAKE_VALUE_FUNC(UINT, nx_packet_release, NX_PACKET *);
DEFINE_FAKE_VALUE_FUNC(UINT, nx_packet_transmit_release, NX_PACKET *);
DEFINE_FAKE_VALUE_FUNC(UINT, nx_packet_allocate, NX_PACKET_POOL *, NX_PACKET **, ULONG, ULONG);
DEFINE_FAKE_VOID_FUNC(_nx_ip_driver_deferred_processing, NX_IP *);
