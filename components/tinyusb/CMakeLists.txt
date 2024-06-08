# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(tinyusb LANGUAGES C)

include(FetchContent)

if("tinyusb" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(tinyusb)
endif()

# Tiny USB
