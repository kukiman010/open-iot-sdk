# Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Mbed OS netsocket library configuration

add_subdirectory(common)

add_library(mbed-os-netsocket)

target_sources(mbed-os-netsocket
    PRIVATE
        src/IotSocketToMbed.c
        src/SocketAddress.cpp
        src/InternetSocket.cpp
        src/TCPSocket.cpp
        src/InternetDatagramSocket.cpp
        src/UDPSocket.cpp
        src/TLSSocketWrapper.cpp
        src/TLSSocket.cpp
)

target_include_directories(mbed-os-netsocket
    PUBLIC
        include
    PRIVATE
        src/include
)

if (CMAKE_CROSSCOMPILING)
    target_link_libraries(mbed-os-netsocket
        PUBLIC
            network-helpers
            mbedtls
            lwip-sockets
            pw_log
            pw_log_mdh_cmsis_rtos_lock
            cmsis-rtos-api
    )
else()
    target_link_libraries(mbed-os-netsocket
        PUBLIC
            network-helpers
    )
endif()

if (BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(mock)
    add_subdirectory(tests)
endif()
