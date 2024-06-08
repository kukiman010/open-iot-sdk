/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

extern "C" {
#include "hal/emac_api.h"
#include "hal/network_stack_memory_manager.h"
}

#include "iotsdk/nx_driver_mdh.h"
#include "fff.h"
#include "iotsdk_memory_manager.h"
#include "mps3_emac.h"
#include "nx_api.h"
#include "nx_arp.h"
#include "nx_ip.h"
#include "nx_rarp.h"
#include "tx_api.h"

#include "gtest/gtest.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

DEFINE_FFF_GLOBALS

typedef struct emac_memory_buf_s {
    void *allocated_memory; /**< Pointer to allocated buffer */
    void *data;             /**< Pointer to data (starting at aligned memory address) */
    uint32_t length;        /**< Consumed buffer length. */
} emac_memory_buf_t;

#define ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE 2U
#define ETHERNET_FRAME_SIZE                 14

#define ETHER_PACKET_TYPE_IP   0x0800
#define ETHER_PACKET_TYPE_IPV6 0x86dd
#define ETHER_PACKET_TYPE_ARP  0x0806
#define ETHER_PACKET_TYPE_RARP 0x8035

#define PACKET_COUNT     12
#define PACKET_SIZE      1536
#define PACKET_POOL_SIZE ((PACKET_SIZE + sizeof(NX_PACKET)) * PACKET_COUNT)

#define UDP_SOCKET_DATA "NetX Duo on Open IoT SDK!"

static mdh_emac_status_t get_ethernet_controller_mac_addr_custom_fake(const mdh_emac_t *const self, uint8_t *addr);
static mdh_emac_status_t power_up_ethernet_controller_custom_fake(const mdh_emac_t *self,
                                                                  mdh_emac_callbacks_t *cbks,
                                                                  mdh_network_stack_memory_manager_t *memory_manager,
                                                                  void *ctx);
static mdh_emac_status_t receive_ethernet_packet_custom_fake(mdh_emac_t *const self,
                                                             const mdh_network_stack_buffer_t *buf);
static UINT
nx_packet_allocate_custom_fake(NX_PACKET_POOL *pool_ptr, NX_PACKET **packet_ptr, ULONG packet_type, ULONG wait_option);

static mdh_network_stack_memory_manager_t *iotsdk_memory_manager_get_instance_custom_fake(void);
static mdh_network_stack_buffer_t *
alloc_from_heap_custom_fake(mdh_network_stack_memory_manager_t *const self, size_t size, size_t align);
static void free_buf_custom_fake(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *buf);
static size_t copy_to_buf_custom_fake(mdh_network_stack_memory_manager_t *const self,
                                      const void *const ptr,
                                      size_t size,
                                      const mdh_network_stack_buffer_t *buf);
static size_t copy_from_buf_custom_fake(mdh_network_stack_memory_manager_t *const self,
                                        const mdh_network_stack_buffer_t *const buf,
                                        void *const ptr,
                                        size_t size);
static void *get_payload_custom_fake(mdh_network_stack_memory_manager_t *const self,
                                     const mdh_network_stack_buffer_t *const buf);
static size_t get_payload_len_custom_fake(mdh_network_stack_memory_manager_t *const self,
                                          const mdh_network_stack_buffer_t *const buf);

static const mdh_emac_vtable_t gsc_vtable = {
    .power_up = power_up_ethernet_controller,
    .power_down = power_down_ethernet_controller,
    .get_mtu = get_ethernet_controller_mtu,
    .get_align = get_ethernet_controller_data_alignment_size,
    .get_interface_name = get_ethernet_controller_interface_name,
    .get_mac_addr_size = get_ethernet_controller_mac_addr_size,
    .get_mac_addr = get_ethernet_controller_mac_addr,
    .set_mac_addr = set_ethernet_controller_mac_addr,
    .add_to_multicast_group = add_to_multicast_group_ethernet_controller,
    .remove_from_multicast_group = remove_from_multicast_group_ethernet_controller,
    .config_multicast_reception = config_multicast_reception_ethernet_controller,
    .transmit = transmit_ethernet_controller,
    .receive = receive_ethernet_controller};

static const mdh_network_stack_memory_manager_vtable_t gsc_mem_mgr_vtable = {
    .alloc_from_heap = net_stack_mem_mgr_alloc_from_heap,
    .alloc_from_static_pool = NULL,
    .get_static_pool_alloc_unit = NULL,
    .free = net_stack_mem_mgr_free_buf,
    .copy = NULL,
    .copy_to_buf = net_stack_mem_mgr_copy_to_buf,
    .copy_from_buf = net_stack_mem_mgr_copy_from_buf,
    .get_payload = net_stack_mem_mgr_get_payload,
    .get_payload_len = net_stack_mem_mgr_get_payload_len,
    .set_payload_len = NULL,
};

static mdh_emac_t gs_ethernet_device;
static uint8_t gs_mac_address[6];
static uint8_t gs_packet_pool_buffer[PACKET_POOL_SIZE];
static size_t gs_mtu;
static mdh_emac_callbacks_t *gs_emac_cbks;
static void *gs_ctx;
static mdh_network_stack_memory_manager_t *gs_memory_manager;
static uint16_t gs_incomming_packet_eth_type;
static mdh_network_stack_memory_manager_t gsc_memory_manager;

