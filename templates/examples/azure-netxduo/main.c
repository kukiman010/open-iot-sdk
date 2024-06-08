/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

// For information how to use Azure RTOS ThreadX please visit:
// https://docs.microsoft.com/en-us/azure/rtos/threadx/chapter2#using-threadx

#include "iotsdk/nx_driver_mdh.h"

#include "app_config.h"
#include "nx_api.h"
#include "tx_api.h"

#ifdef NX_DISABLE_LOOPBACK_INTERFACE
#error "Ensure the configurable option `NX_DISABLE_LOOPBACK_INTERFACE` is not set!"
#endif

static UINT init_nx_ip(uint8_t *stack_pointer);
static void udp_socket_thread_entry(ULONG context);
static void release_socket(NX_UDP_SOCKET *socket, CHAR *name);

static TX_THREAD gs_udp_socket_thread = {0};
static NX_IP gs_network_ip = {0};
static NX_PACKET_POOL gs_packet_pool = {0};
static ULONG gs_dut_ip_address = IP_ADDRESS(127, 0, 0, 1);

static UCHAR gs_packet_pool_buffer[PACKET_POOL_SIZE];

int main(void)
{
    tx_kernel_enter();
}

void tx_application_define(void *first_unused_memory)
{
    printf("Initializing system resources.\r\n");
    uint8_t *stack_pointer = (uint8_t *)first_unused_memory;

    UINT status = tx_thread_create(&gs_udp_socket_thread,
                                   "UDP Socket",
                                   udp_socket_thread_entry,
                                   0UL,
                                   stack_pointer,
                                   THREAD_STACK_SIZE,
                                   THREAD_PRIORITY_UDP_SOCKET,
                                   THREAD_PRIORITY_UDP_SOCKET,
                                   TX_NO_TIME_SLICE,
                                   TX_AUTO_START);
    stack_pointer += THREAD_STACK_SIZE;
    if (status != TX_SUCCESS) {
        printf("Error (0x%02X): UDP thread could not be created.\r\n", status);
        return;
    }

    status = init_nx_ip(stack_pointer);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): Failed to initialize Network IP.\r\n", status);
        return;
    }
}

static UINT init_nx_ip(uint8_t *stack_pointer)
{
    printf("Initializing network.\r\n");

    nx_system_initialize();

    UINT status = nx_packet_pool_create(
        &gs_packet_pool, "NetX Packet Pool", PACKET_SIZE, gs_packet_pool_buffer, PACKET_POOL_SIZE);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): NetX Duo packet pool could not be created.\r\n", status);
        return status;
    }

    status = nx_ip_create(&gs_network_ip,
                          "Network IP",
                          gs_dut_ip_address,
                          IP_ADDRESS(255, 255, 255, 0),
                          &gs_packet_pool,
                          nx_driver_mdh_ip_link_fsm,
                          stack_pointer,
                          IP_STACK_SIZE,
                          THREAD_PRIORITY_NETWORK_IP);
    if (status != NX_SUCCESS) {
        nx_packet_pool_delete(&gs_packet_pool);
        printf("Error (0x%02X): NetX Duo IP instance could not be created.\r\n", status);
        return status;
    }

    status = nx_udp_enable(&gs_network_ip);
    if (status != NX_SUCCESS) {
        nx_ip_delete(&gs_network_ip);
        nx_packet_pool_delete(&gs_packet_pool);
        printf("Error (0x%02X): UDP could not be enabled\r\n", status);
        return status;
    }

    return status;
}

