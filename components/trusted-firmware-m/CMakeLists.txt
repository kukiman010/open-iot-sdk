# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(tfm-ns-interface)

include(FetchContent)

if("trusted-firmware-m" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(trusted-firmware-m)
endif()

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(mocks)
endif()

if(trusted-firmware-m_POPULATED)
    include(BuildTfm.cmake)

    # TF-M NS interface for the non-secure side

    add_library(tfm-ns-interface ${tfm_ns_interface_generated})
    add_dependencies(tfm-ns-interface trusted-firmware-m-build)

    target_include_directories(tfm-ns-interface
        PUBLIC
            ${BINARY_DIR}/generated/interface/include
            ${BINARY_DIR}/install/interface/include
    )

    target_link_libraries(tfm-ns-interface
        PRIVATE
            ${s_veneers_generated}
    )

    target_compile_definitions(tfm-ns-interface
        PUBLIC
            BL2
            PLATFORM_DEFAULT_CRYPTO_KEYS
            MCUBOOT_IMAGE_NUMBER=2
    )

    if(TARGET cmsis-rtos-api)
        # OS layer for the TF-M NS interface, based on CMSIS-RTOS
        add_library(tfm-ns-interface-cmsis-rtos ${tfm_ns_interface_cmsis_rtos_api_generated})
        add_dependencies(tfm-ns-interface-cmsis-rtos trusted-firmware-m-build)

        target_include_directories(tfm-ns-interface-cmsis-rtos
            PUBLIC
                ${BINARY_DIR}/lib/ext/tfm_test_repo-src/ns_interface/
                ${BINARY_DIR}/lib/ext/tfm_test_repo-src/ns_interface/ns_client_ext
        )

        target_link_libraries(tfm-ns-interface-cmsis-rtos
            PUBLIC
                tfm-ns-interface
                cmsis-rtos-api
        )
    endif()

    if(TARGET cmsis-rtx)
        # TrustZone support of RTX requires symbols from s_veneers.o
        target_link_libraries(cmsis-rtx
            PRIVATE
                tfm-ns-interface
                tfm-ns-interface-cmsis-rtos
        )
    endif()

    # Reference NS startup files from TF-M
    # They are optional - an application can provide its own.

    add_library(tfm-ns-startup-an552 OBJECT
        ${trusted-firmware-m_SOURCE_DIR}/platform/ext/target/arm/mps3/an552/device/source/startup_an552.c
    )
    target_include_directories(tfm-ns-startup-an552
        PRIVATE
            ${trusted-firmware-m_SOURCE_DIR}/platform/include
            ${trusted-firmware-m_SOURCE_DIR}/platform/ext/cmsis
            ${trusted-firmware-m_SOURCE_DIR}/platform/ext/target/arm/mps3/an552/device/include
            ${trusted-firmware-m_SOURCE_DIR}/platform/ext/target/arm/mps3/an552/partition
    )

    add_library(tfm-ns-startup-an555 OBJECT
        ${trusted-firmware-m_SOURCE_DIR}/platform/ext/target/arm/mps3/corstone310/common/device/source/startup_corstone310.c
    )
    target_include_directories(tfm-ns-startup-an555
        PRIVATE
            ${trusted-firmware-m_SOURCE_DIR}/platform/include
            ${trusted-firmware-m_SOURCE_DIR}/platform/ext/cmsis
            ${trusted-firmware-m_SOURCE_DIR}/platform/ext/target/arm/mps3/corstone310/fvp/device/config/
            ${trusted-firmware-m_SOURCE_DIR}/platform/ext/target/arm/mps3/corstone310/common/device/include/
            ${trusted-firmware-m_SOURCE_DIR}/platform/ext/target/arm/mps3/corstone310/common/partition/
    )

endif() # trusted-firmware-m_POPULATED

add_subdirectory(tfm_ns_interface_threadx)
