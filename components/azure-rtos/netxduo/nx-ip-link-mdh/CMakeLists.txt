# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(tests)
else()
    if (TARGET netxduo)
        add_library(nx-ip-link-mdh
            ./src/iotsdk/nx_driver_mdh.c
        )

        target_include_directories(nx-ip-link-mdh
            PUBLIC
                inc
        )

        target_link_libraries(nx-ip-link-mdh
            PUBLIC
                mcu-driver-hal
                netxduo
                threadx
                iotsdk-memory-manager
        )

        set_property(TARGET nx-ip-link-mdh PROPERTY C_STANDARD 99)
        set(C_STANDARD_REQUIRED ON)
    endif()
endif()