static void udp_socket_thread_entry(ULONG context)
{
    NX_PARAMETER_NOT_USED(context);

    ULONG actual_status = UINT32_MAX;
    const ULONG needed_status = NX_IP_INITIALIZE_DONE;
    UINT status = nx_ip_status_check(&gs_network_ip, needed_status, &actual_status, NX_IP_PERIODIC_RATE);
    if ((status != NX_SUCCESS) && (actual_status != needed_status)) {
        printf("Error (0x%02X): Expected `0x%02X` but got `0x%02X`.\r\n",
               status,
               (UINT)needed_status,
               (UINT)actual_status);
        return;
    }

    printf("DUT IP address: %lu.%lu.%lu.%lu.\r\n",
           (gs_dut_ip_address >> 24) & 0xff,
           (gs_dut_ip_address >> 16) & 0xff,
           (gs_dut_ip_address >> 8) & 0xff,
           (gs_dut_ip_address & 0xff));

    // Initialise client socket
    NX_UDP_SOCKET client_socket = {0};
    status = nx_udp_socket_create(&gs_network_ip,
                                  &client_socket,
                                  UDP_SOCKET_NAME_CLIENT,
                                  NX_IP_NORMAL,
                                  NX_FRAGMENT_OKAY,
                                  NX_IP_TIME_TO_LIVE,
                                  UDP_SOCKET_MAX_QUEUE_SIZE);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): %s socket could not be created.\r\n", status, UDP_SOCKET_NAME_CLIENT);
        return;
    }
    printf("%s socket created.\r\n", UDP_SOCKET_NAME_CLIENT);

    status = nx_udp_socket_bind(&client_socket, UDP_PORT_CLIENT, NX_WAIT_FOREVER);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): %s socket could not be bound.\r\n", status, UDP_SOCKET_NAME_CLIENT);
        nx_udp_socket_delete(&client_socket);
        return;
    }
    printf("%s socket bound.\r\n", UDP_SOCKET_NAME_CLIENT);

    // Initialise server socket
    NX_UDP_SOCKET server_socket = {0};
    status = nx_udp_socket_create(&gs_network_ip,
                                  &server_socket,
                                  UDP_SOCKET_NAME_SERVER,
                                  NX_IP_NORMAL,
                                  NX_FRAGMENT_OKAY,
                                  NX_IP_TIME_TO_LIVE,
                                  UDP_SOCKET_MAX_QUEUE_SIZE);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): %s socket could not be created.\r\n", status, UDP_SOCKET_NAME_SERVER);
        release_socket(&client_socket, UDP_SOCKET_NAME_CLIENT);
        return;
    }
    printf("%s socket created.\r\n", UDP_SOCKET_NAME_SERVER);

    status = nx_udp_socket_bind(&server_socket, UDP_PORT_SERVER, NX_WAIT_FOREVER);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): %s socket could not be bound.\r\n", status, UDP_SOCKET_NAME_SERVER);
        nx_udp_socket_delete(&server_socket);
        release_socket(&client_socket, UDP_SOCKET_NAME_CLIENT);
        return;
    }
    printf("%s socket bound.\r\n", UDP_SOCKET_NAME_SERVER);

    // Set up datagram packet
    NX_PACKET *outgoing_packet;
    status = nx_packet_allocate(&gs_packet_pool, &outgoing_packet, NX_UDP_PACKET, NX_WAIT_FOREVER);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): Could not allocate memory for outgoing packet.\r\n", status);
        release_socket(&client_socket, UDP_SOCKET_NAME_CLIENT);
        return;
    }
    printf("Outgoing packet memory allocated.\r\n");

    status = nx_packet_data_append(
        outgoing_packet, UDP_SOCKET_DATA, sizeof(UDP_SOCKET_DATA), &gs_packet_pool, TX_WAIT_FOREVER);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): Could not append data to outgoing packet.\r\n", status);
        release_socket(&server_socket, UDP_SOCKET_NAME_SERVER);
        release_socket(&client_socket, UDP_SOCKET_NAME_CLIENT);
        return;
    }
    printf("Data appended to outgoing packet memory.\r\n");

    ULONG length = 0;
    status = nx_packet_length_get(outgoing_packet, &length);
    if ((status != NX_SUCCESS) || (length != sizeof(UDP_SOCKET_DATA))) {
        printf("Error (0x%02X): Outgoing packet length could not be retrieved.\r\n", status);
        release_socket(&server_socket, UDP_SOCKET_NAME_SERVER);
        release_socket(&client_socket, UDP_SOCKET_NAME_CLIENT);
        return;
    }

    // Send datagram packet
    status = nx_udp_socket_send(&client_socket, outgoing_packet, gs_dut_ip_address, UDP_PORT_SERVER);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): Could not send packet with UDP socket.\r\n", status);
        nx_packet_release(outgoing_packet);
        release_socket(&client_socket, UDP_SOCKET_NAME_CLIENT);
        return;
    }
    printf("%s socket sent data packet.\r\n", UDP_SOCKET_NAME_CLIENT);

    // Check datagram packet reception
    printf("%s socket waits for packet.\r\n", UDP_SOCKET_NAME_SERVER);
    NX_PACKET *incoming_packet = NX_NULL;
    status = nx_udp_socket_receive(&server_socket, &incoming_packet, UDP_SOCKET_WAIT);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): %s socket did not receive data.\r\n", status, UDP_SOCKET_NAME_SERVER);
        release_socket(&server_socket, UDP_SOCKET_NAME_SERVER);
        release_socket(&client_socket, UDP_SOCKET_NAME_CLIENT);
        return;
    }
    printf("%s socket received packet.\r\n", UDP_SOCKET_NAME_SERVER);

    status = nx_packet_release(incoming_packet);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): Could not release received packet.\r\n", status);
        release_socket(&server_socket, UDP_SOCKET_NAME_SERVER);
        release_socket(&client_socket, UDP_SOCKET_NAME_CLIENT);
        return;
    }
    printf("Received packet released.\r\n");

    // Clean up and end test
    release_socket(&server_socket, UDP_SOCKET_NAME_SERVER);
    release_socket(&client_socket, UDP_SOCKET_NAME_CLIENT);

    printf("Test successfully completed.\r\n");

    tx_thread_sleep(NX_IP_PERIODIC_RATE);
}

static void release_socket(NX_UDP_SOCKET *socket, CHAR *name)
{
    UINT status = nx_udp_socket_unbind(socket);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): %s socket could not unbind.\r\n", status, name);
    } else {
        printf("%s socket unbound.\r\n", name);
    }

    status = nx_udp_socket_delete(socket);
    if (status != NX_SUCCESS) {
        printf("Error (0x%02X): %s socket could not be deleted.\r\n", status, name);
    } else {
        printf("%s socket deleted.\r\n", name);
    }
}
