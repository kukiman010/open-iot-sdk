/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_INTERRUPT_SPINLOCK_INLINE_H
#define PW_SYNC_CMSIS_RTOS_INTERRUPT_SPINLOCK_INLINE_H

#include "cmsis_os2.h"
#include "pw_sync/interrupt_spin_lock.h"

namespace pw::sync {
constexpr InterruptSpinLock::InterruptSpinLock()
{
}

inline void InterruptSpinLock::lock() PW_EXCLUSIVE_LOCK_FUNCTION()
{
    while (!try_lock()) {
        osThreadYield();
    }
}

inline bool InterruptSpinLock::try_lock() PW_EXCLUSIVE_TRYLOCK_FUNCTION(true)
{
    return native_type_.try_lock();
}

inline void InterruptSpinLock::unlock() PW_UNLOCK_FUNCTION()
{
    native_type_.unlock();
}

inline InterruptSpinLock::native_handle_type InterruptSpinLock::native_handle()
{
    return native_type_;
}
} // namespace pw::sync

#endif // ! PW_SYNC_CMSIS_RTOS_INTERRUPT_SPINLOCK_INLINE_H
