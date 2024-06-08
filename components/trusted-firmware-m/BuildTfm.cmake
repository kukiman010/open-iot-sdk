# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Files generated by TF-M build must be listed as BUILD_BYPRODUCTS
# to inform CMake that they don't exist before build starts. Include
# paths do not need to be listed.
# <BINARY_DIR> is a placeholder keyword in ExternalProject_Add.

set(tfm_ns_interface_generated
    <BINARY_DIR>/install/interface/src/tfm_psa_ns_api.c
    <BINARY_DIR>/install/interface/src/tfm_ps_api.c
    <BINARY_DIR>/install/interface/src/tfm_its_api.c
    <BINARY_DIR>/install/interface/src/tfm_crypto_api.c
    <BINARY_DIR>/install/interface/src/tfm_attest_api.c
    <BINARY_DIR>/install/interface/src/tfm_platform_api.c
)
if(TFM_PSA_FIRMWARE_UPDATE)
    list(APPEND tfm_ns_interface_generated <BINARY_DIR>/install/interface/src/tfm_fwu_api.c)
endif()

set(s_veneers_generated
    <BINARY_DIR>/install/interface/lib/s_veneers.o
)
set(tfm_ns_interface_cmsis_rtos_api_generated
    <BINARY_DIR>/install/interface/src/tfm_ns_interface.c
    <BINARY_DIR>/install/interface/src/os_wrapper_cmsis_rtos_v2.c
    <BINARY_DIR>/install/interface/src/tz_shim_layer.c
)
set(tfm_ns_interface_cmsis_rtos_api_src
    <BINARY_DIR>/lib/ext/tfm_test_repo-src/app/tfm_ns_interface.c
    <BINARY_DIR>/lib/ext/tfm_test_repo-src/app/os_wrapper_cmsis_rtos_v2.c
    <BINARY_DIR>/lib/ext/tfm_test_repo-src/ns_interface/ns_client_ext/tz_shim_layer.c
)

include(ExternalProject)

# TF-M can be built with a different toolchain, but the toolchain that
# builds the IoT SDK is guaranteed to be available in the environment.
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(tfm_toolchain_file "toolchain_GNUARM.cmake")
elseif(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
    set(tfm_toolchain_file "toolchain_ARMCLANG.cmake")
else()
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_C_COMPILER_ID}")
endif()

ExternalProject_Add(
    trusted-firmware-m-build

    # Use code fetched by FetchContent
    DOWNLOAD_COMMAND    ""
    SOURCE_DIR          ${trusted-firmware-m_SOURCE_DIR}
    BINARY_DIR          ${trusted-firmware-m_BINARY_DIR}

    CMAKE_ARGS
        -D TFM_TOOLCHAIN_FILE=<SOURCE_DIR>/${tfm_toolchain_file}
        -D TFM_PLATFORM=${TFM_PLATFORM}
        -D TFM_PARTITION_FIRMWARE_UPDATE=${TFM_PSA_FIRMWARE_UPDATE}
        -D PLATFORM_HAS_FIRMWARE_UPDATE_SUPPORT=${TFM_PSA_FIRMWARE_UPDATE}
        -D MCUBOOT_DATA_SHARING=${TFM_PSA_FIRMWARE_UPDATE}
        -D MCUBOOT_IMAGE_VERSION_NS=${MCUBOOT_IMAGE_VERSION_NS}
        -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        ${TFM_CMAKE_ARGS}

    PATCH_COMMAND
        ${TFM_PATCH_COMMAND}

    BUILD_BYPRODUCTS
        ${tfm_ns_interface_generated}
        ${s_veneers_generated}
)

# Note: The CMSIS-RTOS-API porting layer is part of the tf-m-tests
# repository fetched by TF-M. It is part of the build byproducts which
# CMake's "clean" target deletes. To avoid deleting files from tf-m-tests
# and causing rebuild failures, make a copy of those files.
ExternalProject_Add_Step(trusted-firmware-m-build copy_tfm_ns_interface_cmsis_rtos_api
    DEPENDEES   install
    COMMAND
        ${CMAKE_COMMAND} -E copy
        ${tfm_ns_interface_cmsis_rtos_api_src}
        <BINARY_DIR>/install/interface/src
    BYPRODUCTS
        ${tfm_ns_interface_cmsis_rtos_api_generated}
)

# The path ${BINARY_DIR} is available after ExternalProject_Add.
# Convert <BINARY_DIR> to allow projects to use those files.
ExternalProject_Get_Property(trusted-firmware-m-build BINARY_DIR)
list(TRANSFORM tfm_ns_interface_generated REPLACE "<BINARY_DIR>" "${BINARY_DIR}")
list(TRANSFORM s_veneers_generated REPLACE "<BINARY_DIR>" "${BINARY_DIR}")
list(TRANSFORM tfm_ns_interface_cmsis_rtos_api_generated REPLACE "<BINARY_DIR>" "${BINARY_DIR}")
