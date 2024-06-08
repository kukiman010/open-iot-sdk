# Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

if("netxduo" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(netxduo)
endif()

add_subdirectory(nx-ip-link-mdh)

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(mocks)
endif()
