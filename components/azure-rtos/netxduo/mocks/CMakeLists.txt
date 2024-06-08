# Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(netxduo-mocks
    src/nx_api.c
    src/nx_arp.c
    src/nx_ip.c
    src/nx_rarp.c
)

target_include_directories(netxduo-mocks
    PUBLIC
       inc
)

target_link_libraries(netxduo-mocks
    PUBLIC
        fff
        threadx-mocks
)
