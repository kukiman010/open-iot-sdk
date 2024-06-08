# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

function(set_pw_backend FACADE BACKEND)
    set(${FACADE}_BACKEND ${BACKEND} CACHE STRING "" FORCE)
endfunction()

set_pw_backend(pw_assert.assert pw_assert_log.assert_backend)
set_pw_backend(pw_assert.check pw_assert_log.check_backend)
set_pw_backend(pw_chrono.system_clock pw_chrono_cmsis_rtos.system_clock)
set_pw_backend(pw_chrono.system_timer pw_chrono_cmsis_rtos.system_timer)
set_pw_backend(pw_interrupt.context pw_interrupt_cortex_m.context)
set_pw_backend(pw_log pw_log_sys_io_mdh)
set_pw_backend(pw_sync.binary_semaphore pw_sync_cmsis_rtos.binary_semaphore)
set_pw_backend(pw_sync.counting_semaphore pw_sync_cmsis_rtos.counting_semaphore)
set_pw_backend(pw_sync.interrupt_spin_lock pw_sync_cmsis_rtos.interrupt_spin_lock)
set_pw_backend(pw_sync.mutex pw_sync_cmsis_rtos.mutex)
set_pw_backend(pw_sync.thread_notification pw_sync_cmsis_rtos.thread_notification)
set_pw_backend(pw_sync.timed_mutex pw_sync_cmsis_rtos.timed_mutex)
set_pw_backend(pw_sync.timed_thread_notification pw_sync_cmsis_rtos.timed_thread_notification)
set_pw_backend(pw_sys_io pw_sys_io_mcu_driver_hal)
set_pw_backend(pw_thread.id pw_thread_cmsis_rtos.id)
set_pw_backend(pw_thread.sleep pw_thread_cmsis_rtos.sleep)
set_pw_backend(pw_thread.thread pw_thread_cmsis_rtos.thread)
set_pw_backend(pw_thread.yield pw_thread_cmsis_rtos.yield)
set_pw_backend(pw_unit_test.main pw_unit_test_cmsis_rtos.main)

set(pw_unit_test_MAIN pw_unit_test_cmsis_rtos.main CACHE STRING "" FORCE)
