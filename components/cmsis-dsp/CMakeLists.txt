# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

if("cmsis-dsp" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(cmsis-dsp)
endif()

if(TARGET CMSISDSP)
    if(NOT TARGET cmsis-core)
        message(FATAL_ERROR "CMSIS-DSP depends on CMSIS-Core which is not fetched")
    endif()
    target_link_libraries(CMSISDSP
        PUBLIC
            cmsis-core
    )
endif()
