# Copyright (c) 2022 - 2023, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Add libraries useful for unit testing.

include(FetchContent)

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    FetchContent_Declare(
        mcu-driver-hal
        GIT_REPOSITORY https://git.gitlab.arm.com/iot/open-iot-sdk/mcu-driver-hal/mcu-driver-hal.git
        GIT_TAG        b92bcd2223007d615de7d64e3d93ca85533c153f
        GIT_PROGRESS   ON
    )
    FetchContent_MakeAvailable(mcu-driver-hal)
    if(IS_DIRECTORY "${mcu-driver-hal_SOURCE_DIR}")
        set_property(DIRECTORY ${mcu-driver-hal_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
    endif()

    FetchContent_Declare(
        fff
        GIT_REPOSITORY https://github.com/meekrosoft/fff.git
        GIT_SHALLOW    OFF
        GIT_TAG        11ab05b518272b1698935a6e617d7894b713c462
    )
    FetchContent_GetProperties(fff)
    FetchContent_MakeAvailable(fff)

    include(GoogleTest)

    # Mark GoogleTest gtest library's include directories as SYSTEM, so that clang-tidy
    # will not warn about issues in GoogleTest that do not comply with our
    # clang-tidy configuration.
    get_target_property(gtest_include gtest INTERFACE_INCLUDE_DIRECTORIES)

    function(iotsdk_add_test target)
        target_include_directories(${target} SYSTEM
            PRIVATE
                ${gtest_include}
        )
        target_link_libraries(${target}
            PRIVATE
                gtest
        )

        gtest_discover_tests(${target})
    endfunction()
endif()
