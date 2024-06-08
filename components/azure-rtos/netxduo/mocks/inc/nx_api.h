/*
 * Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NX_API_H
#define NX_API_H

#include "tx_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NX_PARAMETER_NOT_USED
#define NX_PARAMETER_NOT_USED(p) ((void)(p))
#endif /* NX_PARAMETER_NOT_USED */

#define NX_CHANGE_ULONG_ENDIAN(a)
#define NX_CHANGE_USHORT_ENDIAN(a)

/* API input parameters and general constants.  */
#define NX_NO_WAIT      0
#define NX_WAIT_FOREVER ((ULONG)0xFFFFFFFF)
#define NX_TRUE         1
#define NX_FALSE        0
#define NX_NULL         0

#ifndef NX_SEARCH_PORT_START
#define NX_SEARCH_PORT_START 0xC000 /* Free port search start UDP/TCP */
#endif                              /* NX_SEARCH_PORT_START */

/* API return values.  */
#define NX_SUCCESS           0x00
#define NX_NO_PACKET         0x01
#define NX_NOT_ENABLED       0x14
#define NX_ALREADY_ENABLED   0x15
#define NX_UNHANDLED_COMMAND 0x44

/* Link Driver commands.  */
#define NX_LINK_PACKET_SEND          0
#define NX_LINK_INITIALIZE           1
#define NX_LINK_ENABLE               2
#define NX_LINK_DISABLE              3
#define NX_LINK_PACKET_BROADCAST     4
#define NX_LINK_ARP_SEND             5
#define NX_LINK_ARP_RESPONSE_SEND    6
#define NX_LINK_RARP_SEND            7
#define NX_LINK_MULTICAST_JOIN       8
#define NX_LINK_MULTICAST_LEAVE      9
#define NX_LINK_GET_STATUS           10
#define NX_LINK_DEFERRED_PROCESSING  18
#define NX_LINK_INTERFACE_ATTACH     19
#define NX_LINK_SET_PHYSICAL_ADDRESS 20
#define NX_LINK_RX_ENABLE            25
#define NX_LINK_RX_DISABLE           26

#define NX_LINK_USER_COMMAND 50 /* Values after this value are reserved for application.  */

#define NX_RECEIVE_PACKET 0 /* This is for driver receive     */

#define NX_IP_VERSION_V4 0x4
#define NX_IP_VERSION_V6 0x6

typedef struct NX_INTERFACE_STRUCT {
    /* Define the Link Up field that is manipulated by the associated link driver.  */
    UCHAR nx_interface_link_up;

    /* Define the index in array. */
    UCHAR nx_interface_index;

    /* Define the physical address of this IP instance.  These field are
           setup by the link driver during initialization.  */
    ULONG nx_interface_physical_address_msw;
    ULONG nx_interface_physical_address_lsw;
} NX_INTERFACE;

typedef struct NX_IP_STRUCT {
    /* Define the default packet pool.  */
    struct NX_PACKET_POOL_STRUCT *nx_ip_default_packet_pool;

    /* Define the interfaces attached to this IP instance. */
    NX_INTERFACE nx_ip_interface[5];
} NX_IP;

typedef struct NX_PACKET_STRUCT {
    /* Define the pointer to the first byte written closest to the beginning of the
    buffer.  This is used to prepend information in front of the packet.  */
    UCHAR *nx_packet_prepend_ptr;

    /* Define the pointer to the byte after the last character written in the buffer.  */
    UCHAR *nx_packet_append_ptr;

    /* Define the packet data area start and end pointer.  These will be used to
       mark the physical boundaries of the packet.  */
    UCHAR *nx_packet_data_start;
    UCHAR *nx_packet_data_end;

    /* Define the total packet length.  */
    ULONG nx_packet_length;

    /*
           Tag the packet type. Valid values are either
           NX_IP_VERSION_V4 or NX_IP_VERSION_V6.
         */
    UCHAR nx_packet_ip_version;

    union {

        /* Define the interface from which the packet was received, or the interface to transmit to. */
        struct NX_INTERFACE_STRUCT *nx_packet_interface_ptr;

        /* Point to the interface IPv6 address structure.  On transmit or receive path. */
        struct NXD_IPV6_ADDRESS_STRUCT *nx_packet_ipv6_address_ptr;
    } nx_packet_address;

#define nx_packet_ip_interface nx_packet_address.nx_packet_interface_ptr
} NX_PACKET;

typedef struct NX_PACKET_POOL_STRUCT {
    unsigned int member;
} NX_PACKET_POOL;

typedef struct NX_IP_DRIVER_STRUCT {
    /* Define the driver command.  */
    UINT nx_ip_driver_command;

    /* Define the driver return status.  */
    UINT nx_ip_driver_status;

    /* Define the physical address that maps to the destination IP address.  */
    ULONG nx_ip_driver_physical_address_msw;
    ULONG nx_ip_driver_physical_address_lsw;

    /* Define the datagram packet (if any) for the driver to send.  */
    NX_PACKET *nx_ip_driver_packet;

    /* Define the return pointer for raw driver command requests.  */
    ULONG *nx_ip_driver_return_ptr;

    /* Define the IP pointer associated with the request.  */
    struct NX_IP_STRUCT *nx_ip_driver_ptr;

    NX_INTERFACE *nx_ip_driver_interface;
} NX_IP_DRIVER;

DECLARE_FAKE_VALUE_FUNC(UINT, nx_ip_interface_mtu_set, NX_IP *, UINT, ULONG);
DECLARE_FAKE_VALUE_FUNC(UINT, nx_ip_interface_physical_address_set, NX_IP *, UINT, ULONG, ULONG, UINT);
DECLARE_FAKE_VALUE_FUNC(UINT, nx_ip_interface_address_mapping_configure, NX_IP *, UINT, UINT);
DECLARE_FAKE_VALUE_FUNC(UINT, nx_packet_release, NX_PACKET *);
DECLARE_FAKE_VALUE_FUNC(UINT, nx_packet_transmit_release, NX_PACKET *);
DECLARE_FAKE_VALUE_FUNC(UINT, nx_packet_allocate, NX_PACKET_POOL *, NX_PACKET **, ULONG, ULONG);
DECLARE_FAKE_VOID_FUNC(_nx_ip_driver_deferred_processing, NX_IP *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // NX_API_H
