# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-iot-sdk-c_POPULATED)
    if(NOT TARGET mbedtls)
        message(ERROR "The Azure C shared utility requires mbedtls component")
    endif()

    add_library(azure-c-shared-utility EXCLUDE_FROM_ALL
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/azure_base32.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/azure_base64.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/buffer.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/connection_string_parser.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/consolelogger.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/constbuffer.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/constbuffer_array.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/constbuffer_array_batcher.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/constmap.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/crt_abstractions.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/doublylinkedlist.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/gb_rand.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/gb_stdio.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/gb_time.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/gballoc.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/hmac.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/hmacsha256.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/http_proxy_io.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/http_proxy_stub.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/httpapiex.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/httpapiexsas.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/httpheaders.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/map.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/memory_data.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/optionhandler.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/sastoken.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/sha1.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/sha224.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/sha384-512.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/singlylinkedlist.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/string_token.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/string_tokenizer.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/strings.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/urlencode.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/usha.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/utf8_checker.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/uuid.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/uws_client.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/uws_frame_encoder.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/vector.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/ws_url.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/wsio.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/xio.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/src/xlogging.c

        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/adapters/agenttime.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/adapters/httpapi_compact.c
        ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/adapters/tlsio_mbedtls.c
    )

    target_include_directories(azure-c-shared-utility
        PUBLIC
            ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/deps/azure-macro-utils-c/inc
            ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/deps/umock-c/inc
            ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/inc
            ${azure-iot-sdk-c_SOURCE_DIR}/c-utility/pal/generic
    )

    target_link_libraries(azure-c-shared-utility
        PUBLIC
            azure-iot-sdk-platform
            mbedtls
    )
endif()
