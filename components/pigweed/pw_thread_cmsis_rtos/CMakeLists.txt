# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(${pigweed_SOURCE_DIR}/pw_build/pigweed.cmake)

pw_add_module_config(pw_thread_cmsis_rtos_CONFIG)
add_library(pw_thread_cmsis_rtos.config INTERFACE)

pw_add_module_library(pw_thread_cmsis_rtos.id
    IMPLEMENTS_FACADES
        pw_thread.id
    HEADERS
        public/pw_thread_cmsis_rtos/id_native.h
        public_overrides/pw_thread_backend/id_native.h
    PUBLIC_INCLUDES
        public
        public_overrides
    PUBLIC_DEPS
        ${pw_sync_cmsis_rtos_CONFIG}
        pw_thread_cmsis_rtos.config
        pw_assert.assert
        pw_assert.check
        pw_iot_sdk_config
        cmsis-rtos-api
    SOURCES
        id.cc
)

pw_add_module_library(pw_thread_cmsis_rtos.thread
    IMPLEMENTS_FACADES
        pw_thread.thread
    HEADERS
        public/pw_thread_cmsis_rtos/thread_inline.h
        public_overrides/pw_thread_backend/thread_inline.h
        public/pw_thread_cmsis_rtos/thread_native.h
        public_overrides/pw_thread_backend/thread_native.h
    PUBLIC_INCLUDES
        public
        public_overrides
        ${cmsis_5_SOURCE_DIR}/CMSIS/RTOS2/Include
    PUBLIC_DEPS
        ${pw_sync_cmsis_rtos_CONFIG}
        pw_thread_cmsis_rtos.config
        pw_sync.thread_notification
        pw_log
        pw_cmsis_rtos_util
        pw_iot_sdk_config
        cmsis-rtos-api
    SOURCES
        thread.cc
)

pw_add_module_library(pw_thread_cmsis_rtos.sleep
    IMPLEMENTS_FACADES
        pw_thread.sleep
    PUBLIC_INCLUDES
        public
        public_overrides
    PUBLIC_DEPS
        ${pw_sync_cmsis_rtos_CONFIG}
        pw_thread_cmsis_rtos.config
        pw_iot_sdk_config
        cmsis-rtos-api
    SOURCES
        sleep.cc
)

pw_add_module_library(pw_thread_cmsis_rtos.yield
    IMPLEMENTS_FACADES
        pw_thread.yield
    PUBLIC_INCLUDES
        public
        public_overrides
    PUBLIC_DEPS
        ${pw_sync_cmsis_rtos_CONFIG}
        pw_thread_cmsis_rtos.config
        pw_iot_sdk_config
        cmsis-rtos-api
    SOURCES
        yield.cc
)
