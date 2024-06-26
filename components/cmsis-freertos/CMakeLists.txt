# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(iotsdk_freertos LANGUAGES C CXX ASM)

include(FetchContent)

if("cmsis-freertos" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(cmsis-freertos)
endif()

if(cmsis-freertos_POPULATED)
    # FreeRTOS kernel

    add_library(freertos-kernel
            ${cmsis-freertos_SOURCE_DIR}/Source/croutine.c
            ${cmsis-freertos_SOURCE_DIR}/Source/event_groups.c
            ${cmsis-freertos_SOURCE_DIR}/Source/list.c
            ${cmsis-freertos_SOURCE_DIR}/Source/queue.c
            ${cmsis-freertos_SOURCE_DIR}/Source/stream_buffer.c
            ${cmsis-freertos_SOURCE_DIR}/Source/tasks.c
            ${cmsis-freertos_SOURCE_DIR}/Source/timers.c
    )

    target_include_directories(freertos-kernel
        PUBLIC
            ${cmsis-freertos_SOURCE_DIR}/Source/include
    )

    # FreeRTOS porting layer. Note: ARMClang and GCC both use the GCC port
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m55 OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m85)
        # M55 support uses the M33 port in FreeRTOS. Currently we only support non secure mode.
        # TODO: Add config to enable trustzone support.
        target_sources(freertos-kernel
            PRIVATE
                ${cmsis-freertos_SOURCE_DIR}/Source/portable/GCC/ARM_CM33_NTZ/non_secure/port.c
                ${cmsis-freertos_SOURCE_DIR}/Source/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c
        )
        target_include_directories(freertos-kernel
            PUBLIC
                ${cmsis-freertos_SOURCE_DIR}/Source/portable/GCC/ARM_CM33_NTZ/non_secure
        )
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m4)
        target_compile_definitions(freertos-kernel PRIVATE CPU_CORTEX_M4)
        target_sources(freertos-kernel
            PRIVATE
                ${cmsis-freertos_SOURCE_DIR}/Source/portable/GCC/ARM_CM4F/port.c
        )
        target_include_directories(freertos-kernel
            PUBLIC
                ${cmsis-freertos_SOURCE_DIR}/Source/portable/GCC/ARM_CM4F
        )
    endif()

    # Enable FreeRTOS TF-M secure services support.
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m33 OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m55 OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m85)
        add_library(freertos-tfm-os-wrapper
            ${cmsis-freertos_SOURCE_DIR}/Source/portable/ThirdParty/GCC/ARM_TFM/os_wrapper_freertos.c
        )
    endif()

    # Allow user to select a heap implementation.
    add_library(freertos-kernel-heap-1 ${cmsis-freertos_SOURCE_DIR}/Source/portable/MemMang/heap_1.c)
    target_link_libraries(freertos-kernel-heap-1 PRIVATE freertos-kernel)

    add_library(freertos-kernel-heap-2 ${cmsis-freertos_SOURCE_DIR}/Source/portable/MemMang/heap_2.c)
    target_link_libraries(freertos-kernel-heap-2 PRIVATE freertos-kernel)

    add_library(freertos-kernel-heap-3 ${cmsis-freertos_SOURCE_DIR}/Source/portable/MemMang/heap_3.c)
    target_link_libraries(freertos-kernel-heap-3 PRIVATE freertos-kernel)

    add_library(freertos-kernel-heap-4 ${cmsis-freertos_SOURCE_DIR}/Source/portable/MemMang/heap_4.c)
    target_link_libraries(freertos-kernel-heap-4 PRIVATE freertos-kernel)

    add_library(freertos-kernel-heap-5 ${cmsis-freertos_SOURCE_DIR}/Source/portable/MemMang/heap_5.c)
    target_link_libraries(freertos-kernel-heap-4 PRIVATE freertos-kernel)

    if(TARGET cmsis-rtos-api)
        # CMSIS-RTOS FreeRTOS implementation
        add_library(freertos-cmsis-rtos
            ${cmsis-freertos_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c
            ${cmsis-freertos_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source/freertos_evr.c
            #${cmsis-freertos_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source/handlers.c - Cortex A only
            ${cmsis-freertos_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source/os_systick.c
        )
        target_sources(freertos-cmsis-rtos
            INTERFACE
                # This retarget's libc mutexes to FreeRTOS objects. The linker
                # requires any retarget code to be a free-standing object file
                # linked directly to the executable, and symbols in static
                # library archives are not allowed to interface with libc,
                # resulting in link errors. Thus, make this an INTERFACE source
                # compiled as part of the executable.
                $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${cmsis-freertos_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Source/ARM/clib_arm.c>
        )
        target_include_directories(freertos-cmsis-rtos
            PUBLIC
                ${cmsis-freertos_SOURCE_DIR}/CMSIS/RTOS2/FreeRTOS/Include
        )
        target_link_libraries(freertos-cmsis-rtos
            PUBLIC
                cmsis-rtos-api
                freertos-kernel
        )
    endif()
endif() # cmsis-freertos_POPULATED
