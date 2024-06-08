# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if("ml-embedded-evaluation-kit" IN_LIST IOTSDK_FETCH_LIST)
    FetchContent_MakeAvailable(ml-embedded-evaluation-kit)
endif()

if(ml-embedded-evaluation-kit_POPULATED)
    include(BuildMlEmbeddedEvaluationKit.cmake)
endif()
