# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-iot-sdk-c_POPULATED)
    add_library(parson EXCLUDE_FROM_ALL
        ${azure-iot-sdk-c_SOURCE_DIR}/deps/parson/parson.c
    )

    target_include_directories(parson
        PUBLIC
            ${azure-iot-sdk-c_SOURCE_DIR}/deps/parson/
    )
endif()
