/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MPS3_EMAC_H
#define MPS3_EMAC_H

#include "fff.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/emac_api.h"

DECLARE_FAKE_VALUE_FUNC(mdh_emac_t *, mdh_emac_get_default_instance);
DECLARE_FAKE_VALUE_FUNC(mdh_emac_status_t,
                        power_up_ethernet_controller,
                        const mdh_emac_t *,
                        mdh_emac_callbacks_t *,
                        mdh_network_stack_memory_manager_t *,
                        void *);
DECLARE_FAKE_VALUE_FUNC(mdh_emac_status_t, power_down_ethernet_controller, mdh_emac_t *const);
DECLARE_FAKE_VALUE_FUNC(size_t, get_ethernet_controller_mtu, const mdh_emac_t *const);
DECLARE_FAKE_VALUE_FUNC(size_t, get_ethernet_controller_data_alignment_size, const mdh_emac_t *const);
DECLARE_FAKE_VALUE_FUNC(size_t, get_ethernet_controller_interface_name, const mdh_emac_t *const, char *, size_t);
DECLARE_FAKE_VALUE_FUNC(size_t, get_ethernet_controller_mac_addr_size, const mdh_emac_t *const);
DECLARE_FAKE_VALUE_FUNC(mdh_emac_status_t, get_ethernet_controller_mac_addr, const mdh_emac_t *const, uint8_t *);
DECLARE_FAKE_VALUE_FUNC(mdh_emac_status_t, set_ethernet_controller_mac_addr, mdh_emac_t *const, const uint8_t *);
DECLARE_FAKE_VALUE_FUNC(mdh_emac_status_t,
                        transmit_ethernet_controller,
                        mdh_emac_t *const,
                        const mdh_network_stack_buffer_t *);
DECLARE_FAKE_VALUE_FUNC(mdh_emac_status_t,
                        receive_ethernet_controller,
                        mdh_emac_t *const,
                        const mdh_network_stack_buffer_t *);
DECLARE_FAKE_VALUE_FUNC(mdh_emac_status_t,
                        add_to_multicast_group_ethernet_controller,
                        mdh_emac_t *const,
                        const uint8_t *);
DECLARE_FAKE_VALUE_FUNC(mdh_emac_status_t,
                        remove_from_multicast_group_ethernet_controller,
                        mdh_emac_t *const,
                        const uint8_t *);
DECLARE_FAKE_VALUE_FUNC(mdh_emac_status_t, config_multicast_reception_ethernet_controller, mdh_emac_t *const, bool);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MPS3_EMAC_H
