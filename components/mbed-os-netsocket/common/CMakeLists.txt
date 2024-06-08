# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(network-helpers)

target_sources(network-helpers
    PRIVATE
        src/libBits/common_functions.c
        src/libip4string/ip4tos.c
        src/libip4string/stoip4.c
        src/libip6string/ip6tos.c
        src/libip6string/stoip6.c
)

target_include_directories(network-helpers
    PUBLIC
        include
        ../include/platform
    PRIVATE
        src/include
        ../include
)

if (CMAKE_CROSSCOMPILING)
    target_link_libraries(network-helpers
        PUBLIC
            cmsis-sockets-api
    )
else()
    target_link_libraries(network-helpers
        PUBLIC
            mbed-os-netsocket-mock
    )
endif()
