# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(pyedmgr)

include(FetchContent)

if("pyedmgr" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(pyedmgr)
endif()

if(pyedmgr_POPULATED)
    set(ENV{PYEDMGR_ROOT} ${pyedmgr_SOURCE_DIR})
endif()
