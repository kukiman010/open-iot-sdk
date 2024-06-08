/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "cmsis_os2.h"
#include "pw_assert/check.h"
#include "pw_interrupt/context.h"
#include "pw_sync/interrupt_spin_lock.h"
#include "pw_sync_cmsis_rtos/config.h"
#include "pw_sync_cmsis_rtos/interrupt_spin_lock_native.h"

extern "C" {
#include "cmsis_compiler.h"
}

#define USER_NMI_PRIORITY 16

namespace pw::sync::backend {
uint8_t NativeInterruptSpinLock::invalid_thread_id_;

volatile int NativeInterruptSpinLock::nested_critical_sections_ = 0;

bool NativeInterruptSpinLock::try_lock()
{
    if (interrupt::InInterruptContext()) {
        set_target_irq_state();
    } else {
        enter_critical_section();
    }

    PW_DCHECK(!locked_, "Recursive InterruptSpinLock::lock() detected");

    locked_ = true;

    return true;
}

void NativeInterruptSpinLock::unlock()
{
    locked_ = false;

    if (interrupt::InInterruptContext()) {
        clear_irq_state();
    } else {
        exit_critical_section();
    }
}

void NativeInterruptSpinLock::enter_critical_section()
{
    set_target_irq_state();
    nested_critical_sections_++;
    __DSB();
    __ISB();
}

void NativeInterruptSpinLock::exit_critical_section()
{
    nested_critical_sections_--;
    if (nested_critical_sections_ == 0) {
        restore_saved_irq_state();
    }
}

void NativeInterruptSpinLock::set_target_irq_state()
{
    saved_irq_state_ = __get_BASEPRI();
    __set_BASEPRI(USER_NMI_PRIORITY);
    __DSB();
    __ISB();
}

void NativeInterruptSpinLock::clear_irq_state()
{
    __set_BASEPRI(0);
    __DSB();
    __ISB();
}

void NativeInterruptSpinLock::restore_saved_irq_state()
{
    __set_BASEPRI(saved_irq_state_);
    __DSB();
    __ISB();
}
} // namespace pw::sync::backend
