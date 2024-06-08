# Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(trusted-firmware-m_POPULATED AND TARGET threadx)
    # OS layer for the TF-M NS interface, based on native ThreadX
    add_library(tfm-ns-interface-threadx
        ${trusted-firmware-m_BINARY_DIR}/install/interface/src/tfm_ns_interface.c
        os_wrapper_threadx.c
    )

    add_dependencies(tfm-ns-interface-threadx trusted-firmware-m-build)

    target_include_directories(tfm-ns-interface-threadx
        PUBLIC
            ${trusted-firmware-m_BINARY_DIR}/lib/ext/tfm_test_repo-src/ns_interface
    )

    target_link_libraries(tfm-ns-interface-threadx
        PUBLIC
            tfm-ns-interface
            threadx
    )
endif()

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(tests)
endif()
