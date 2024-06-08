# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(cmsis-sockets-api)

include(FetchContent)

if("cmsis-sockets-api" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(cmsis-sockets-api)
endif()

if(cmsis-sockets-api_POPULATED)
    add_library(cmsis-sockets-api INTERFACE)

    target_include_directories(cmsis-sockets-api
        INTERFACE
            ${cmsis-sockets-api_SOURCE_DIR}/include
            include
    )

    target_sources(cmsis-sockets-api
        PRIVATE
            ${cmsis-sockets-api_SOURCE_DIR}/include/iot_socket.h
    )

    # because we don't have sources we need to set the language manually
    set_target_properties(cmsis-sockets-api PROPERTIES LINKER_LANGUAGE C)

    # Include SDK implementation based on LwIP
    add_subdirectory(lwip_sockets)
endif()
