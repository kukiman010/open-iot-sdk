# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(cmsis-core-mock INTERFACE)

target_include_directories(cmsis-core-mock
    INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/inc
)

add_library(cmsis-rtos-api-mock
    src/cmsis_os2.c
)

target_link_libraries(cmsis-rtos-api-mock
    PRIVATE
        fff
        cmsis-core-mock
)
