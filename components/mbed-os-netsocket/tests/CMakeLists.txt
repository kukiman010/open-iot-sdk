# Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(mbed-os-netsocket-tests
    src/IotSocketToMbedTest.cpp
    src/SocketAddressTest.cpp
    src/TCPSocketTest.cpp
    src/UDPSocketTest.cpp
    src/TLSSocketWrapperTest.cpp
    src/TLSSocketTest.cpp
)

target_include_directories(mbed-os-netsocket-tests
    PRIVATE
        ../src/include
)

target_link_libraries(mbed-os-netsocket-tests
    PRIVATE
        mbed-os-netsocket
)

iotsdk_add_test(mbed-os-netsocket-tests)


add_executable(mbed-os-netsocket-common-tests
    common/test_common_functions.cpp
    common/test_ip4tos.cpp
    common/test_ip6tos.cpp
    common/test_stoip4.cpp
    common/test_stoip6.cpp
    ../common/src/libBits/common_functions.c
    ../common/src/libip4string/ip4tos.c
    ../common/src/libip4string/stoip4.c
    ../common/src/libip6string/ip6tos.c
    ../common/src/libip6string/stoip6.c
)

target_include_directories(mbed-os-netsocket-common-tests
    PRIVATE
        ../common/src/include
        ../common/include
)

target_link_libraries(mbed-os-netsocket-common-tests
    PRIVATE
        project_options
)

iotsdk_add_test(mbed-os-netsocket-common-tests)
