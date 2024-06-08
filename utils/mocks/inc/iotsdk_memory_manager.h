/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IOTSDK_MEMORY_MANAGER_H
#define IOTSDK_MEMORY_MANAGER_H

#include "fff.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/network_stack_memory_manager.h"

DECLARE_FAKE_VALUE_FUNC(mdh_network_stack_memory_manager_t *, iotsdk_memory_manager_get_instance);
DECLARE_FAKE_VALUE_FUNC(mdh_network_stack_buffer_t *,
                        net_stack_mem_mgr_alloc_from_heap,
                        mdh_network_stack_memory_manager_t *,
                        size_t,
                        size_t);
DECLARE_FAKE_VOID_FUNC(net_stack_mem_mgr_free_buf,
                       mdh_network_stack_memory_manager_t *,
                       const mdh_network_stack_buffer_t *);
DECLARE_FAKE_VALUE_FUNC(size_t,
                        net_stack_mem_mgr_copy_to_buf,
                        mdh_network_stack_memory_manager_t *,
                        const void *,
                        size_t,
                        const mdh_network_stack_buffer_t *);
DECLARE_FAKE_VALUE_FUNC(size_t,
                        net_stack_mem_mgr_copy_from_buf,
                        mdh_network_stack_memory_manager_t *,
                        const mdh_network_stack_buffer_t *,
                        void *,
                        size_t);
DECLARE_FAKE_VALUE_FUNC(void *,
                        net_stack_mem_mgr_get_payload,
                        mdh_network_stack_memory_manager_t *,
                        const mdh_network_stack_buffer_t *);
DECLARE_FAKE_VALUE_FUNC(size_t,
                        net_stack_mem_mgr_get_payload_len,
                        mdh_network_stack_memory_manager_t *,
                        const mdh_network_stack_buffer_t *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // IOTSDK_MEMORY_MANAGER_H
