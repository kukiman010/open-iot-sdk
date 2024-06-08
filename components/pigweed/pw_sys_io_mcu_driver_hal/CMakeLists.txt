# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(${pigweed_SOURCE_DIR}/pw_build/pigweed.cmake)

add_subdirectory(lock)

pw_add_module_config(pw_sys_io_mcu_driver_hal_CONFIG)

pw_add_module_library(pw_sys_io_mcu_driver_hal
    IMPLEMENTS_FACADES
        pw_sys_io
    HEADERS
        public/pw_sys_io_mdh/backend.h
        public/pw_sys_io_mdh/config.h
        public_overrides/pw_sys_io_backend/sys_io_backend.h
    PUBLIC_INCLUDES
        public
        public_overrides
    PRIVATE_DEPS
        ${pw_sys_io_mcu_driver_hal_CONFIG}
        pw_assert.assert
        pw_assert.check
        pw_sys_io.facade
        pw_preprocessor
        pw_span
    PUBLIC_DEPS
        pw_iot_sdk_config
        mcu-driver-hal
    SOURCES
        sys_io_mdh.cc
)
