# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Azure NetX Duo example

set(example azure-netxduo)
set(supported_platforms corstone-300 corstone-310)

file(STRINGS intro.md EXAMPLE_INTRO NEWLINE_CONSUME)

foreach(platform IN LISTS supported_platforms)
    get_target_platform_variables(${platform})
    get_target_rtos_variables(threadx)
    set(EXAMPLE_TITLE "Azure RTOS NetX Duo example")
    set(EXAMPLE_RUN_COMMAND "${PLATFORM_VHT} -a __build/iotsdk-example-azure-netxduo.elf -C mps3_board.smsc_91c111.enabled=1 -C mps3_board.hostbridge.userNetworking=1")
    set(example_dir ${EXAMPLES_OUTPUT_DIR}/${example}/${platform})
    configure_file(CMakeLists.txt.in ${example_dir}/CMakeLists.txt @ONLY)
    configure_file(${README_TEMPLATE_FILE} ${example_dir}/README.md @ONLY)
    configure_file(${LINKER_SCRIPT_GCC} ${example_dir}/gcc.ld @ONLY)
    configure_file(${LINKER_SCRIPT_ARM} ${example_dir}/armclang.sct @ONLY)
    file(COPY
        app_config.h
        main.c
        test.log
        threadx-config
        DESTINATION ${example_dir}
    )
endforeach()