class TestNxDriverMdh : public ::testing::Test {
public:
    TestNxDriverMdh()
    {
        RESET_FAKE(mdh_emac_get_default_instance);

        RESET_FAKE(power_up_ethernet_controller);
        RESET_FAKE(power_down_ethernet_controller);
        RESET_FAKE(get_ethernet_controller_mtu);
        RESET_FAKE(get_ethernet_controller_data_alignment_size);
        RESET_FAKE(get_ethernet_controller_interface_name);
        RESET_FAKE(get_ethernet_controller_mac_addr_size);
        RESET_FAKE(get_ethernet_controller_mac_addr);
        RESET_FAKE(set_ethernet_controller_mac_addr);
        RESET_FAKE(add_to_multicast_group_ethernet_controller);
        RESET_FAKE(remove_from_multicast_group_ethernet_controller);
        RESET_FAKE(config_multicast_reception_ethernet_controller);
        RESET_FAKE(transmit_ethernet_controller);
        RESET_FAKE(receive_ethernet_controller);

        RESET_FAKE(nx_ip_interface_mtu_set);
        RESET_FAKE(nx_ip_interface_physical_address_set);
        RESET_FAKE(nx_ip_interface_address_mapping_configure);
        RESET_FAKE(nx_packet_release);
        RESET_FAKE(nx_packet_transmit_release);
        RESET_FAKE(nx_packet_allocate);
        RESET_FAKE(_nx_ip_driver_deferred_processing);

        RESET_FAKE(_nx_arp_packet_deferred_receive);
        RESET_FAKE(_nx_rarp_packet_deferred_receive);
        RESET_FAKE(_nx_ip_packet_deferred_receive);

        RESET_FAKE(tx_thread_preemption_change);
        RESET_FAKE(tx_thread_identify);
        RESET_FAKE(_tx_thread_interrupt_disable);
        RESET_FAKE(_tx_thread_interrupt_restore);

        RESET_FAKE(iotsdk_memory_manager_get_instance);
        RESET_FAKE(net_stack_mem_mgr_alloc_from_heap);
        RESET_FAKE(net_stack_mem_mgr_free_buf);
        RESET_FAKE(net_stack_mem_mgr_copy_to_buf);
        RESET_FAKE(net_stack_mem_mgr_copy_from_buf);
        RESET_FAKE(net_stack_mem_mgr_get_payload);
        RESET_FAKE(net_stack_mem_mgr_get_payload_len);

        iotsdk_memory_manager_get_instance_fake.custom_fake = iotsdk_memory_manager_get_instance_custom_fake;

        gs_ethernet_device.vtable = &gsc_vtable;
        memset(gs_mac_address, 0xFFU, sizeof(gs_mac_address));
        memset(gs_packet_pool_buffer, 0, sizeof(gs_packet_pool_buffer));
        gs_mtu = 1518U;
        gs_emac_cbks = nullptr;
        gs_ctx = nullptr;
        gs_memory_manager = nullptr;
        gs_incomming_packet_eth_type = UINT16_MAX;
        gsc_memory_manager.vtable = &gsc_mem_mgr_vtable;
    }
};

class TestNxDriverMdhInit : public TestNxDriverMdh {
public:
    TestNxDriverMdhInit() : TestNxDriverMdh()
    {
        initialize_attach_and_enable();
    }

private:
    void initialize_attach_and_enable(void)
    {
        get_ethernet_controller_mtu_fake.return_val = gs_mtu;
        mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
        power_up_ethernet_controller_fake.custom_fake = power_up_ethernet_controller_custom_fake;

        struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
        NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_FALSE};
        NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_INITIALIZE,
                                     .nx_ip_driver_status = NX_DRIVER_ERROR,
                                     .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                     .nx_ip_driver_interface = &nx_ip_driver_interface};
        nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
        EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

        nx_ip_driver.nx_ip_driver_command = NX_LINK_INTERFACE_ATTACH;
        nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

        nx_ip_driver.nx_ip_driver_command = NX_LINK_ENABLE;
        nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
        EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
        EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_TRUE);
    }
};

TEST_F(TestNxDriverMdh, hw_power_up_failure_fails_ip_driver_init)
{
    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    power_up_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_INTERNAL_ERROR;

    NX_IP_DRIVER nx_ip_driver = {
        .nx_ip_driver_command = NX_LINK_INITIALIZE,
        .nx_ip_driver_status = NX_SUCCESS,
    };

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
}

TEST_F(TestNxDriverMdh, failure_to_get_hw_mac_address_fails_ip_driver_init)
{
    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    power_up_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    get_ethernet_controller_mac_addr_fake.return_val = MDH_EMAC_STATUS_INTERNAL_ERROR;

    NX_IP_DRIVER nx_ip_driver = {
        .nx_ip_driver_command = NX_LINK_INITIALIZE,
        .nx_ip_driver_status = NX_SUCCESS,
    };

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
}

TEST_F(TestNxDriverMdh, hw_power_up_success_succeeds_ip_driver_init)
{
    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    power_up_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    get_ethernet_controller_mac_addr_fake.custom_fake = get_ethernet_controller_mac_addr_custom_fake;

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    UCHAR nx_interface_index = 67U;
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_index = nx_interface_index};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_INITIALIZE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdh, hw_power_up_success_succeeds_sets_ip_interface_mtu)
{
    const size_t mtu = 1518U;

    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    power_up_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    get_ethernet_controller_mtu_fake.return_val = mtu;
    get_ethernet_controller_mac_addr_fake.custom_fake = get_ethernet_controller_mac_addr_custom_fake;

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    UCHAR nx_interface_index = 67U;
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_index = nx_interface_index};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_INITIALIZE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

    EXPECT_EQ(nx_ip_interface_mtu_set_fake.arg0_val, &nx_ip_driver_ptr);
    EXPECT_EQ(nx_ip_interface_mtu_set_fake.arg1_val, nx_interface_index);
    EXPECT_EQ(nx_ip_interface_mtu_set_fake.arg2_val, (mtu - ETHERNET_FRAME_SIZE));
}

