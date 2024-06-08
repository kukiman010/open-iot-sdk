# Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(trusted-firmware-m-mocks INTERFACE)

target_include_directories(trusted-firmware-m-mocks
    INTERFACE
       inc
)

target_link_libraries(trusted-firmware-m-mocks
    INTERFACE
        fff
)
