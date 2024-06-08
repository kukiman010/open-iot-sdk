# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-iot-sdk-c_POPULATED)
    add_library(azure-umqtt-client EXCLUDE_FROM_ALL
        ${azure-iot-sdk-c_SOURCE_DIR}/umqtt/src/mqtt_client.c
        ${azure-iot-sdk-c_SOURCE_DIR}/umqtt/src/mqtt_codec.c
        ${azure-iot-sdk-c_SOURCE_DIR}/umqtt/src/mqtt_message.c
    )

    target_include_directories(azure-umqtt-client
        PUBLIC
            ${azure-iot-sdk-c_SOURCE_DIR}/umqtt/inc
    )

    target_link_libraries(azure-umqtt-client
        PUBLIC
            azure-uamqp-client
    )
endif()
