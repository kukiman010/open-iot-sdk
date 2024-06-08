# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Components

include(DeclareSdkComponents.cmake)

# DO NOT CHANGE THE ORDER OF ADDITION AS SOME COMPONENTS NEED TO BE ADDED BEFORE OTHERS
add_subdirectory(storage)
add_subdirectory(mcu-driver-reference-platforms-for-arm)
add_subdirectory(cmsis_5)
add_subdirectory(cmsis-dsp)
add_subdirectory(arm-2d)
add_subdirectory(cmsis-freertos)
add_subdirectory(azure-rtos)
add_subdirectory(connectivity)
add_subdirectory(trusted-firmware-m)
add_subdirectory(ml-embedded-evaluation-kit)
add_subdirectory(tinyusb)
add_subdirectory(nimble)
add_subdirectory(pigweed)
add_subdirectory(pyedmgr)
add_subdirectory(mbedtls)
add_subdirectory(aws-iot-device-sdk-embedded-c)
add_subdirectory(azure-iot-sdk-c)
add_subdirectory(mbed-os-netsocket)
