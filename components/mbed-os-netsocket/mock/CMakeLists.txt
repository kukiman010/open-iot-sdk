# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(mbed-os-netsocket-mock)

target_sources(mbed-os-netsocket-mock
    PRIVATE
        src/cmsis_mock.c
        src/iot_socket_mock.c
        src/mbedtls_mock.c
)

target_include_directories(mbed-os-netsocket-mock
    PUBLIC
        ./include
)

target_link_libraries(mbed-os-netsocket-mock
    PUBLIC
        fff
)
