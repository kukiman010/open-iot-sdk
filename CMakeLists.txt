# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# IoT-SDK

cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

project(open-iot-sdk LANGUAGES C CXX ASM)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

set(IOTSDK_FETCH_LIST "" CACHE STRING
    "List of Open IoT SDK components to fetch. See components/DeclareSdkComponents.cmake for available components.")

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    include(AddTestTools)
endif()

if("trusted-firmware-m" IN_LIST IOTSDK_FETCH_LIST)
    set(TFM_PLATFORM "" CACHE STRING "Value of -DTFM_PLATFORM= passed to TF-M")
    option(TFM_PSA_FIRMWARE_UPDATE "Enable PSA Firmware Update API in TF-M")
    set(MCUBOOT_IMAGE_VERSION_NS "0.0.0" CACHE STRING "Version number of non-secure image (in the x.x.x format)")
    set(TFM_CMAKE_ARGS "" CACHE STRING "Additional CMake arguments passed to TF-M (semicolon-separated)")
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/components/TF-M)
    set(TFM_PATCH_COMMAND "" CACHE STRING "Command to patch TF-M")
endif()

if("ml-embedded-evaluation-kit" IN_LIST IOTSDK_FETCH_LIST)
    set(ML_PATCH_COMMAND "" CACHE STRING "Command to patch ML Embedded Evaluation Kit")
    set(ML_TARGETS "all" CACHE STRING "Targets to build from the ML Embedded Evaluation Kit")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Enable features of project_options.
    # Note: We need to enable project_options after adding fetched components
    # and before adding local components, to ensure options are only applied to
    # local components.
    FetchContent_Declare(
        project_options
        GIT_REPOSITORY  https://github.com/cpp-best-practices/project_options.git
        GIT_TAG         25196f0756ce52166b53453b887e462f6d547291
    )
    FetchContent_MakeAvailable(project_options)
    include(${project_options_SOURCE_DIR}/Index.cmake)
    include(${project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

    # cppcheck will be run by pre-commit checks
    set(ENABLE_CPPCHECK_DEVELOPER_DEFAULT OFF)

    include(AddToolchainWarningFlags)
    set(GCC_WARNINGS ${IOTSDK_WARNING_FLAGS} ${IOTSDK_WARNING_AS_ERRORS_FLAGS})
    set(CLANG_WARNINGS ${GCC_WARNINGS})

    dynamic_project_options(
        CLANG_WARNINGS
        ${CLANG_WARNINGS}
        GCC_WARNINGS
        ${GCC_WARNINGS}
    )
    if (OPT_ENABLE_COVERAGE AND NOT CMAKE_CROSSCOMPILING)
        include(GenerateCodeCoverageReport)
    endif()
endif()

add_subdirectory(utils)

if(CMAKE_CROSSCOMPILING)
    add_subdirectory(components EXCLUDE_FROM_ALL)
else()
    add_subdirectory(components)
endif()