TEST_F(TestNxDriverMdh, hw_power_up_success_succeeds_sets_ip_interface_mac)
{
    gs_mac_address[0] = 0xDE;
    gs_mac_address[1] = 0xAD;
    gs_mac_address[2] = 0xBE;
    gs_mac_address[3] = 0xEF;
    gs_mac_address[4] = 0xBA;
    gs_mac_address[5] = 0xBE;

    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    power_up_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    get_ethernet_controller_mac_addr_fake.custom_fake = get_ethernet_controller_mac_addr_custom_fake;

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    UCHAR nx_interface_index = 67U;
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_index = nx_interface_index};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_INITIALIZE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

    EXPECT_EQ(nx_ip_interface_physical_address_set_fake.arg0_val, &nx_ip_driver_ptr);
    EXPECT_EQ(nx_ip_interface_physical_address_set_fake.arg1_val, nx_interface_index);
    EXPECT_EQ(nx_ip_interface_physical_address_set_fake.arg2_val,
              (uint32_t)((gs_mac_address[0] << 8) | (gs_mac_address[1])));
    EXPECT_EQ(nx_ip_interface_physical_address_set_fake.arg3_val,
              (uint32_t)((gs_mac_address[2] << 24) | (gs_mac_address[3] << 16) | (gs_mac_address[4] << 8)
                         | (gs_mac_address[5])));
    EXPECT_EQ(nx_ip_interface_physical_address_set_fake.arg4_val, NX_FALSE);
}

TEST_F(TestNxDriverMdh, hw_power_up_success_succeeds_sets_ip_interface_address_mapping)
{
    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    power_up_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    get_ethernet_controller_mac_addr_fake.custom_fake = get_ethernet_controller_mac_addr_custom_fake;

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    UCHAR nx_interface_index = 67U;
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_index = nx_interface_index};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_INITIALIZE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

    EXPECT_EQ(nx_ip_interface_address_mapping_configure_fake.arg0_val, &nx_ip_driver_ptr);
    EXPECT_EQ(nx_ip_interface_address_mapping_configure_fake.arg1_val, nx_interface_index);
    EXPECT_EQ(nx_ip_interface_address_mapping_configure_fake.arg2_val, NX_TRUE);
}

TEST_F(TestNxDriverMdh, enabling_uninitialized_ip_driver_fails)
{
    NX_IP_DRIVER nx_ip_driver = {
        .nx_ip_driver_command = NX_LINK_ENABLE,
        .nx_ip_driver_status = NX_SUCCESS,
    };

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
}

TEST_F(TestNxDriverMdh, failure_to_enable_hw_of_initialized_ip_driver_indicates_link_down)
{
    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    // `power_up_ethernet_controller()` is called at initialization as well as to enable the interface.
    const uint8_t power_up_max_call_number = 2;
    mdh_emac_status_t power_up_return_values[power_up_max_call_number] = {
        MDH_EMAC_STATUS_NO_ERROR,      // Successful at initialization
        MDH_EMAC_STATUS_INTERNAL_ERROR // Failure at interface enablement
    };
    SET_RETURN_SEQ(power_up_ethernet_controller, power_up_return_values, power_up_max_call_number);

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    UCHAR nx_interface_index = 67U;
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_FALSE, .nx_interface_index = nx_interface_index};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_INITIALIZE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

    nx_ip_driver.nx_ip_driver_command = NX_LINK_ENABLE;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_FALSE);
}

TEST_F(TestNxDriverMdh, enabling_initialized_ip_driver_indicates_link_up)
{
    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    power_up_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    UCHAR nx_interface_index = 67U;
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_FALSE, .nx_interface_index = nx_interface_index};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_INITIALIZE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

    nx_ip_driver.nx_ip_driver_command = NX_LINK_ENABLE;

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_TRUE);
}

TEST_F(TestNxDriverMdh, enabling_already_enabled_ip_driver_fails)
{
    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    power_up_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    UCHAR nx_interface_index = 67U;
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_FALSE, .nx_interface_index = nx_interface_index};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_INITIALIZE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

    nx_ip_driver.nx_ip_driver_command = NX_LINK_ENABLE;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_TRUE);

    nx_ip_driver.nx_ip_driver_command = NX_LINK_ENABLE;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_ALREADY_ENABLED);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_TRUE);
}

TEST_F(TestNxDriverMdhInit, reenabling_disabled_ip_driver_succeeds)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DISABLE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    power_down_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_FALSE);

    nx_ip_driver.nx_ip_driver_command = NX_LINK_ENABLE;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_TRUE);
}

TEST_F(TestNxDriverMdhInit, disabling_enabled_ip_driver_indicates_link_down)
{

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DISABLE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_ip_driver.nx_ip_driver_command = NX_LINK_DISABLE;
    power_down_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_FALSE);
}

TEST_F(TestNxDriverMdhInit, failure_to_disable_enabled_ip_driver_fails_leaves_unmodified_link_status)
{

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DISABLE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    power_down_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_INTERNAL_ERROR;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_TRUE);
}

TEST_F(TestNxDriverMdh, disable_when_link_down_leaves_unmodified_link_status)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_FALSE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DISABLE,
                                 .nx_ip_driver_status = NX_SUCCESS,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_FALSE);
}

TEST_F(TestNxDriverMdh, current_link_status_is_accurately_reported)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {.nx_ip_interface = {{
                                                .nx_interface_link_up = 0xDE // This member is what should be returned.
                                            }}};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_FALSE};
    ULONG link_status = 0xBEEFBABE;
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_GET_STATUS,
                                 .nx_ip_driver_status = NX_SUCCESS,
                                 .nx_ip_driver_return_ptr = &link_status,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_ptr->nx_ip_interface[0].nx_interface_link_up, link_status);
}

