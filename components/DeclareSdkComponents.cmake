# Copyright (c) 2022 - 2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

# Declare all components here, so that application developers can easily see
# what are available to fetch. Before an application fetches and adds the Open
# IoT SDK, it needs to set `IOTSDK_FETCH_LIST` to a list of components to fetch.

FetchContent_Declare(
    arm-2d
    GIT_REPOSITORY  https://git.gitlab.arm.com/iot/open-iot-sdk/examples/ts-pixelgraphics.git
    GIT_TAG         f1b5e71860bd8defc935747558acb48fad95fa9f
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    aws-iot-device-sdk-embedded-c
    GIT_REPOSITORY https://github.com/aws/aws-iot-device-sdk-embedded-C
    GIT_TAG        75e545b0e807ab6dff9bcb0ee5942e9a58435b10
    GIT_SHALLOW    OFF
    GIT_PROGRESS   ON
    # Note: This prevents FetchContent_MakeAvailable() from calling
    # add_subdirectory() on the fetched repository. The built-in CMake support
    # of aws-iot-device-sdk-embedded-C does not work out of the box with the
    # Open IoT SDK, so we do custom CMake support in the SDK.
    SOURCE_SUBDIR  NONE
)

FetchContent_Declare(
    azure-iot-sdk-c
    GIT_REPOSITORY  https://github.com/Azure/azure-iot-sdk-c
    GIT_TAG         3113236d6146cf0363eb9f5ee252c80d546b8a43
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
    # Note: This prevents FetchContent_MakeAvailable() from calling
    # add_subdirectory() on the fetched repository. The built-in CMake support
    # of azure-iot-sdk-c does not work out of the box with the
    # Open IoT SDK, so we do custom CMake support in the SDK.
    SOURCE_SUBDIR   NONE
)

FetchContent_Declare(
    cmsis-5
    GIT_REPOSITORY  https://github.com/ARM-software/CMSIS_5.git
    GIT_TAG         03560b9c2f7fb69a6b337881071e591e4d7fcc84
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    cmsis-dsp
    GIT_REPOSITORY  https://github.com/ARM-software/CMSIS-DSP.git
    GIT_TAG         30302ca6912720cc6868a0fc4ebc19ca487fb347
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    cmsis-freertos
    GIT_REPOSITORY  https://github.com/ARM-software/CMSIS-FreeRTOS.git
    GIT_TAG         e7a610a4b5cd70f22ce012efa7623a3dcca2917c
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    cmsis-sockets-api
    GIT_REPOSITORY  https://git.gitlab.arm.com/iot/open-iot-sdk/iot-socket.git
    GIT_TAG         8412fb4769701bb5c2b7cd8db356b9e689185597
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    littlefs
    GIT_REPOSITORY  https://github.com/littlefs-project/littlefs.git
    GIT_TAG         v2.5.1
    GIT_SHALLOW     ON
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    lwip
    GIT_REPOSITORY  https://github.com/lwip-tcpip/lwip.git
    GIT_TAG         STABLE-2_1_3_RELEASE
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    mbedtls
    GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls.git
    GIT_TAG        v2.26.0
    GIT_SHALLOW    ON
    GIT_PROGRESS   ON
)

FetchContent_Declare(
    mcu-driver-reference-platforms-for-arm
    GIT_REPOSITORY https://git.gitlab.arm.com/iot/open-iot-sdk/mcu-driver-hal/mcu-driver-reference-platforms-for-arm.git
    GIT_TAG        bf61750e16eae4148bfaa4f50a6eab4d9991b47a
    GIT_PROGRESS   ON
)

FetchContent_Declare(
    ml-embedded-evaluation-kit
    GIT_REPOSITORY  https://review.mlplatform.org/ml/ethos-u/ml-embedded-evaluation-kit
    GIT_TAG         5b4fea129b2f738fc877916111908b955ad07d5a
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
    PATCH_COMMAND   ${ML_PATCH_COMMAND}
    # Note: This prevents FetchContent_MakeAvailable() from calling
    # add_subdirectory() on the fetched repository. ml-embedded-evaluation-kit
    # needs a standalone build because it uses its own toolchain files.
    SOURCE_SUBDIR   NONE
)

FetchContent_Declare(
    netxduo
    GIT_REPOSITORY  https://github.com/azure-rtos/netxduo.git
    GIT_TAG         2973652d801e92615caa285ad9ee13db19a26cb9
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    nimble
    GIT_REPOSITORY  https://github.com/apache/mynewt-nimble.git
    GIT_TAG         df80b4c954dcfb01259fa076b2de0f84de9d8528
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    pigweed
    GIT_REPOSITORY  https://github.com/google/pigweed.git
    GIT_TAG         7ac9fbde25331b72d7b13c13bf51b2f0714efb21
    # Note: According to CMake's documentation, the patch command is called
    # whenever the sources have been updated, but changes introduced by previous
    # patch are not discarded. That makes the PATCH_COMMAND itself responsible
    # for ensuring that it can be safely re-run after any update.
    PATCH_COMMAND   git reset --hard --quiet && git clean --force -dx --quiet && git apply ${CMAKE_CURRENT_SOURCE_DIR}/pigweed/pigweed.patch
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    pyedmgr
    GIT_REPOSITORY  https://git.gitlab.arm.com/iot/open-iot-sdk/tools/pyedmgr.git
    GIT_TAG         523d60b95d9a7ee78f9c6ae458153c0ed465fc87
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    threadx
    GIT_REPOSITORY  https://github.com/azure-rtos/threadx.git
    GIT_TAG         4e62226eeaf870827facbeef40bd1767db5cf9f0
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    threadx-cdi-port
    GIT_REPOSITORY  https://git.gitlab.arm.com/iot/open-iot-sdk/threadx-cdi-port.git
    GIT_TAG         a83cd4907133c6c7f4f90599c525d8c03c8b6b71
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    tinyusb
    GIT_REPOSITORY  https://github.com/hathach/tinyusb.git
    GIT_TAG         549bee94add9d8f1c346bfa0fa6b25481db3f191
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_Declare(
    trusted-firmware-m
    GIT_REPOSITORY  https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git
    GIT_TAG         3ae2ac3022fdb07adcc6272b9cb0761075b5f991
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
    # Note: This prevents FetchContent_MakeAvailable() from calling
    # add_subdirectory() on the fetched repository. TF-M needs a
    # standalone build because it relies on functions defined in its
    # own toolchain files and contains paths that reference the
    # top-level project instead of its own project.
    SOURCE_SUBDIR   NONE
)
