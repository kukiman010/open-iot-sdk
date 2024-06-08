/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/nx_driver_mdh.h"

#include "hal/emac_api.h"
#include "iotsdk_memory_manager.h"
#include "nx_arp.h"
#include "nx_ip.h"
#include "nx_rarp.h"
#include "tx_port.h"

#include <stdbool.h>

#define ETHER_PACKET_TYPE_IP   0x0800
#define ETHER_PACKET_TYPE_IPV6 0x86dd
#define ETHER_PACKET_TYPE_ARP  0x0806
#define ETHER_PACKET_TYPE_RARP 0x8035

#define ETHERNET_FRAME_SIZE 14

#define PHYSICAL_ADDRESS_SIZE 6

#define ETHERNET_ETHER_TYPE_MSB_POS 12U
#define ETHERNET_ETHER_TYPE_LSB_POS 13U

#define ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE 2U

enum {
    NX_DRIVER_STATE_NOT_INITIALIZED = 1,
    NX_DRIVER_STATE_INITIALIZE_FAILED = 2,
    NX_DRIVER_STATE_INITIALIZED = 3,
    NX_DRIVER_STATE_LINK_ENABLED = 4
};

#define DEFERRED_EVENT_PACKET_RECEIVED (1 << 0)

typedef struct nx_driver_s {
    NX_INTERFACE *interface; /* Define the driver interface association.  */
    NX_IP *ip;               /* NetX IP instance that this driver is attached to.  */
} nx_driver_t;

typedef struct device_driver_s {
    uint8_t state;
    bool busy_transmitting;
    mdh_emac_t *(*get_emac_fn)(void);
    mdh_network_stack_memory_manager_t *memory_manager;
    size_t mtu;
    size_t memory_alignment;
    uint8_t mac_address[PHYSICAL_ADDRESS_SIZE];
    NX_PACKET *outgoing_packet;
    NX_PACKET_POOL *incomming_packet_pool;
    nx_driver_t *nx_driver;
    uint8_t deferred_event;
} device_driver_t;

/** Get an instance of a MCU-Driver-HAL EMAC
 *
 * @note
 * This function must be provided by the application until a factory is available
 * from MCU Driver HAL
 *
 * @returns A pointer to an MCU-Driver-HAL EMAC instance that has access to platform specific API calls.,
 */
extern mdh_emac_t *mdh_emac_get_default_instance(void);

static void nx_driver_mdh_ip_link_fsm_impl(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);