TEST_F(TestNxDriverMdh, multicast_join_when_link_down_fails)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {0};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_MULTICAST_JOIN,
                                 .nx_ip_driver_status = NX_SUCCESS,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(add_to_multicast_group_ethernet_controller_fake.call_count, 0);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_NOT_ENABLED);
}

TEST_F(TestNxDriverMdh, multicast_leave_when_link_down_fails)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {0};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_MULTICAST_LEAVE,
                                 .nx_ip_driver_status = NX_SUCCESS,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(remove_from_multicast_group_ethernet_controller_fake.call_count, 0);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_NOT_ENABLED);
}

TEST_F(TestNxDriverMdhInit, multicast_join_enabled_ip_driver_succeeds)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_MULTICAST_JOIN,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_ip_driver.nx_ip_driver_command = NX_LINK_MULTICAST_JOIN;
    add_to_multicast_group_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(add_to_multicast_group_ethernet_controller_fake.call_count, 1U);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdhInit, multicast_leave_enabled_ip_driver_succeeds)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_MULTICAST_LEAVE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    remove_from_multicast_group_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(remove_from_multicast_group_ethernet_controller_fake.call_count, 1U);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdhInit, multicast_join_enabled_ip_driver_fails_with_hw_error)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_MULTICAST_JOIN,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    add_to_multicast_group_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_INTERNAL_ERROR;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(add_to_multicast_group_ethernet_controller_fake.call_count, 1U);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_TRUE);
}

TEST_F(TestNxDriverMdhInit, multicast_leave_enabled_ip_driver_fails_with_hw_error)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_MULTICAST_LEAVE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    remove_from_multicast_group_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_INTERNAL_ERROR;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(remove_from_multicast_group_ethernet_controller_fake.call_count, 1U);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_TRUE);
}

TEST_F(TestNxDriverMdh, send_when_link_down_fails)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_FALSE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_PACKET_SEND,
                                 .nx_ip_driver_status = NX_SUCCESS,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(transmit_ethernet_controller_fake.call_count, 0);
    EXPECT_EQ(nx_packet_transmit_release_fake.call_count, 0);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_NOT_ENABLED);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_FALSE);
}

TEST_F(TestNxDriverMdhInit, ethernet_header_correctly_formed)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_copy_to_buf_fake.custom_fake = copy_to_buf_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    tx_thread_preemption_change_fake.return_val = TX_SUCCESS;
    transmit_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    tx_thread_identify_fake.return_val = nullptr;

    UCHAR *payload_location = gs_packet_pool_buffer + ETHERNET_FRAME_SIZE + ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;
    NX_PACKET outgoing_packet = {.nx_packet_prepend_ptr = payload_location,
                                 .nx_packet_append_ptr = payload_location + gs_mtu - ETHERNET_FRAME_SIZE
                                                         - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE + 1,
                                 .nx_packet_length = gs_mtu - ETHERNET_FRAME_SIZE};
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE,
                                           .nx_interface_physical_address_msw = 0xFFFF99AA,
                                           .nx_interface_physical_address_lsw = 0xBBCCDDEE};
    NX_IP_DRIVER nx_ip_driver = {
        .nx_ip_driver_command = NX_LINK_PACKET_SEND,
        .nx_ip_driver_status = NX_DRIVER_ERROR,
        .nx_ip_driver_physical_address_msw = 0x11223344,
        .nx_ip_driver_physical_address_lsw = 0x55667788,
        .nx_ip_driver_packet = &outgoing_packet,
        .nx_ip_driver_ptr = &nx_ip_driver_ptr,
        .nx_ip_driver_interface = &nx_ip_driver_interface,
    };

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    const uint8_t ethernet_header[ETHERNET_FRAME_SIZE] = {// Destination
                                                          0x44,
                                                          0x33,
                                                          0x22,
                                                          0x11,
                                                          0x88,
                                                          0x77,
                                                          // Source
                                                          0x66,
                                                          0x55,
                                                          0xCC,
                                                          0xBB,
                                                          0xAA,
                                                          0x99,
                                                          // Ether type
                                                          0x00,
                                                          0x08};

    const void *buf =
        static_cast<const uint8_t *>(net_stack_mem_mgr_copy_to_buf_fake.arg1_val) - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;
    EXPECT_EQ(0, memcmp(buf, ethernet_header, ETHERNET_FRAME_SIZE));
}

TEST_F(TestNxDriverMdhInit, outgoing_packet_exceeding_mtu_fails)
{
    UCHAR *payload_location = gs_packet_pool_buffer + ETHERNET_FRAME_SIZE + ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;
    NX_PACKET outgoing_packet = {
        .nx_packet_prepend_ptr = payload_location,
        .nx_packet_append_ptr =
            payload_location + gs_mtu + 1 - ETHERNET_FRAME_SIZE - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE + 1,
        .nx_packet_length = gs_mtu + 1 - ETHERNET_FRAME_SIZE,
    };
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_PACKET_SEND,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = &outgoing_packet,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(transmit_ethernet_controller_fake.call_count, 0);
    EXPECT_EQ(nx_packet_transmit_release_fake.call_count, 0);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
}

