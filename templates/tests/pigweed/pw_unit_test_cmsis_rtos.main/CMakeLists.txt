# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

pw_add_module_config(pw_unit_test_cmsis_rtos.main_CONFIG)
pw_add_module_library(pw_unit_test_cmsis_rtos.main
    IMPLEMENTS_FACADES
        pw_unit_test.main
    SOURCES
        main.cc
        ${CMAKE_BINARY_DIR}/_deps/pigweed-src/pw_unit_test/simple_printing_event_handler.cc
    PRIVATE_DEPS
        pw_assert.assert
        pw_assert.check
        pw_log
        pw_unit_test
        pw_unit_test.googletest_style_event_handler
        pw_preprocessor
        pw_string
        pw_sys_io

        mcu-driver-hal

        cmsis-rtos-api
)

# This was needed for GCC build.
if(IOTSDK_FREERTOS)
    target_link_libraries(pw_unit_test_cmsis_rtos.main PRIVATE freertos-cmsis-rtos)
elseif(IOTSDK_CMSIS_RTX)
    target_link_libraries(pw_unit_test_cmsis_rtos.main PRIVATE cmsis-rtx)
endif()