static void nx_link_interface_attach(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static void nx_link_initialize(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static void nx_link_enable(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static void nx_link_disable(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static void nx_link_packet_send(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static void nx_link_multicast_join(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static void nx_link_multicast_leave(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static void nx_link_get_status(NX_IP_DRIVER *nx_ip_driver);
static void nx_link_deferred_processing(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static bool transfer_to_netx(NX_IP_DRIVER *nx_ip_driver, NX_PACKET *incoming_packet);
static inline void remove_nx_packet_ethernet_header(NX_PACKET *packet);

static void add_ethernet_header(NX_IP_DRIVER *nx_ip_driver, uint32_t *ethernet_frame_ptr);
static UINT hardware_initialize(device_driver_t *device);
static UINT hardware_enable(device_driver_t *device);
static UINT hardware_disable(device_driver_t *device);
static UINT hardware_multicast_join(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static UINT hardware_multicast_leave(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static UINT hardware_send_packet(NX_PACKET *outgoing_packet, device_driver_t *device);
static UINT hardware_receive_packet(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device);
static void on_receive_complete(mdh_emac_t *self, void *ctx, mdh_emac_receive_t status);
static void
on_transmit_complete(mdh_emac_t *self, void *ctx, mdh_emac_transfer_t status, const mdh_network_stack_buffer_t *buffer);
static void on_event(mdh_emac_t *self, void *ctx, mdh_emac_event_t event);

static nx_driver_t gs_nx_driver = {0};
static device_driver_t gs_device = {.nx_driver = &gs_nx_driver, .get_emac_fn = mdh_emac_get_default_instance};
static const mdh_emac_callbacks_t gs_cbks = {.rx = on_receive_complete, .tx = on_transmit_complete, .event = on_event};

void nx_driver_mdh_ip_link_fsm(NX_IP_DRIVER *nx_ip_driver)
{
    nx_driver_mdh_ip_link_fsm_impl(nx_ip_driver, &gs_device);
}

static void nx_driver_mdh_ip_link_fsm_impl(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    nx_ip_driver->nx_ip_driver_status = NX_SUCCESS;

    switch (nx_ip_driver->nx_ip_driver_command) {
        case NX_LINK_INTERFACE_ATTACH:
            nx_link_interface_attach(nx_ip_driver, device);
            break;
        case NX_LINK_INITIALIZE:
            nx_link_initialize(nx_ip_driver, device);
            break;
        case NX_LINK_ENABLE:
            nx_link_enable(nx_ip_driver, device);
            break;
        case NX_LINK_DISABLE:
            nx_link_disable(nx_ip_driver, device);
            break;
        case NX_LINK_GET_STATUS:
            nx_link_get_status(nx_ip_driver);
            break;
        case NX_LINK_ARP_SEND:
        case NX_LINK_ARP_RESPONSE_SEND:
        case NX_LINK_PACKET_BROADCAST:
        case NX_LINK_RARP_SEND:
        case NX_LINK_PACKET_SEND:
            nx_link_packet_send(nx_ip_driver, device);
            break;
        case NX_LINK_MULTICAST_JOIN:
            nx_link_multicast_join(nx_ip_driver, device);
            break;
        case NX_LINK_MULTICAST_LEAVE:
            nx_link_multicast_leave(nx_ip_driver, device);
            break;
        case NX_LINK_DEFERRED_PROCESSING:
            nx_link_deferred_processing(nx_ip_driver, device);
            break;
        default:
            nx_ip_driver->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
            break;
    }
}

static void nx_link_interface_attach(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    device->nx_driver->interface = nx_ip_driver->nx_ip_driver_interface;
    device->nx_driver->ip = nx_ip_driver->nx_ip_driver_ptr;
}

static void nx_link_initialize(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    device->state = NX_DRIVER_STATE_NOT_INITIALIZED;

    if (hardware_initialize(device) != NX_SUCCESS) {
        nx_ip_driver->nx_ip_driver_status = NX_DRIVER_ERROR;
        return;
    }

    device->incomming_packet_pool = nx_ip_driver->nx_ip_driver_ptr->nx_ip_default_packet_pool;

    UINT interface_index = nx_ip_driver->nx_ip_driver_interface->nx_interface_index;

    nx_ip_interface_mtu_set(nx_ip_driver->nx_ip_driver_ptr, interface_index, (device->mtu - ETHERNET_FRAME_SIZE));

    nx_ip_interface_physical_address_set(nx_ip_driver->nx_ip_driver_ptr,
                                         interface_index,
                                         (uint32_t)((device->mac_address[0] << 8) | (device->mac_address[1])),
                                         (uint32_t)((device->mac_address[2] << 24) | (device->mac_address[3] << 16)
                                                    | (device->mac_address[4] << 8) | (device->mac_address[5])),
                                         NX_FALSE);

    nx_ip_interface_address_mapping_configure(nx_ip_driver->nx_ip_driver_ptr, interface_index, NX_TRUE);

    device->state = NX_DRIVER_STATE_INITIALIZED;
}

static void nx_link_enable(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    if (device->state < NX_DRIVER_STATE_INITIALIZED) {
        nx_ip_driver->nx_ip_driver_status = NX_DRIVER_ERROR;
        return;
    }

    if (device->state == NX_DRIVER_STATE_LINK_ENABLED) {
        nx_ip_driver->nx_ip_driver_status = NX_ALREADY_ENABLED;
        return;
    }

    if (hardware_enable(device) != NX_SUCCESS) {
        nx_ip_driver->nx_ip_driver_status = NX_DRIVER_ERROR;
        return;
    }

    nx_ip_driver->nx_ip_driver_interface->nx_interface_link_up = NX_TRUE;
    device->state = NX_DRIVER_STATE_LINK_ENABLED;
}

static void nx_link_disable(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    if (device->state != NX_DRIVER_STATE_LINK_ENABLED) {
        // As documentation is not provided for what to do in this case, return an error as done by
        // Microsoft sample drivers at
        // https://github.com/azure-rtos/getting-started/blob/master/NXP/MIMXRT1050-EVKB/lib/
        // netx_driver/src/nx_driver_imxrt10xx.c#L592
        nx_ip_driver->nx_ip_driver_status = NX_DRIVER_ERROR;
        return;
    }

    if (hardware_disable(device) != NX_SUCCESS) {
        nx_ip_driver->nx_ip_driver_status = NX_DRIVER_ERROR;
        return;
    }

    nx_ip_driver->nx_ip_driver_interface->nx_interface_link_up = NX_FALSE;
    device->state = NX_DRIVER_STATE_INITIALIZED;
}

static void nx_link_packet_send(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    if (device->state != NX_DRIVER_STATE_LINK_ENABLED) {
        nx_ip_driver->nx_ip_driver_status = NX_NOT_ENABLED;
        return;
    }

    NX_PACKET *outgoing_packet = nx_ip_driver->nx_ip_driver_packet;

    // Point to the begining of the Ethernet packet instead of its payload
    outgoing_packet->nx_packet_prepend_ptr = outgoing_packet->nx_packet_prepend_ptr - ETHERNET_FRAME_SIZE;

    // Ensure the size includes the Ethernet header
    outgoing_packet->nx_packet_length = outgoing_packet->nx_packet_length + ETHERNET_FRAME_SIZE;

    if (outgoing_packet->nx_packet_length > device->mtu) {
        remove_nx_packet_ethernet_header(outgoing_packet);
        nx_ip_driver->nx_ip_driver_status = NX_DRIVER_ERROR;
        return;
    }

    // Go back another 2 bytes (2 + 14 = 16 which is divisible by 4) to be at 4 byte alignment.
    uint32_t *ethernet_frame_ptr =
        (uint32_t *)(outgoing_packet->nx_packet_prepend_ptr - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE);

    add_ethernet_header(nx_ip_driver, ethernet_frame_ptr);

    device->outgoing_packet = outgoing_packet;

    UINT status = hardware_send_packet(outgoing_packet, device);

    if (status != NX_SUCCESS) {
        remove_nx_packet_ethernet_header(outgoing_packet);
        nx_ip_driver->nx_ip_driver_status = NX_DRIVER_ERROR;
    }

    // The packet is released in the transmit callback if successfully sent or an error occurs whilst sending.
    // Otherwise it is up to the application to release.
}

static void nx_link_multicast_join(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    if (device->state != NX_DRIVER_STATE_LINK_ENABLED) {
        nx_ip_driver->nx_ip_driver_status = NX_NOT_ENABLED;
        return;
    }

    if (hardware_multicast_join(nx_ip_driver, device) != NX_SUCCESS) {
        nx_ip_driver->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}

static void nx_link_multicast_leave(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    if (device->state != NX_DRIVER_STATE_LINK_ENABLED) {
        nx_ip_driver->nx_ip_driver_status = NX_NOT_ENABLED;
        return;
    }

    if (hardware_multicast_leave(nx_ip_driver, device) != NX_SUCCESS) {
        nx_ip_driver->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}

static void nx_link_get_status(NX_IP_DRIVER *nx_ip_driver)
{
    *(nx_ip_driver->nx_ip_driver_return_ptr) = nx_ip_driver->nx_ip_driver_ptr->nx_ip_interface[0].nx_interface_link_up;
}

static void nx_link_deferred_processing(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    TX_INTERRUPT_SAVE_AREA
    TX_DISABLE

    uint8_t deferred_events = device->deferred_event;

    // Assert that no unexpected events are deferred
    assert((device->deferred_event & ~DEFERRED_EVENT_PACKET_RECEIVED) == 0);

    device->deferred_event = 0;

    TX_RESTORE

    if (deferred_events & DEFERRED_EVENT_PACKET_RECEIVED) {
        if (hardware_receive_packet(nx_ip_driver, device) != NX_SUCCESS) {
            nx_ip_driver->nx_ip_driver_status = NX_DRIVER_ERROR;
        }
    }
}

static bool transfer_to_netx(NX_IP_DRIVER *nx_ip_driver, NX_PACKET *incoming_packet)
{
    incoming_packet->nx_packet_ip_interface = nx_ip_driver->nx_ip_driver_interface;

    const uint32_t packet_type_msb =
        (uint32_t) * (incoming_packet->nx_packet_prepend_ptr + ETHERNET_ETHER_TYPE_MSB_POS);
    const uint32_t packet_type_lsb =
        (uint32_t) * (incoming_packet->nx_packet_prepend_ptr + ETHERNET_ETHER_TYPE_LSB_POS);
    const uint16_t packet_type = (uint16_t)((packet_type_msb << 8) | packet_type_lsb);

    // Point to the Ethernet payload
    // Note:  The length reported includes bytes after the packet as well as
    //        the Ethernet header. The actual packet length after the Ethernet
    //        header should be derived from the length in the IP header
    //        (lower 16 bits of the first 32-bit word).
    remove_nx_packet_ethernet_header(incoming_packet);

    switch (packet_type) {
        case ETHER_PACKET_TYPE_IP:
        case ETHER_PACKET_TYPE_IPV6:
            _nx_ip_packet_deferred_receive(nx_ip_driver->nx_ip_driver_ptr, incoming_packet);
            break;
        case ETHER_PACKET_TYPE_ARP:
            _nx_arp_packet_deferred_receive(nx_ip_driver->nx_ip_driver_ptr, incoming_packet);
            break;
        case ETHER_PACKET_TYPE_RARP:
            _nx_rarp_packet_deferred_receive(nx_ip_driver->nx_ip_driver_ptr, incoming_packet);
            break;
        default:
            return false;
    }

    return true;
}

static UINT hardware_initialize(device_driver_t *device)
{
    device->memory_manager = iotsdk_memory_manager_get_instance();

    // Ethernet controller must be enabled to get/set MAC address
    if (hardware_enable(device) != NX_SUCCESS) {
        return NX_DRIVER_ERROR;
    }

    mdh_emac_t *emac = device->get_emac_fn();
    device->mtu = mdh_emac_get_mtu(emac);
    device->memory_alignment = mdh_emac_get_align(emac);

    if (mdh_emac_get_mac_addr(emac, device->mac_address) != MDH_EMAC_STATUS_NO_ERROR) {
        return NX_DRIVER_ERROR;
    }

    device->busy_transmitting = false;

    return NX_SUCCESS;
}

static UINT hardware_enable(device_driver_t *device)
{
    return (mdh_emac_power_up(device->get_emac_fn(), (mdh_emac_callbacks_t *)&gs_cbks, device->memory_manager, device)
            == MDH_EMAC_STATUS_NO_ERROR)
               ? NX_SUCCESS
               : NX_DRIVER_ERROR;
}

static UINT hardware_disable(device_driver_t *device)
{
    return (mdh_emac_power_down(device->get_emac_fn()) == MDH_EMAC_STATUS_NO_ERROR) ? NX_SUCCESS : NX_DRIVER_ERROR;
}

static UINT hardware_multicast_join(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    uint8_t addr[PHYSICAL_ADDRESS_SIZE] = {(uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_msw >> 8),
                                           (uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_msw),
                                           (uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_lsw >> 24),
                                           (uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_lsw >> 16),
                                           (uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_lsw >> 8),
                                           (uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_lsw)};

    return (mdh_emac_add_to_multicast_group(device->get_emac_fn(), addr) == MDH_EMAC_STATUS_NO_ERROR) ? NX_SUCCESS
                                                                                                      : NX_DRIVER_ERROR;
}

static UINT hardware_multicast_leave(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    uint8_t addr[PHYSICAL_ADDRESS_SIZE] = {(uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_msw >> 8),
                                           (uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_msw),
                                           (uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_lsw >> 24),
                                           (uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_lsw >> 16),
                                           (uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_lsw >> 8),
                                           (uint8_t)(nx_ip_driver->nx_ip_driver_physical_address_lsw)};

    return (mdh_emac_remove_from_multicast_group(device->get_emac_fn(), addr) == MDH_EMAC_STATUS_NO_ERROR)
               ? NX_SUCCESS
               : NX_DRIVER_ERROR;
}

static UINT hardware_send_packet(NX_PACKET *outgoing_packet, device_driver_t *device)
{
    if (device->busy_transmitting) {
        return NX_DRIVER_ERROR;
    }

    mdh_network_stack_buffer_t *buffer = mdh_network_stack_memory_manager_alloc_from_heap(
        device->memory_manager, outgoing_packet->nx_packet_length, device->memory_alignment);

    if (buffer == NULL) {
        return NX_DRIVER_ERROR;
    }

    mdh_network_stack_memory_manager_copy_to_buf(
        device->memory_manager, outgoing_packet->nx_packet_prepend_ptr, outgoing_packet->nx_packet_length, buffer);

    device->busy_transmitting = true;
    mdh_emac_status_t transmit_outcome = mdh_emac_transmit(device->get_emac_fn(), buffer);

    if (MDH_EMAC_STATUS_NO_ERROR != transmit_outcome) {
        device->busy_transmitting = false;
    }

    return (transmit_outcome == MDH_EMAC_STATUS_NO_ERROR) ? NX_SUCCESS : NX_DRIVER_ERROR;
}

static UINT hardware_receive_packet(NX_IP_DRIVER *nx_ip_driver, device_driver_t *device)
{
    mdh_network_stack_buffer_t *buffer =
        mdh_network_stack_memory_manager_alloc_from_heap(device->memory_manager, device->mtu, device->memory_alignment);

    if (buffer == NULL) {
        return NX_DRIVER_ERROR;
    }

    if (mdh_emac_receive(device->get_emac_fn(), buffer) != MDH_EMAC_STATUS_NO_ERROR) {
        mdh_network_stack_memory_manager_free(device->memory_manager, buffer);
        return NX_DRIVER_ERROR;
    }

    NX_PACKET *incoming_packet = {0};
    if (nx_packet_allocate(device->incomming_packet_pool, &incoming_packet, NX_RECEIVE_PACKET, NX_NO_WAIT)
        != NX_SUCCESS) {
        mdh_network_stack_memory_manager_free(device->memory_manager, buffer);
        return NX_DRIVER_ERROR;
    }

    incoming_packet->nx_packet_prepend_ptr += ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;

    if (mdh_network_stack_memory_manager_copy_from_buf(
            device->memory_manager, buffer, incoming_packet->nx_packet_prepend_ptr, device->mtu)
        == 0) {
        mdh_network_stack_memory_manager_free(device->memory_manager, buffer);
        nx_packet_release(incoming_packet);
        return NX_DRIVER_ERROR;
    }

    incoming_packet->nx_packet_length =
        mdh_network_stack_memory_manager_get_payload_len(device->memory_manager, buffer);
    incoming_packet->nx_packet_append_ptr = incoming_packet->nx_packet_prepend_ptr + incoming_packet->nx_packet_length;

    mdh_network_stack_memory_manager_free(device->memory_manager, buffer);

    if (transfer_to_netx(nx_ip_driver, incoming_packet) == false) {
        nx_packet_release(incoming_packet);
    }

    return NX_SUCCESS;
}

static void on_receive_complete(mdh_emac_t *self, void *ctx, mdh_emac_receive_t status)
{
    NX_PARAMETER_NOT_USED(self);
    NX_PARAMETER_NOT_USED(status);

    device_driver_t *device = (device_driver_t *)(ctx);
    device->deferred_event |= DEFERRED_EVENT_PACKET_RECEIVED;

    _nx_ip_driver_deferred_processing(device->nx_driver->ip);
}

static void
on_transmit_complete(mdh_emac_t *self, void *ctx, mdh_emac_transfer_t status, const mdh_network_stack_buffer_t *buffer)
{
    NX_PARAMETER_NOT_USED(self);
    NX_PARAMETER_NOT_USED(status);

    device_driver_t *device = (device_driver_t *)(ctx);

    device->busy_transmitting = false;

    // Restore the packet as it was when passed down by NetX Duo IP layer before releasing it.
    remove_nx_packet_ethernet_header(device->outgoing_packet);
    nx_packet_transmit_release(device->outgoing_packet);

    mdh_network_stack_memory_manager_free(device->memory_manager, buffer);
}

static void on_event(mdh_emac_t *self, void *ctx, mdh_emac_event_t event)
{
    NX_PARAMETER_NOT_USED(self);

    device_driver_t *device = (device_driver_t *)(ctx);
    if (event == MDH_EMAC_EVENT_LINK_STATUS_CHANGE) {
        // Infer link status change based on current link status
        if (device->nx_driver->ip->nx_ip_interface[0].nx_interface_link_up == NX_TRUE) {
            device->nx_driver->ip->nx_ip_interface[0].nx_interface_link_up = NX_FALSE;
        } else {
            device->nx_driver->ip->nx_ip_interface[0].nx_interface_link_up = NX_TRUE;
        }
    }
}

static void add_ethernet_header(NX_IP_DRIVER *nx_ip_driver, uint32_t *ethernet_frame_ptr)
{
    ethernet_frame_ptr[0] = nx_ip_driver->nx_ip_driver_physical_address_msw;
    ethernet_frame_ptr[1] = nx_ip_driver->nx_ip_driver_physical_address_lsw;
    ethernet_frame_ptr[2] = (nx_ip_driver->nx_ip_driver_interface->nx_interface_physical_address_msw << 16)
                            | (nx_ip_driver->nx_ip_driver_interface->nx_interface_physical_address_lsw >> 16);
    ethernet_frame_ptr[3] = (nx_ip_driver->nx_ip_driver_interface->nx_interface_physical_address_lsw << 16);

    if ((nx_ip_driver->nx_ip_driver_command == NX_LINK_ARP_SEND)
        || (nx_ip_driver->nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND)) {
        ethernet_frame_ptr[3] |= ETHER_PACKET_TYPE_ARP;
    } else if (nx_ip_driver->nx_ip_driver_command == NX_LINK_RARP_SEND) {
        ethernet_frame_ptr[3] |= ETHER_PACKET_TYPE_RARP;
    } else if (nx_ip_driver->nx_ip_driver_packet->nx_packet_ip_version == NX_IP_VERSION_V6) {
        ethernet_frame_ptr[3] |= ETHER_PACKET_TYPE_IPV6;
    } else {
        ethernet_frame_ptr[3] |= ETHER_PACKET_TYPE_IP;
    }

    /* Endian swapping if NX_LITTLE_ENDIAN is defined. */
    NX_CHANGE_ULONG_ENDIAN(ethernet_frame_ptr[0]);
    NX_CHANGE_ULONG_ENDIAN(ethernet_frame_ptr[1]);
    NX_CHANGE_ULONG_ENDIAN(ethernet_frame_ptr[2]);
    NX_CHANGE_ULONG_ENDIAN(ethernet_frame_ptr[3]);
}

static inline void remove_nx_packet_ethernet_header(NX_PACKET *packet)
{
    packet->nx_packet_prepend_ptr += ETHERNET_FRAME_SIZE;
    packet->nx_packet_length -= ETHERNET_FRAME_SIZE;
}
