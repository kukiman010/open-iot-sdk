# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Test for mixed usage of ThreadX API and CMSIS-RTOSv2 API

set(test threadx-cmsis-rtos-mixed)
set(supported_platforms corstone-300 corstone-310)

foreach(platform IN LISTS supported_platforms)
    get_target_platform_variables(${platform})
    get_target_rtos_variables(cmsis-threadx)
    set(test_dir ${TESTS_OUTPUT_DIR}/${test}/${platform})
    configure_file(CMakeLists.txt.in ${test_dir}/CMakeLists.txt @ONLY)
    configure_file(${LINKER_SCRIPT_GCC} ${test_dir}/gcc.ld @ONLY)
    configure_file(${LINKER_SCRIPT_ARM} ${test_dir}/armclang.sct @ONLY)
    file(COPY
        main.c
        cmsis-config
        threadx-config
        test.log
        DESTINATION ${test_dir}
    )
endforeach()
