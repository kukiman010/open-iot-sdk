# Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(iotsdk_threadx)

include(FetchContent)

if("threadx" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(threadx)
endif()

if("threadx-cdi-port" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(threadx-cdi-port)
endif()

if(TARGET threadx)
    # Assembly files in ThreadX ports do not include tx_user.h but require
    # configuration macros, so we force include tx_user.h
    target_compile_options(threadx
        PRIVATE
            # ThreadX always makes a copy of TX_USER_FILE here and uses it
            $<$<COMPILE_LANGUAGE:ASM>:-include ${threadx_BINARY_DIR}/custom_inc/tx_user.h>
    )
endif()

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(mocks)
endif()
