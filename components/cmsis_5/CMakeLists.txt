# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(cmsis_5 LANGUAGES C CXX ASM)

include(FetchContent)

if("cmsis-5" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(cmsis-5)
endif()

if(cmsis-5_POPULATED)
    # CMSIS-Core

    add_library(cmsis-core INTERFACE)

    target_include_directories(cmsis-core
        INTERFACE
            ${cmsis-5_SOURCE_DIR}/CMSIS/Core/Include
            $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},cortex-m55>:${cmsis-5_SOURCE_DIR}/Device/ARM/ARMCM55/Include>
            $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},cortex-m85>:${cmsis-5_SOURCE_DIR}/Device/ARM/ARMCM85/Include>
            $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},cortex-m4>:${cmsis-5_SOURCE_DIR}/Device/ARM/ARMCM4/Include>
    )

    # CMSIS-RTOS v2 API

    add_library(cmsis-rtos-api
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/Source/os_systick.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/Source/os_tick_ptim.c
        # ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/Source/os_tick_gtim.c # Cortex-A only
    )
    target_include_directories(cmsis-rtos-api
        PUBLIC
            ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/Include
    )
    target_link_libraries(cmsis-rtos-api
        PUBLIC
            cmsis-core
    )

    # CMSIS-RTX5

    add_library(cmsis-rtx
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Config/RTX_Config.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Library/cmsis_os1.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_delay.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_evflags.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_evr.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_kernel.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_lib.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_memory.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_mempool.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_msgqueue.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_mutex.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_semaphore.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_system.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_thread.c
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/rtx_timer.c
        # Note: Arm Compiler 6 prefers the GNU assembly format, thus "GCC"
        ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source/GCC/irq_armv8mml.S
    )
    target_include_directories(cmsis-rtx
        PUBLIC
            ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Config
            ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Include
            ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Include1
            ${cmsis-5_SOURCE_DIR}/CMSIS/RTOS2/RTX/Source
    )

    target_compile_definitions(cmsis-rtx
        PRIVATE
            # allows RTX_Config.h to get RTX overrides from RTE_Components.h
            _RTE_
    )

    target_link_libraries(cmsis-rtx
        PUBLIC
            cmsis-rtos-api
    )

    # Reference startup file from CMSIS_5
    # This is optional - an application can choose to use it or provide its own.
    add_library(cmsis-startup-cortex-m
        $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},cortex-m55>:${cmsis-5_SOURCE_DIR}/Device/ARM/ARMCM55/Source/startup_ARMCM55.c>
        $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},cortex-m85>:${cmsis-5_SOURCE_DIR}/Device/ARM/ARMCM85/Source/startup_ARMCM85.c>
        $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},cortex-m4>:${cmsis-5_SOURCE_DIR}/Device/ARM/ARMCM4/Source/startup_ARMCM4.c>
    )

    target_compile_definitions(cmsis-startup-cortex-m
        PRIVATE
            $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},cortex-m55>:ARMCM55>
            $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},cortex-m85>:ARMCM85>
            $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},cortex-m4>:ARMCM4>
    )

    target_link_libraries(cmsis-startup-cortex-m
        PRIVATE
            cmsis-core
    )
endif() # cmsis-5_POPULATED

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(mocks)
endif()
