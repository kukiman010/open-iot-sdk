# Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(pw_iot_sdk_config INTERFACE)
target_include_directories(pw_iot_sdk_config INTERFACE include)
target_compile_features(pw_iot_sdk_config INTERFACE c_static_assert)

if(TARGET freertos-kernel)
    target_compile_definitions(pw_iot_sdk_config
        INTERFACE
            PW_IOTSDK_FREERTOS=1
    )

    if(TARGET freertos-config)
        target_link_libraries(pw_iot_sdk_config
            INTERFACE
                freertos-config # FreeRTOSConfig.h
        )
    endif()

    target_link_libraries(pw_iot_sdk_config
        INTERFACE
            freertos-kernel # portmacro.h
            cmsis-core      # CMSIS_device_header
    )
elseif(TARGET cmsis-rtx)
    target_compile_definitions(pw_iot_sdk_config
        INTERFACE
            PW_IOTSDK_CMSIS_RTX=1
    )

    target_link_libraries(pw_iot_sdk_config
        INTERFACE
            cmsis-rtx  # RTX_Config.h, rtx_os.h
            cmsis-core # RTE_Components.h CMSIS_device_header
    )
endif()
