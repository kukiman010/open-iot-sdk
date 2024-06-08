# Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(nx-driver-mdh-test
    test_nx_driver_mdh.cpp
    ../src/iotsdk/nx_driver_mdh.c
)

target_include_directories(nx-driver-mdh-test
    PRIVATE
        ../inc
)

target_link_libraries(nx-driver-mdh-test
    PRIVATE
        mcu-driver-hal
        netxduo-mocks
        threadx-mocks
        iotsdk-memory-manager-mocks
        iotsdk-emac-factory-mocks
        fff
        project_options
)

iotsdk_add_test(nx-driver-mdh-test)