TEST_F(TestNxDriverMdhInit, outgoing_ip_packet_up_to_mtu_succeeds)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_copy_to_buf_fake.custom_fake = copy_to_buf_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    tx_thread_preemption_change_fake.return_val = TX_SUCCESS;
    transmit_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    tx_thread_identify_fake.return_val = nullptr;

    UCHAR *payload_location = gs_packet_pool_buffer + ETHERNET_FRAME_SIZE + ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;
    NX_PACKET outgoing_packet = {.nx_packet_prepend_ptr = payload_location,
                                 .nx_packet_append_ptr = payload_location + gs_mtu - ETHERNET_FRAME_SIZE
                                                         - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE + 1,
                                 .nx_packet_length = gs_mtu - ETHERNET_FRAME_SIZE};
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_PACKET_SEND,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = &outgoing_packet,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdhInit, outgoing_arp_packet_up_to_mtu_succeeds)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_copy_to_buf_fake.custom_fake = copy_to_buf_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    tx_thread_preemption_change_fake.return_val = TX_SUCCESS;
    transmit_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    tx_thread_identify_fake.return_val = nullptr;

    UCHAR *payload_location = gs_packet_pool_buffer + ETHERNET_FRAME_SIZE + ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;
    NX_PACKET outgoing_packet = {.nx_packet_prepend_ptr = payload_location,
                                 .nx_packet_append_ptr = payload_location + gs_mtu - ETHERNET_FRAME_SIZE
                                                         - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE + 1,
                                 .nx_packet_length = gs_mtu - ETHERNET_FRAME_SIZE};
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_ARP_SEND,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = &outgoing_packet,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdhInit, outgoing_rarp_packet_up_to_mtu_succeeds)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_copy_to_buf_fake.custom_fake = copy_to_buf_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    tx_thread_preemption_change_fake.return_val = TX_SUCCESS;
    transmit_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    tx_thread_identify_fake.return_val = nullptr;

    UCHAR *payload_location = gs_packet_pool_buffer + ETHERNET_FRAME_SIZE + ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;
    NX_PACKET outgoing_packet = {.nx_packet_prepend_ptr = payload_location,
                                 .nx_packet_append_ptr = payload_location + gs_mtu - ETHERNET_FRAME_SIZE
                                                         - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE + 1,
                                 .nx_packet_length = gs_mtu - ETHERNET_FRAME_SIZE};
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_RARP_SEND,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = &outgoing_packet,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdhInit, outgoing_ipv6_packet_up_to_mtu_succeeds)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_copy_to_buf_fake.custom_fake = copy_to_buf_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    tx_thread_preemption_change_fake.return_val = TX_SUCCESS;
    transmit_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    tx_thread_identify_fake.return_val = nullptr;

    UCHAR *payload_location = gs_packet_pool_buffer + ETHERNET_FRAME_SIZE + ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;
    NX_PACKET outgoing_packet = {.nx_packet_prepend_ptr = payload_location,
                                 .nx_packet_append_ptr = payload_location + gs_mtu - ETHERNET_FRAME_SIZE
                                                         - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE + 1,
                                 .nx_packet_length = gs_mtu - ETHERNET_FRAME_SIZE,
                                 .nx_packet_ip_version = NX_IP_VERSION_V6};
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_PACKET_SEND,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = &outgoing_packet,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdh, unhandled_command_reported_as_failure)
{
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = UINT32_MAX, .nx_ip_driver_status = NX_DRIVER_ERROR};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_UNHANDLED_COMMAND);
}

TEST_F(TestNxDriverMdhInit, hw_tx_error_fails_ip_driver_send)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_copy_to_buf_fake.custom_fake = copy_to_buf_custom_fake;
    tx_thread_preemption_change_fake.return_val = TX_SUCCESS;
    transmit_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_INTERNAL_ERROR;
    tx_thread_identify_fake.return_val = nullptr;

    UCHAR *payload_location = gs_packet_pool_buffer + ETHERNET_FRAME_SIZE + ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;
    NX_PACKET outgoing_packet = {.nx_packet_prepend_ptr = payload_location,
                                 .nx_packet_append_ptr = payload_location + gs_mtu - ETHERNET_FRAME_SIZE
                                                         - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE + 1,
                                 .nx_packet_length = gs_mtu - ETHERNET_FRAME_SIZE};
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_PACKET_SEND,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = &outgoing_packet,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(transmit_ethernet_controller_fake.call_count, 1);
    EXPECT_EQ(nx_packet_transmit_release_fake.call_count, 0);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
}

TEST_F(TestNxDriverMdhInit, no_memory_for_outgoing_packet_fails_ip_driver_send)
{
    net_stack_mem_mgr_alloc_from_heap_fake.return_val = NULL;
    tx_thread_preemption_change_fake.return_val = TX_SUCCESS;
    transmit_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_INTERNAL_ERROR;
    tx_thread_identify_fake.return_val = nullptr;

    UCHAR *payload_location = gs_packet_pool_buffer + ETHERNET_FRAME_SIZE + ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;
    NX_PACKET outgoing_packet = {.nx_packet_prepend_ptr = payload_location,
                                 .nx_packet_append_ptr = payload_location + gs_mtu - ETHERNET_FRAME_SIZE
                                                         - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE + 1,
                                 .nx_packet_length = gs_mtu - ETHERNET_FRAME_SIZE};
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_PACKET_SEND,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = &outgoing_packet,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(transmit_ethernet_controller_fake.call_count, 0);
    EXPECT_EQ(nx_packet_transmit_release_fake.call_count, 0);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
}

