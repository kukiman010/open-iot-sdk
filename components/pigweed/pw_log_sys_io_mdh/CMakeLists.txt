# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(${pigweed_SOURCE_DIR}/pw_build/pigweed.cmake)

pw_add_module_config(pw_log_sys_io_mdh_CONFIG)

pw_add_module_library(pw_log_sys_io_mdh
    IMPLEMENTS_FACADES
        pw_log
    HEADERS
        public/pw_log_sys_io_mdh/backend.h
        public/pw_log_sys_io_mdh/config.h
        public/pw_log_sys_io_mdh/util.h
        public_overrides/pw_log_backend/log_backend.h
    PUBLIC_INCLUDES
        public
        public_overrides
    PRIVATE_DEPS
        ${pw_log_sys_io_mdh_CONFIG}
        pw_sys_io
    PUBLIC_DEPS
        pw_iot_sdk_config
        pw_preprocessor
        pw_sys_io_mcu_driver_hal
    SOURCES
        pw_log_mdh_backend.cc
)
