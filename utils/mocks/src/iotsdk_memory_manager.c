/*
 * Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk_memory_manager.h"

DEFINE_FAKE_VALUE_FUNC(mdh_network_stack_memory_manager_t *, iotsdk_memory_manager_get_instance);

DEFINE_FAKE_VALUE_FUNC(mdh_network_stack_buffer_t *,
                       net_stack_mem_mgr_alloc_from_heap,
                       mdh_network_stack_memory_manager_t *,
                       size_t,
                       size_t);
DEFINE_FAKE_VOID_FUNC(net_stack_mem_mgr_free_buf,
                      mdh_network_stack_memory_manager_t *,
                      const mdh_network_stack_buffer_t *);
DEFINE_FAKE_VALUE_FUNC(size_t,
                       net_stack_mem_mgr_copy_to_buf,
                       mdh_network_stack_memory_manager_t *,
                       const void *,
                       size_t,
                       const mdh_network_stack_buffer_t *);
DEFINE_FAKE_VALUE_FUNC(size_t,
                       net_stack_mem_mgr_copy_from_buf,
                       mdh_network_stack_memory_manager_t *,
                       const mdh_network_stack_buffer_t *,
                       void *,
                       size_t);
DEFINE_FAKE_VALUE_FUNC(void *,
                       net_stack_mem_mgr_get_payload,
                       mdh_network_stack_memory_manager_t *,
                       const mdh_network_stack_buffer_t *);
DEFINE_FAKE_VALUE_FUNC(size_t,
                       net_stack_mem_mgr_get_payload_len,
                       mdh_network_stack_memory_manager_t *,
                       const mdh_network_stack_buffer_t *);
