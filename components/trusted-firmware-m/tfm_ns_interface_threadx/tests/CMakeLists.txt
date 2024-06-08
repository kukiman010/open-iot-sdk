# Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(tfm-ns-interface-threadx-test
    test_os_wrapper_threadx.cpp
    ../os_wrapper_threadx.c
)

target_link_libraries(tfm-ns-interface-threadx-test
    PRIVATE
        trusted-firmware-m-mocks
        threadx-mocks
        fff
        project_options
        project_warnings
)

iotsdk_add_test(tfm-ns-interface-threadx-test)
