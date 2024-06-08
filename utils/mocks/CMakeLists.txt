# Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(iotsdk-memory-manager-mocks
    src/iotsdk_memory_manager.c
)

target_include_directories(iotsdk-memory-manager-mocks
    PUBLIC
       inc
)

target_link_libraries(iotsdk-memory-manager-mocks
    PUBLIC
        mcu-driver-hal
        fff
)

add_library(iotsdk-emac-factory-mocks
    src/mps3_emac.c
)

target_include_directories(iotsdk-emac-factory-mocks
    PUBLIC
       inc
)

target_link_libraries(iotsdk-emac-factory-mocks
    PUBLIC
        mcu-driver-hal
        fff
)
