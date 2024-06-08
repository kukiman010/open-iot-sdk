# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(pw_cmsis_rtos_util
    kernel_lock.cc
)

target_include_directories(pw_cmsis_rtos_util
    PUBLIC
        include/
)

target_link_libraries(pw_cmsis_rtos_util
    PUBLIC
        pw_function
        pw_iot_sdk_config
    PRIVATE
        pw_assert.assert
        pw_assert.check
        cmsis-rtos-api
)
