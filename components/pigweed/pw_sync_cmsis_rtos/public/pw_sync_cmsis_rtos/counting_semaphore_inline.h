/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_COUNTING_SEMAPHORE_INLINE_H
#define PW_SYNC_CMSIS_RTOS_COUNTING_SEMAPHORE_INLINE_H

#include "cmsis_os2.h"
#include "pw_sync/counting_semaphore.h"

#include <limits>

namespace pw::sync {
inline CountingSemaphore::CountingSemaphore()
{
    // Do nothing.
}

inline CountingSemaphore::~CountingSemaphore()
{
    // Do nothing.
}

inline void CountingSemaphore::release(ptrdiff_t update)
{
    native_type_.release(update);
}

inline void CountingSemaphore::acquire()
{
    return native_type_.acquire();
}

inline bool CountingSemaphore::try_acquire() noexcept
{
    return native_type_.try_acquire();
}

inline bool CountingSemaphore::try_acquire_for(chrono::SystemClock::duration timeout)
{
    return native_type_.try_acquire_for(std::max(static_cast<int64_t>(1), timeout.count()));
}

inline bool CountingSemaphore::try_acquire_until(chrono::SystemClock::time_point deadline)
{
    return native_type_.try_acquire_for((deadline - chrono::SystemClock::now()).count());
}

inline CountingSemaphore::native_handle_type CountingSemaphore::native_handle()
{
    return native_type_.semaphore_id();
}

} // namespace pw::sync

#endif // ! PW_SYNC_CMSIS_RTOS_COUNTING_SEMAPHORE_INLINE_H
