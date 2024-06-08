# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

macro(get_target_rtos_variables rtos)
    if("${${rtos}-STRING}" STREQUAL "")
        message(FATAL_ERROR "get_target_rtos_variables called with invalid rtos=${rtos}")
    endif()

    set(RTOS_STRING ${${rtos}-STRING})
    set(RTOS_FETCH ${${rtos}-FETCH})
    set(RTOS_LIBS ${${rtos}-LIBS})
    set(RTOS_HEADER_INCLUDE ${${rtos}-HEADER_INCLUDE})
    set(RTOS_CONFIG_SET ${${rtos}-CONFIG_SET})
endmacro()

# CMSIS RTX
set(cmsis-rtx-STRING "CMSIS RTX")
set(cmsis-rtx-FETCH "cmsis-5")
set(cmsis-rtx-LIBS "cmsis-rtx")
set(cmsis-rtx-HEADER_INCLUDE
    "target_include_directories(cmsis-core INTERFACE cmsis-config)"
)

# CMSIS RTOS v2 wrapper on FreeRTOS
set(cmsis-freertos-STRING "CMSIS RTOS API wrapper on FreeRTOS")
set(cmsis-freertos-FETCH
    "cmsis-freertos
    cmsis-5")
set(cmsis-freertos-LIBS "freertos-cmsis-rtos
    freertos-kernel-heap-2")
set(cmsis-freertos-HEADER_INCLUDE
"target_include_directories(cmsis-core INTERFACE cmsis-config)
target_include_directories(freertos-kernel PUBLIC freertos-config)
target_link_libraries(freertos-kernel PUBLIC cmsis-core)"
)

# CMSIS RTOS v2 wrapper on ThreadX
set(cmsis-threadx-STRING "CMSIS RTOS API wrapper on ThreadX")
set(cmsis-threadx-FETCH
    "threadx-cdi-port
    cmsis-5")
set(cmsis-threadx-LIBS "threadx-cdi-port")
set(cmsis-threadx-HEADER_INCLUDE
    "target_include_directories(cmsis-core INTERFACE cmsis-config)"
)
set(cmsis-threadx-CONFIG_SET
    "set(TX_USER_FILE \"\$\{CMAKE_CURRENT_LIST_DIR\}/threadx-config/tx_user.h\")"
)

# FreeRTOS
set(freertos-STRING "FreeRTOS")
set(freertos-FETCH "cmsis-freertos")
set(freertos-LIBS "freertos-kernel
    freertos-kernel-heap-2")
set(freertos-HEADER_INCLUDE
    "target_include_directories(freertos-kernel PUBLIC freertos-config)"
)

# FreeRTOS wrapper on ThreadX
set(freertos-threadx-STRING "FreeRTOS API wrapper on ThreadX")
set(freertos-threadx-FETCH "threadx")
set(freertos-threadx-LIBS "freertos-threadx")
set(freertos-threadx-HEADER_INCLUDE
    "target_include_directories(threadx PUBLIC freertos-config)"
)
set(freertos-threadx-CONFIG_SET
    "set(TX_USER_FILE \"\$\{CMAKE_CURRENT_LIST_DIR\}/threadx-config/tx_user.h\")"
)

# ThreadX
set(threadx-STRING "ThreadX")
set(threadx-FETCH "threadx")
set(threadx-LIBS "threadx")
set(threadx-CONFIG_SET
    "set(TX_USER_FILE \"\$\{CMAKE_CURRENT_LIST_DIR\}/threadx-config/tx_user.h\")"
)
