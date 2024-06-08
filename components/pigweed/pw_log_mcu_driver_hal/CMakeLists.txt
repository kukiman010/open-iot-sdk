# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(tests)
elseif()
    include(${pigweed_SOURCE_DIR}/pw_build/pigweed.cmake)
endif()

pw_add_module_config(pw_log_mcu_driver_hal_CONFIG)

pw_add_module_library(pw_log_mcu_driver_hal
    IMPLEMENTS_FACADES
        pw_log
    HEADERS
        public/pw_log_mdh/backend.h
        public/pw_log_mdh/config.h
        public/pw_log_mdh/util.h
        public_overrides/pw_log_backend/log_backend.h
    PUBLIC_INCLUDES
        public
        public_overrides
    PRIVATE_DEPS
        ${pw_log_mcu_driver_hal_CONFIG}
    PUBLIC_DEPS
        pw_preprocessor
        mcu-driver-hal
    SOURCES
        pw_log_mdh_backend.c
)

# Build locks supported by the backend.
add_subdirectory(lock)
