/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "cmsis_os2.h"

#include <cinttypes>
#include <cstdarg>

#define PW_LOG_MODULE_NAME "main"

#include "pw_assert/assert.h"
#include "pw_chrono/system_clock.h"
#include "pw_chrono/system_timer.h"
#include "pw_log/log.h"
#include "pw_log_mdh/backend.h"
#include "pw_sync/interrupt_spin_lock.h"

using pw::chrono::SystemClock;
using pw::chrono::SystemTimer;
using pw::sync::InterruptSpinLock;

using ticks_count_t = int64_t;

static constexpr ticks_count_t TICKS_MAX = 550;
static constexpr ticks_count_t INITIAL_TICKS = 10;

extern "C" mdh_serial_t *get_example_serial();

static ticks_count_t times_ticked;
static bool ticked;
static InterruptSpinLock spinlock;

// Converts the tick count sequence from [INITIAL_TICKS, 2*INITIAL_TICKS, ...] to Fibonacci numbers
static inline constexpr ticks_count_t convert_ticks(ticks_count_t actual_ticks)
{
    return actual_ticks / INITIAL_TICKS;
}

static void serial_setup(void)
{
    mdh_serial_t *my_serial = get_example_serial();
    mdh_serial_set_baud(my_serial, 115200);
    pw_log_mdh_init(my_serial);
}

static void timer_cb(SystemClock::time_point expired_deadline)
{
    static SystemClock::time_point previous_deadline;

    ticks_count_t elapsed = convert_ticks((expired_deadline - previous_deadline).count());

    previous_deadline = expired_deadline;

    PW_LOG_INFO("timer fired after %" PRId64 " ticks", elapsed);
    spinlock.lock();
    times_ticked++;
    ticked = true;
    spinlock.unlock();
}

static void main_task_cb(void *ignored)
{
    uint32_t prev_ticks = 0;
    uint32_t curr_ticks = INITIAL_TICKS;

    SystemTimer timer(timer_cb);

    // Invoke timer to call timer_cb N times (N depends on TICKS_MAX). For testing, this shows that the timer works with
    // different values.
    while (curr_ticks <= TICKS_MAX) {
        spinlock.lock();

        {
            ticked = false;
            timer.InvokeAfter(SystemClock::duration(curr_ticks));
            curr_ticks += prev_ticks;
            prev_ticks = curr_ticks - prev_ticks;
        }

        spinlock.unlock();

        // Wait here until unblocked by timer_cb.
        while (!ticked) {
            osThreadYield();
        }
    }

    PW_LOG_INFO("timer has fired %" PRId64 " times, checking if a timer fires after cancelling", times_ticked);

    // Invoke a timer and cancel it to demonstrate that the timer does not fire.
    static SystemClock::duration ten_ms(10);
    auto now = SystemClock::now();
    auto then = now + ten_ms;
    timer.InvokeAt(then);
    timer.Cancel();
    while (SystemClock::now() < then) {
        // Busy waiting until 10 ms expires.
    }

    PW_LOG_INFO("timer has fired %" PRId64 " times", times_ticked);

    ((void)ignored);
}

int main(void)
{
    serial_setup();

    auto res = osKernelInitialize();
    PW_CHECK_INT_EQ(res, 0, "osKernelInitialize failed");

    auto thread_id = osThreadNew(main_task_cb, nullptr, nullptr);
    PW_CHECK_PTR_NE(thread_id, nullptr, "osThreadNew failed");

    auto state = osKernelGetState();
    PW_CHECK_INT_EQ(state, osKernelReady, "kernel is not ready");

    res = osKernelStart();
    PW_CHECK_INT_EQ(res, osOK, "osKernelStart failed");

    PW_UNREACHABLE;
}
