# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-iot-sdk-c_POPULATED)
    add_library(azure-iothub-client EXCLUDE_FROM_ALL
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/blob.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_client.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_client_authorization.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_client_core.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_client_core_ll.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_client_diagnostic.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_client_edge.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_client_ll.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_client_ll_uploadtoblob.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_client_retry_control.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_device_client.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_device_client_ll.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_message.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_module_client.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_module_client_ll.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothub_transport_ll_private.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransport.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransport_amqp_cbs_auth.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransport_amqp_common.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransport_amqp_connection.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransport_amqp_device.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransport_amqp_messenger.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransport_amqp_telemetry_messenger.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransport_amqp_twin_messenger.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransport_mqtt_common.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransportamqp.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransportamqp_methods.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransportamqp_websockets.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransporthttp.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransportmqtt.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/iothubtransportmqtt_websockets.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/message_queue.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/uamqp_messaging.c
        ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/src/version.c
    )

    target_include_directories(azure-iothub-client
        PUBLIC
            ${azure-iot-sdk-c_SOURCE_DIR}/iothub_client/inc
    )

    target_compile_definitions(azure-iothub-client
        PRIVATE
            USE_PROV_MODULE
    )

    target_link_libraries(azure-iothub-client
        PUBLIC
            azure-umqtt-client
            parson
            azure-c-shared-utility
            $<$<STREQUAL:${CMAKE_C_COMPILER_ID},GNU>:m> # libm for GCC
        PRIVATE
            azure-provisioning-client
    )
endif()