TEST_F(TestNxDriverMdhInit, send_two_packets_subsequently_succeeds)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_copy_to_buf_fake.custom_fake = copy_to_buf_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    tx_thread_preemption_change_fake.return_val = TX_SUCCESS;
    transmit_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_NO_ERROR;
    tx_thread_identify_fake.return_val = nullptr;

    UCHAR *payload_location = gs_packet_pool_buffer + ETHERNET_FRAME_SIZE + ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE;
    NX_PACKET outgoing_packet = {.nx_packet_prepend_ptr = payload_location,
                                 .nx_packet_append_ptr = payload_location + gs_mtu - ETHERNET_FRAME_SIZE
                                                         - ETHERNET_FRAME_ALIGNMENT_BYTES_SIZE + 1,
                                 .nx_packet_length = gs_mtu - ETHERNET_FRAME_SIZE};
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_PACKET_SEND,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = &outgoing_packet,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    gs_emac_cbks->tx(&gs_ethernet_device, gs_ctx, MDH_EMAC_TRANSFER_DONE, nullptr);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdhInit, incoming_packet_processing_is_successfully_deferred)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    net_stack_mem_mgr_copy_from_buf_fake.custom_fake = copy_from_buf_custom_fake;
    net_stack_mem_mgr_get_payload_len_fake.custom_fake = get_payload_len_custom_fake;

    // We are only interested to see if there is an indication that a packet
    // was received, we do not want to handle the packet in this test case.
    receive_ethernet_controller_fake.return_val = MDH_EMAC_STATUS_INTERNAL_ERROR;

    // The call back should indicate that there is packet pending
    gs_emac_cbks->rx(&gs_ethernet_device, gs_ctx, MDH_EMAC_RECEIVE_ERROR);

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DEFERRED_PROCESSING,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = nullptr,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    // Reception will only occur if a packet is pending
    EXPECT_EQ(receive_ethernet_controller_fake.call_count, 1);
}

TEST_F(TestNxDriverMdhInit, no_deferred_processing_if_no_packet_pending)
{
    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DEFERRED_PROCESSING,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = nullptr,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    // Reception of packet only if notified
    EXPECT_EQ(receive_ethernet_controller_fake.call_count, 0);
}

TEST_F(TestNxDriverMdhInit, processing_of_ip_packet_succeeds)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    net_stack_mem_mgr_copy_from_buf_fake.custom_fake = copy_from_buf_custom_fake;
    net_stack_mem_mgr_get_payload_len_fake.custom_fake = get_payload_len_custom_fake;
    net_stack_mem_mgr_get_payload_fake.custom_fake = get_payload_custom_fake;

    gs_incomming_packet_eth_type = ETHER_PACKET_TYPE_IP;
    receive_ethernet_controller_fake.custom_fake = receive_ethernet_packet_custom_fake;
    nx_packet_allocate_fake.custom_fake = nx_packet_allocate_custom_fake;

    gs_emac_cbks->rx(&gs_ethernet_device, gs_ctx, MDH_EMAC_RECEIVE_DONE);

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DEFERRED_PROCESSING,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = nullptr,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(receive_ethernet_controller_fake.call_count, 1);
    EXPECT_EQ(_nx_ip_packet_deferred_receive_fake.call_count, 1);
    EXPECT_EQ(nx_packet_release_fake.call_count, 0);
}

TEST_F(TestNxDriverMdhInit, receive_ip_type_no_pool_space_for_packet)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    net_stack_mem_mgr_copy_from_buf_fake.custom_fake = copy_from_buf_custom_fake;
    net_stack_mem_mgr_get_payload_len_fake.custom_fake = get_payload_len_custom_fake;
    net_stack_mem_mgr_get_payload_fake.custom_fake = get_payload_custom_fake;

    gs_incomming_packet_eth_type = ETHER_PACKET_TYPE_IP;
    receive_ethernet_controller_fake.custom_fake = receive_ethernet_packet_custom_fake;
    nx_packet_allocate_fake.return_val = NX_NO_PACKET;

    gs_emac_cbks->rx(&gs_ethernet_device, gs_ctx, MDH_EMAC_RECEIVE_DONE);

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DEFERRED_PROCESSING,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = nullptr,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(receive_ethernet_controller_fake.call_count, 1);
    EXPECT_EQ(_nx_ip_packet_deferred_receive_fake.call_count, 0);
    EXPECT_EQ(nx_packet_release_fake.call_count, 0);
}

TEST_F(TestNxDriverMdhInit, processing_of_deferred_ipv6_packet_type_succeeds)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    net_stack_mem_mgr_copy_from_buf_fake.custom_fake = copy_from_buf_custom_fake;
    net_stack_mem_mgr_get_payload_len_fake.custom_fake = get_payload_len_custom_fake;
    net_stack_mem_mgr_get_payload_fake.custom_fake = get_payload_custom_fake;

    gs_incomming_packet_eth_type = ETHER_PACKET_TYPE_IPV6;
    receive_ethernet_controller_fake.custom_fake = receive_ethernet_packet_custom_fake;
    nx_packet_allocate_fake.custom_fake = nx_packet_allocate_custom_fake;

    gs_emac_cbks->rx(&gs_ethernet_device, gs_ctx, MDH_EMAC_RECEIVE_DONE);

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DEFERRED_PROCESSING,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = nullptr,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(receive_ethernet_controller_fake.call_count, 1);
    EXPECT_EQ(_nx_ip_packet_deferred_receive_fake.call_count, 1);
    EXPECT_EQ(nx_packet_release_fake.call_count, 0);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdhInit, processing_of_deferred_arp_packet_type_succeeds)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    net_stack_mem_mgr_copy_from_buf_fake.custom_fake = copy_from_buf_custom_fake;
    net_stack_mem_mgr_get_payload_len_fake.custom_fake = get_payload_len_custom_fake;
    net_stack_mem_mgr_get_payload_fake.custom_fake = get_payload_custom_fake;

    gs_incomming_packet_eth_type = ETHER_PACKET_TYPE_ARP;
    receive_ethernet_controller_fake.custom_fake = receive_ethernet_packet_custom_fake;
    nx_packet_allocate_fake.custom_fake = nx_packet_allocate_custom_fake;

    gs_emac_cbks->rx(&gs_ethernet_device, gs_ctx, MDH_EMAC_RECEIVE_DONE);

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DEFERRED_PROCESSING,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = nullptr,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(receive_ethernet_controller_fake.call_count, 1);
    EXPECT_EQ(_nx_arp_packet_deferred_receive_fake.call_count, 1);
    EXPECT_EQ(nx_packet_release_fake.call_count, 0);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdhInit, processing_of_deferred_rarp_packet_type_succeeds)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    net_stack_mem_mgr_copy_from_buf_fake.custom_fake = copy_from_buf_custom_fake;
    net_stack_mem_mgr_get_payload_len_fake.custom_fake = get_payload_len_custom_fake;
    net_stack_mem_mgr_get_payload_fake.custom_fake = get_payload_custom_fake;

    gs_incomming_packet_eth_type = ETHER_PACKET_TYPE_RARP;
    receive_ethernet_controller_fake.custom_fake = receive_ethernet_packet_custom_fake;
    nx_packet_allocate_fake.custom_fake = nx_packet_allocate_custom_fake;

    gs_emac_cbks->rx(&gs_ethernet_device, gs_ctx, MDH_EMAC_RECEIVE_DONE);

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DEFERRED_PROCESSING,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = nullptr,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(receive_ethernet_controller_fake.call_count, 1);
    EXPECT_EQ(_nx_rarp_packet_deferred_receive_fake.call_count, 1);
    EXPECT_EQ(nx_packet_release_fake.call_count, 0);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdhInit, processing_of_deferred_unknown_packet_type_succeeds)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    net_stack_mem_mgr_copy_from_buf_fake.custom_fake = copy_from_buf_custom_fake;
    net_stack_mem_mgr_get_payload_len_fake.custom_fake = get_payload_len_custom_fake;
    net_stack_mem_mgr_get_payload_fake.custom_fake = get_payload_custom_fake;

    gs_incomming_packet_eth_type = UINT16_MAX;
    receive_ethernet_controller_fake.custom_fake = receive_ethernet_packet_custom_fake;
    nx_packet_allocate_fake.custom_fake = nx_packet_allocate_custom_fake;

    gs_emac_cbks->rx(&gs_ethernet_device, gs_ctx, MDH_EMAC_RECEIVE_DONE);

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DEFERRED_PROCESSING,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = nullptr,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(receive_ethernet_controller_fake.call_count, 1);
    EXPECT_EQ(nx_packet_release_fake.call_count, 1);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
}

