# Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(mbedtls-mocks
    src/threading.c
)

target_include_directories(mbedtls-mocks
    PUBLIC
        ../include
        inc
)

target_link_libraries(mbedtls-mocks
    PRIVATE
        fff
        cmsis-core-mock
)
