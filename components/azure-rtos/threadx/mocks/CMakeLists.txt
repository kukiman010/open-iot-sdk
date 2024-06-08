# Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(threadx-mocks
    src/tx_api.c
)

target_include_directories(threadx-mocks
    PUBLIC
       inc
)

target_link_libraries(threadx-mocks
    PUBLIC
        fff
)
