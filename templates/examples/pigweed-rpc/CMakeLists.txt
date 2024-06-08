# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Pigweed RPC example

set(example pigweed-rpc)
set(supported_platforms corstone-300 corstone-310)
set(supported_rtoses cmsis-rtx cmsis-freertos)

foreach(platform IN LISTS supported_platforms)
    get_target_platform_variables(${platform})

    foreach(rtos IN LISTS supported_rtoses)
        get_target_rtos_variables(${rtos})

        set(EXAMPLE_TITLE "Example for Pigweed RPC")
        set(EXAMPLE_INTRO "This example shows the usage of Pigweed RPC. RTOS: ${RTOS_STRING}, platform: ${PLATFORM_STRING}.")
        set(EXAMPLE_RUN_COMMAND "__codegen/examples/pigweed-rpc/cmsis-rtx/corstone-300/example/run-example/run-example __build/examples/pigweed-rpc/cmsis-rtx/corstone-300/_deps/pyedmgr-src __build/examples/pigweed-rpc/cmsis-rtx/corstone-300/iotsdk-example-pigweed-rpc-server.elf")
        set(example_dir ${EXAMPLES_OUTPUT_DIR}/${example}/${rtos}/${platform})
        configure_file(${LINKER_SCRIPT_GCC} ${example_dir}/gcc.ld @ONLY)
        configure_file(${LINKER_SCRIPT_ARM} ${example_dir}/armclang.sct @ONLY)

        configure_file(CMakeLists.txt.in ${example_dir}/CMakeLists.txt @ONLY)
        configure_file(${README_TEMPLATE_FILE} ${example_dir}/ReadMe.md @ONLY)

        file(COPY
            cmake
            example
            src
            cmsis-config
            DESTINATION ${example_dir}
        )

        if(rtos STREQUAL "cmsis-freertos")
            file(COPY freertos-config DESTINATION ${example_dir})
        endif()

    endforeach()

endforeach()
