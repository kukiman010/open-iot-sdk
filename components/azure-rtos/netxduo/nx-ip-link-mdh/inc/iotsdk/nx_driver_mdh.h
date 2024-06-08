/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NX_DRIVER_MDH_H
#define NX_DRIVER_MDH_H

#include "hal/emac_api.h"
#include "nx_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NX_DRIVER_ERROR 90

/** NetX Duo network driver entry function
 *
 * @note
 * The function is used by NetX Duo for:
 * - driver initialization
 * - enabling/disabling network device
 * - sending packets
 * - processing incoming packets
 * - various control and status operations
 *
 * The supported NetX Duo commands are:
 * - NX_LINK_INTERFACE_ATTACH
 * - NX_LINK_INITIALIZE
 * - NX_LINK_ENABLE
 * - NX_LINK_DISABLE
 * - NX_LINK_GET_STATUS
 * - NX_LINK_ARP_SEND
 * - NX_LINK_ARP_RESPONSE_SEND
 * - NX_LINK_PACKET_BROADCAST
 * - NX_LINK_RARP_SEND
 * - NX_LINK_PACKET_SEND
 * - NX_LINK_MULTICAST_JOIN
 * - NX_LINK_MULTICAST_LEAVE
 * - NX_LINK_DEFERRED_PROCESSING
 *
 * @param[in] nx_ip_driver Pointer to an instance of a request structure.
 */
void nx_driver_mdh_ip_link_fsm(NX_IP_DRIVER *nx_ip_driver);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // NX_DRIVER_MDH_H
