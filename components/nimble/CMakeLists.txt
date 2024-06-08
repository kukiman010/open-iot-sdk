# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(nimble LANGUAGES C)

include(FetchContent)

if("nimble" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(nimble)
endif()

# NimBLE
