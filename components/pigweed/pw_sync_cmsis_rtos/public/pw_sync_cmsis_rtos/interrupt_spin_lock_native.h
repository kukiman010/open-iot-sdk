/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_INTERRUPT_SPINLOCK_NATIVE_H
#define PW_SYNC_CMSIS_RTOS_INTERRUPT_SPINLOCK_NATIVE_H

#include "cmsis_os2.h"
#include "pw_assert/check.h"
#include "pw_sync/mutex.h"

namespace pw::sync::backend {
// Implements the lock/try_lock/unlock portion of the pw::sync::InterruptSpinLock API. This is used to implement a lock
// which synchronises threads and ISRs by locking a mutex and disabling IRQs except for SVC (due to its use by CMSIS
// RTOS) and NMI (in accordance with the InterruptSpinLock API).
// Multiple InterruptSpinLocks are allowed to be held simultaneously but the same ISL can't be locked multiple times
// due to the internal mutex.
struct NativeInterruptSpinLock {
    constexpr NativeInterruptSpinLock() : locked_(false), owner_(&invalid_thread_id_), saved_irq_state_(0)
    {
    }

    void lock();

    bool try_lock();

    void unlock();

private:
    using irq_state_t = uint32_t;

    // In CMSIS RTOS, main/the scheduler has a thread ID of NULL so this variable's address will be used to represent
    // the invalid/empty thread ID.
    static uint8_t invalid_thread_id_;

    static volatile int nested_critical_sections_;

    bool locked_;
    osThreadId_t owner_;
    irq_state_t saved_irq_state_;

    void enter_critical_section();

    void exit_critical_section();

    void set_target_irq_state();

    void clear_irq_state();

    void restore_saved_irq_state();
};

using NativeInterruptSpinLockHandle = NativeInterruptSpinLock &;
} // namespace pw::sync::backend

#endif // ! PW_SYNC_CMSIS_RTOS_INTERRUPT_SPINLOCK_NATIVE_H
