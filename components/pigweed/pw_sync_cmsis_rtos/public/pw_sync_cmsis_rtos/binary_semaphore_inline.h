/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_BINARY_SEMAPHORE_INLINE_H
#define PW_SYNC_CMSIS_RTOS_BINARY_SEMAPHORE_INLINE_H

#include "cmsis_os2.h"
#include "pw_chrono/system_clock.h"

#include <algorithm>

namespace pw::sync {
inline BinarySemaphore::BinarySemaphore()
{
    // Do nothing.
}

inline BinarySemaphore::~BinarySemaphore()
{
    // Do nothing.
}

inline void BinarySemaphore::release()
{
    native_type_.release(1);
}

inline void BinarySemaphore::acquire()
{
    native_type_.acquire();
}

inline bool BinarySemaphore::try_acquire() noexcept
{
    return native_type_.try_acquire();
}

inline bool BinarySemaphore::try_acquire_for(chrono::SystemClock::duration timeout)
{
    return native_type_.try_acquire_for(std::max(static_cast<int64_t>(1), timeout.count()));
}

inline bool BinarySemaphore::try_acquire_until(chrono::SystemClock::time_point deadline)
{
    return native_type_.try_acquire_for((deadline - chrono::SystemClock::now()).count());
}

inline BinarySemaphore::native_handle_type BinarySemaphore::native_handle()
{
    return native_type_.semaphore_id();
}
} // namespace pw::sync

#endif // ! PW_SYNC_CMSIS_RTOS_BINARY_SEMAPHORE_INLINE_H
