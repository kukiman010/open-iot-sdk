# Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(mbedtls-threading-cmsis-rtos-test
    test_mbedtls_threading_cmsis_rtos.cpp
    ../src/mbedtls_threading_cmsis_rtos.c
)

target_include_directories(mbedtls-threading-cmsis-rtos-test
    PRIVATE
        ../include
)

target_link_libraries(mbedtls-threading-cmsis-rtos-test
    PRIVATE
        fff
        project_options
        cmsis-core-mock
        cmsis-rtos-api-mock
        mbedtls-mocks
)

iotsdk_add_test(mbedtls-threading-cmsis-rtos-test)
