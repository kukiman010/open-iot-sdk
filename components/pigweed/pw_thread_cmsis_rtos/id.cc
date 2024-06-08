/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pw_thread/id.h"

#include "cmsis_os2.h"
#include "pw_assert/check.h"
#include "pw_thread_cmsis_rtos/config.h"

#include <cinttypes>
#include <cstddef>
#include <cstdint>

namespace pw::this_thread {
thread::Id get_id() noexcept
{
    // Returns nullptr when called from main().
    return osThreadGetId();
}
} // namespace pw::this_thread
