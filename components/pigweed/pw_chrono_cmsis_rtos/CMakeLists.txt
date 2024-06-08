# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(${pigweed_SOURCE_DIR}/pw_build/pigweed.cmake)

pw_add_module_config(pw_chrono_cmsis_rtos_CONFIG)

pw_add_module_library(pw_chrono_cmsis_rtos.system_clock
    IMPLEMENTS_FACADES
        pw_chrono.system_clock
    HEADERS
        public/pw_chrono_cmsis_rtos/backend.h
        public/pw_chrono_cmsis_rtos/system_clock_config.h
        public_overrides/pw_chrono_backend/chrono_backend.h
        public_overrides/pw_chrono_backend/system_clock_config.h
    PUBLIC_INCLUDES
        public
        public_overrides
    PRIVATE_DEPS
        ${pw_chrono_cmsis_rtos_CONFIG}
        pw_chrono.epoch
        pw_sync.interrupt_spin_lock
        pw_log
        pw_preprocessor
        pw_cmsis_rtos_util
    PUBLIC_DEPS
        pw_iot_sdk_config
        pw_assert.assert
        pw_assert.check
        pw_function
        cmsis-rtos-api
    SOURCES
        system_clock.cc
)

pw_add_module_library(pw_chrono_cmsis_rtos.system_timer
    IMPLEMENTS_FACADES
        pw_chrono.system_timer
    HEADERS
        public/pw_chrono_cmsis_rtos/backend.h
        public/pw_chrono_cmsis_rtos/system_timer_inline.h
        public/pw_chrono_cmsis_rtos/system_timer_native.h
        public_overrides/pw_chrono_backend/chrono_backend.h
        public_overrides/pw_chrono_backend/system_timer_inline.h
        public_overrides/pw_chrono_backend/system_timer_native.h
    PUBLIC_INCLUDES
        public
        public_overrides
    PRIVATE_DEPS
        pw_chrono.epoch
        pw_log
        pw_cmsis_rtos_util
    PUBLIC_DEPS
        pw_chrono_cmsis_rtos.system_clock
        pw_iot_sdk_config
        pw_assert.assert
        pw_assert.check
        pw_function
        cmsis-rtos-api
    SOURCES
        system_timer.cc
)