TEST_F(TestNxDriverMdhInit, no_memory_to_retrieve_packet_fails_deferred_processing)
{
    net_stack_mem_mgr_alloc_from_heap_fake.return_val = NULL;

    gs_incomming_packet_eth_type = UINT16_MAX;
    receive_ethernet_controller_fake.custom_fake = receive_ethernet_packet_custom_fake;
    nx_packet_allocate_fake.custom_fake = nx_packet_allocate_custom_fake;

    gs_emac_cbks->rx(&gs_ethernet_device, gs_ctx, MDH_EMAC_RECEIVE_DONE);

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DEFERRED_PROCESSING,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = nullptr,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
}

TEST_F(TestNxDriverMdhInit, failure_to_copy_received_packet_to_ip_driver_results_in_deferred_processing_error)
{
    net_stack_mem_mgr_alloc_from_heap_fake.custom_fake = alloc_from_heap_custom_fake;
    net_stack_mem_mgr_free_buf_fake.custom_fake = free_buf_custom_fake;
    net_stack_mem_mgr_copy_from_buf_fake.return_val = 0;
    net_stack_mem_mgr_get_payload_len_fake.custom_fake = get_payload_len_custom_fake;
    net_stack_mem_mgr_get_payload_fake.custom_fake = get_payload_custom_fake;

    gs_incomming_packet_eth_type = UINT16_MAX;
    receive_ethernet_controller_fake.custom_fake = receive_ethernet_packet_custom_fake;
    nx_packet_allocate_fake.custom_fake = nx_packet_allocate_custom_fake;

    gs_emac_cbks->rx(&gs_ethernet_device, gs_ctx, MDH_EMAC_RECEIVE_DONE);

    struct NX_IP_STRUCT nx_ip_driver_ptr = {0};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_TRUE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_DEFERRED_PROCESSING,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_packet = nullptr,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};

    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(receive_ethernet_controller_fake.call_count, 1);
    EXPECT_EQ(nx_packet_release_fake.call_count, 1);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_DRIVER_ERROR);
}

TEST_F(TestNxDriverMdh, on_link_down_event_sets_link_status_down)
{
    get_ethernet_controller_mtu_fake.return_val = gs_mtu;
    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    power_up_ethernet_controller_fake.custom_fake = power_up_ethernet_controller_custom_fake;

    struct NX_IP_STRUCT nx_ip_driver_ptr = {.nx_ip_interface = {{
                                                .nx_interface_link_up = NX_TRUE // State before the event.
                                            }}};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_FALSE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_INITIALIZE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

    nx_ip_driver.nx_ip_driver_command = NX_LINK_INTERFACE_ATTACH;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    nx_ip_driver.nx_ip_driver_command = NX_LINK_ENABLE;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_TRUE);

    gs_emac_cbks->event(&gs_ethernet_device, gs_ctx, MDH_EMAC_EVENT_LINK_STATUS_CHANGE);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_ptr->nx_ip_interface[0].nx_interface_link_up, NX_FALSE);
}

