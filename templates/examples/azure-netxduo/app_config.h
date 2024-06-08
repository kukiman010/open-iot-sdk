/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_NETX_DUO_CONFIG_H
#define APP_NETX_DUO_CONFIG_H

#include "nx_api.h"

#define THREAD_STACK_SIZE 4096UL
#define IP_STACK_SIZE     2048UL

#define THREAD_PRIORITY_NETWORK_IP 1U
#define THREAD_PRIORITY_UDP_SOCKET 3U

#define UDP_SOCKET_MAX_QUEUE_SIZE 5U

#define UDP_PORT_CLIENT 88U
#define UDP_PORT_SERVER 89U

#define PACKET_COUNT     10
#define PACKET_SIZE      1536
#define PACKET_POOL_SIZE ((PACKET_SIZE + sizeof(NX_PACKET)) * PACKET_COUNT)

#define UDP_SOCKET_WAIT (10 * NX_IP_PERIODIC_RATE)

#define UDP_SOCKET_DATA        "NetX Duo on Open IoT SDK!"
#define UDP_SOCKET_NAME_CLIENT "Client"
#define UDP_SOCKET_NAME_SERVER "Server"

#endif // APP_NETX_DUO_CONFIG_H
