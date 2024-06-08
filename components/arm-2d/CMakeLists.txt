# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

if("arm-2d" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(arm-2d)
endif()
