# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(lwip)

include(FetchContent)

if("lwip" IN_LIST IOTSDK_FETCH_LIST)
    # LwIP CMake build files require Doxygen to have valid CMake targets.
    # If Doxygen is not available in the system, create a fake lwipdocs target.
    find_package(Doxygen)
    if (NOT DOXYGEN_FOUND)
        add_custom_target(lwipdocs)
    endif()
    FetchContent_MakeAvailable(lwip)
endif()

if(TARGET lwipcore)
    # Set proper includes for lwipcore target
    target_include_directories(lwipcore
        PUBLIC
            ${lwip_SOURCE_DIR}/src/include
    )

    add_subdirectory(lwip_cmsis_port)
endif()
