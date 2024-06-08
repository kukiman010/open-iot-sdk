/*
 * Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mps3_emac.h"

DEFINE_FAKE_VALUE_FUNC(mdh_emac_t *, mdh_emac_get_default_instance);
DEFINE_FAKE_VALUE_FUNC(mdh_emac_status_t,
                       power_up_ethernet_controller,
                       const mdh_emac_t *,
                       mdh_emac_callbacks_t *,
                       mdh_network_stack_memory_manager_t *,
                       void *);
DEFINE_FAKE_VALUE_FUNC(mdh_emac_status_t, power_down_ethernet_controller, mdh_emac_t *const);
DEFINE_FAKE_VALUE_FUNC(size_t, get_ethernet_controller_mtu, const mdh_emac_t *const);
DEFINE_FAKE_VALUE_FUNC(size_t, get_ethernet_controller_data_alignment_size, const mdh_emac_t *const);
DEFINE_FAKE_VALUE_FUNC(size_t, get_ethernet_controller_interface_name, const mdh_emac_t *const, char *, size_t);
DEFINE_FAKE_VALUE_FUNC(size_t, get_ethernet_controller_mac_addr_size, const mdh_emac_t *const);
DEFINE_FAKE_VALUE_FUNC(mdh_emac_status_t, get_ethernet_controller_mac_addr, const mdh_emac_t *const, uint8_t *);
DEFINE_FAKE_VALUE_FUNC(mdh_emac_status_t, set_ethernet_controller_mac_addr, mdh_emac_t *const, const uint8_t *);
DEFINE_FAKE_VALUE_FUNC(mdh_emac_status_t,
                       transmit_ethernet_controller,
                       mdh_emac_t *const,
                       const mdh_network_stack_buffer_t *);
DEFINE_FAKE_VALUE_FUNC(mdh_emac_status_t,
                       receive_ethernet_controller,
                       mdh_emac_t *const,
                       const mdh_network_stack_buffer_t *);
DEFINE_FAKE_VALUE_FUNC(mdh_emac_status_t,
                       add_to_multicast_group_ethernet_controller,
                       mdh_emac_t *const,
                       const uint8_t *);
DEFINE_FAKE_VALUE_FUNC(mdh_emac_status_t,
                       remove_from_multicast_group_ethernet_controller,
                       mdh_emac_t *const,
                       const uint8_t *);
DEFINE_FAKE_VALUE_FUNC(mdh_emac_status_t, config_multicast_reception_ethernet_controller, mdh_emac_t *const, bool);
