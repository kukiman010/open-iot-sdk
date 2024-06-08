# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(pigweed LANGUAGES C CXX ASM)

include(FetchContent)

if("pigweed" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(pigweed)
endif()

if(pigweed_POPULATED)
    set(CMAKE_CXX_STANDARD 17)

    # Include backend implementations.
    add_subdirectory(pw_iot_sdk_config)
    add_subdirectory(pw_cmsis_rtos_util)
    add_subdirectory(pw_chrono_cmsis_rtos)
    add_subdirectory(pw_log_mcu_driver_hal)
    add_subdirectory(pw_log_sys_io_mdh)
    add_subdirectory(pw_sys_io_mcu_driver_hal)
    add_subdirectory(pw_sync_cmsis_rtos)
    add_subdirectory(pw_thread_cmsis_rtos)
endif()
