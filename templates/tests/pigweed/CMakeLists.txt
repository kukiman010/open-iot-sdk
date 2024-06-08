# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Pigweed facade tests

set(test pigweed)
set(supported_platforms corstone-300 corstone-310)
set(supported_rtoses cmsis-rtx cmsis-freertos)

foreach(platform IN LISTS supported_platforms)
    get_target_platform_variables(${platform})

    foreach(rtos IN LISTS supported_rtoses)
        get_target_rtos_variables(${rtos})
        set(test_dir ${TESTS_OUTPUT_DIR}/${test}/${rtos}/${platform})

        configure_file(CMakeLists.txt.in ${test_dir}/CMakeLists.txt @ONLY)
        configure_file(${LINKER_SCRIPT_GCC} ${test_dir}/gcc.ld @ONLY)
        configure_file(${LINKER_SCRIPT_ARM} ${test_dir}/armclang.sct @ONLY)

        file(COPY
            cmake
            cmsis-config
            freertos-config
            logs
            pw_unit_test_cmsis_rtos.main

            ${LINKER_SCRIPTS}
            DESTINATION ${test_dir}
        )

    endforeach()

endforeach()
