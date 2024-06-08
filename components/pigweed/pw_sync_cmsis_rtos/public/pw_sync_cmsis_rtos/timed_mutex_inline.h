/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_TIMED_MUTEX_INLINE_H
#define PW_SYNC_CMSIS_RTOS_TIMED_MUTEX_INLINE_H

#include "pw_chrono/system_clock.h"
#include "pw_cmsis_rtos_util/repeat.h"
#include "pw_function/function.h"
#include "pw_sync/timed_mutex.h"

namespace pw::sync {
inline bool TimedMutex::try_lock_for(chrono::SystemClock::duration timeout)
{
    return pw_cmsis_rtos_util::repeat_with_delay<bool>(timeout.count(), 1U, [this](bool &acquired) {
        if (this->try_lock()) {
            acquired = true;
            return false;
        }

        return true;
    });
}

inline bool TimedMutex::try_lock_until(chrono::SystemClock::time_point deadline)
{
    return try_lock_for(deadline - chrono::SystemClock::now());
}

} // namespace pw::sync

#endif // ! PW_SYNC_CMSIS_RTOS_TIMED_MUTEX_INLINE_H
