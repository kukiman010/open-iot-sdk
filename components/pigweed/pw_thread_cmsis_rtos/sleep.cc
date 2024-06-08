/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pw_thread/sleep.h"

#include "cmsis_os2.h"
#include "pw_chrono/system_clock.h"
#include "pw_log/log.h"
#include "pw_thread/id.h"
#include "pw_thread/thread.h"

#include <algorithm>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <limits>

using pw::chrono::SystemClock;

namespace {
constexpr int64_t MAX_TICKS = std::numeric_limits<uint32_t>::max() - 2;
}

namespace pw::this_thread {
void sleep_until(SystemClock::time_point deadline)
{
    sleep_for(std::clamp(deadline - SystemClock::now(), SystemClock::duration(1), SystemClock::duration(MAX_TICKS)));
}

void sleep_for(SystemClock::duration sleep_duration)
{
    PW_CHECK_PTR_NE(get_id(), thread::Id());
    int64_t ticks = sleep_duration.count();
    while (ticks > 0) {
        uint32_t ticks32 = uint32_t(std::clamp(ticks, int64_t(1), int64_t(MAX_TICKS)));

        osDelay(ticks32);

        ticks -= int64_t(ticks32);
    }
}
} // namespace pw::this_thread