TEST_F(TestNxDriverMdh, on_link_up_event_sets_link_status_up)
{
    get_ethernet_controller_mtu_fake.return_val = gs_mtu;
    mdh_emac_get_default_instance_fake.return_val = &gs_ethernet_device;
    power_up_ethernet_controller_fake.custom_fake = power_up_ethernet_controller_custom_fake;

    struct NX_IP_STRUCT nx_ip_driver_ptr = {.nx_ip_interface = {{
                                                .nx_interface_link_up = NX_FALSE // State before the event.
                                            }}};
    NX_INTERFACE nx_ip_driver_interface = {.nx_interface_link_up = NX_FALSE};
    NX_IP_DRIVER nx_ip_driver = {.nx_ip_driver_command = NX_LINK_INITIALIZE,
                                 .nx_ip_driver_status = NX_DRIVER_ERROR,
                                 .nx_ip_driver_ptr = &nx_ip_driver_ptr,
                                 .nx_ip_driver_interface = &nx_ip_driver_interface};
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);

    nx_ip_driver.nx_ip_driver_command = NX_LINK_INTERFACE_ATTACH;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);

    nx_ip_driver.nx_ip_driver_command = NX_LINK_ENABLE;
    nx_driver_mdh_ip_link_fsm(&nx_ip_driver);
    EXPECT_EQ(nx_ip_driver.nx_ip_driver_status, NX_SUCCESS);
    EXPECT_EQ(nx_ip_driver_interface.nx_interface_link_up, NX_TRUE);

    gs_emac_cbks->event(&gs_ethernet_device, gs_ctx, MDH_EMAC_EVENT_LINK_STATUS_CHANGE);

    EXPECT_EQ(nx_ip_driver.nx_ip_driver_ptr->nx_ip_interface[0].nx_interface_link_up, NX_TRUE);
}

static mdh_emac_status_t get_ethernet_controller_mac_addr_custom_fake(const mdh_emac_t *const self, uint8_t *addr)
{
    memcpy(addr, gs_mac_address, sizeof(gs_mac_address));
    return MDH_EMAC_STATUS_NO_ERROR;
}

static mdh_emac_status_t power_up_ethernet_controller_custom_fake(const mdh_emac_t *self,
                                                                  mdh_emac_callbacks_t *cbks,
                                                                  mdh_network_stack_memory_manager_t *memory_manager,
                                                                  void *ctx)
{
    gs_emac_cbks = cbks;
    gs_ctx = ctx;
    gs_memory_manager = memory_manager;
    return MDH_EMAC_STATUS_NO_ERROR;
}

static mdh_emac_status_t receive_ethernet_packet_custom_fake(mdh_emac_t *const self,
                                                             const mdh_network_stack_buffer_t *buf)
{
    uint8_t *buffer_data = static_cast<uint8_t *>(mdh_network_stack_memory_manager_get_payload(gs_memory_manager, buf));

    size_t buf_payload_len = mdh_network_stack_memory_manager_get_payload_len(gs_memory_manager, buf);

    EXPECT_EQ(buf_payload_len, gs_mtu);

    // Ignore dest and src mac addresses
    uint8_t *eth_type = buffer_data + 12U;

    eth_type[0] = (uint8_t)(gs_incomming_packet_eth_type >> 8U);
    eth_type[1] = (uint8_t)(gs_incomming_packet_eth_type);

    return MDH_EMAC_STATUS_NO_ERROR;
}

static UINT
nx_packet_allocate_custom_fake(NX_PACKET_POOL *pool_ptr, NX_PACKET **packet_ptr, ULONG packet_type, ULONG wait_option)
{
    static NX_PACKET incoming_packet = {0};

    incoming_packet.nx_packet_prepend_ptr = gs_packet_pool_buffer;
    incoming_packet.nx_packet_append_ptr = gs_packet_pool_buffer + 1;
    incoming_packet.nx_packet_length = 0;

    *packet_ptr = &incoming_packet;

    return NX_SUCCESS;
}

static mdh_network_stack_memory_manager_t *iotsdk_memory_manager_get_instance_custom_fake(void)
{
    return &gsc_memory_manager;
}

static mdh_network_stack_buffer_t *
alloc_from_heap_custom_fake(mdh_network_stack_memory_manager_t *const self, size_t size, size_t align)
{
    emac_memory_buf_t *buf = new emac_memory_buf_t;

    buf->allocated_memory = static_cast<void *>(new uint8_t[size + align]);
    buf->length = size;

    buf->data = buf->allocated_memory;

    return reinterpret_cast<mdh_network_stack_buffer_t *>(buf);
}

static void free_buf_custom_fake(mdh_network_stack_memory_manager_t *const self, const mdh_network_stack_buffer_t *buf)
{
    if (!buf) {
        return;
    }

    const emac_memory_buf_t *buffer = reinterpret_cast<const emac_memory_buf_t *>(buf);

    delete[] static_cast<uint8_t *>(buffer->allocated_memory);
    delete buffer;
}

static size_t copy_to_buf_custom_fake(mdh_network_stack_memory_manager_t *const self,
                                      const void *const ptr,
                                      size_t size,
                                      const mdh_network_stack_buffer_t *buf)
{
    const emac_memory_buf_t *dst_buf = static_cast<const emac_memory_buf_t *>(buf);

    memcpy(dst_buf->data, const_cast<void *>(ptr), size);
    return size;
}

static size_t copy_from_buf_custom_fake(mdh_network_stack_memory_manager_t *const self,
                                        const mdh_network_stack_buffer_t *const buf,
                                        void *const ptr,
                                        size_t size)
{
    const emac_memory_buf_t *const src_buf = reinterpret_cast<const emac_memory_buf_t *const>(buf);

    memcpy(ptr, src_buf->data, size);
    return size;
}

static void *get_payload_custom_fake(mdh_network_stack_memory_manager_t *const self,
                                     const mdh_network_stack_buffer_t *const buf)
{
    const emac_memory_buf_t *const buffer = reinterpret_cast<const emac_memory_buf_t *const>(buf);

    return buffer->data;
}

static size_t get_payload_len_custom_fake(mdh_network_stack_memory_manager_t *const self,
                                          const mdh_network_stack_buffer_t *const buf)
{
    const emac_memory_buf_t *const buffer = reinterpret_cast<const emac_memory_buf_t *const>(buf);

    return buffer->length;
}
