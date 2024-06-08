/*
 * Copyright (c) 2022-2023, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_INTERNAL_COUNTING_SEMAPHORE_H
#define PW_SYNC_CMSIS_RTOS_INTERNAL_COUNTING_SEMAPHORE_H

#include "cmsis_os2.h"
#include "pw_assert/check.h"
#include "pw_cmsis_rtos_util/repeat.h"
#include "pw_sync_cmsis_rtos/config.h"
#include "pw_sync_cmsis_rtos/internal/mutex.h"

#include <algorithm>
#include <atomic>
#include <cinttypes>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace pw::sync::cmsis_rtos {
template <class CounterT, CounterT Max> struct InternalCountingSemaphore {
    using counter_type = CounterT;

    // The actual max count is the minimum of the user's max, the max for CMSIS RTOS API (uint32_max), the max of the
    // counter type, and the underlying RTOS' max.
    static constexpr counter_type _get_max_count()
    {
        constexpr uintmax_t uint32_max = static_cast<uintmax_t>(std::numeric_limits<uint32_t>::max());
        constexpr uintmax_t counter_max = static_cast<uintmax_t>(std::numeric_limits<counter_type>::max());
        constexpr uintmax_t user_max = static_cast<uintmax_t>(Max);
        constexpr uintmax_t rtos_max = static_cast<uintmax_t>(PW_SYNC_CMSIS_RTOS_SEMAPHORE_MAX);

        return static_cast<counter_type>(std::min(counter_max, std::min(uint32_max, std::min(user_max, rtos_max))));
    }

    static constexpr counter_type max_count = _get_max_count();

    InternalCountingSemaphore()
    {
        semaphore_ = osSemaphoreNew(static_cast<uint32_t>(max_count), 0, nullptr);
        PW_CHECK_NOTNULL(semaphore_, "osSemaphoreNew failed");
    }

    void release(counter_type update)
    {
        while (update > 0) {
            osStatus_t status = osSemaphoreRelease(semaphore_);
            if (status == osErrorResource) {
                return;
            }

            PW_CHECK_INT_EQ(status, osOK, "osSemaphoreRelease failed");
            update--;
        }
    }

    bool try_acquire_for(int64_t ticks)
    {
        return pw_cmsis_rtos_util::repeat_for_ticks<bool>(
            ticks, [this](uint32_t ticks_in, uint32_t &ticks_out, bool &acquired) {
                (void)ticks_out;

                osStatus_t status = osSemaphoreAcquire(semaphore_, ticks_in);
                if (status == osOK || status == osErrorTimeout) {
                    acquired = status == osOK;
                    return false;
                }

                PW_CHECK_INT_EQ(status, osOK, "osSemaphoreAcquire failed");

                return true;
            });
    }

    bool try_acquire()
    {
        osStatus_t status = osSemaphoreAcquire(semaphore_, 0);
        if (status == osOK) {
            return true;
        }

        // Semaphore is exhausted.
        if (status == osErrorResource) {
            return false;
        }

        PW_CHECK_INT_EQ(status, osOK, "osSemaphoreAcquire failed");
        return false;
    }

    void acquire()
    {
        osStatus_t status = osSemaphoreAcquire(semaphore_, osWaitForever);
        PW_CHECK_INT_EQ(status, osOK, "osSemaphoreAcquire failed");
    }

    counter_type count() const
    {
        return static_cast<counter_type>(osSemaphoreGetCount(semaphore_));
    }

    osSemaphoreId_t semaphore_id()
    {
        return semaphore_;
    }

    InternalCountingSemaphore(const InternalCountingSemaphore &) = delete;
    InternalCountingSemaphore(InternalCountingSemaphore &&) = delete;
    InternalCountingSemaphore &operator=(const InternalCountingSemaphore &) = delete;
    InternalCountingSemaphore &operator=(InternalCountingSemaphore &&) = delete;

private:
    osSemaphoreId_t semaphore_;
};
} // namespace pw::sync::cmsis_rtos

#endif // ! PW_SYNC_CMSIS_RTOS_INTERNAL_COUNTING_SEMAPHORE_H
