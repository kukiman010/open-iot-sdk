/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_TIMED_THREAD_NOTIFICATION_INLINE_H
#define PW_SYNC_CMSIS_RTOS_TIMED_THREAD_NOTIFICATION_INLINE_H

#include "pw_chrono/system_clock.h"
#include "pw_cmsis_rtos_util/repeat.h"
#include "pw_sync/timed_thread_notification.h"

#include <algorithm>

namespace pw::sync {
inline bool TimedThreadNotification::try_acquire_for(chrono::SystemClock::duration timeout)
{
    return pw_cmsis_rtos_util::repeat_with_delay<bool>(timeout.count(), 1U, [this](bool &acquired) {
        if (this->try_acquire()) {
            acquired = true;
            return false;
        }

        return true;
    });
}

inline bool TimedThreadNotification::try_acquire_until(chrono::SystemClock::time_point deadline)
{
    return try_acquire_for(deadline - chrono::SystemClock::now());
}

} // namespace pw::sync

#endif // ! PW_SYNC_CMSIS_RTOS_TIMED_THREAD_NOTIFICATION_INLINE_H
