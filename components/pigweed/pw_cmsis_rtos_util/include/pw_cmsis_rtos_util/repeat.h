/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_CMSIS_RTOS_UTIL_REPEAT_H
#define PW_CMSIS_RTOS_UTIL_REPEAT_H

#include "cmsis_os2.h"
#include "pw_function/function.h"

#include <algorithm>
#include <limits>
#include <type_traits>

namespace pw_cmsis_rtos_util {
// Some CMSIS RTOS functions don't time out when timeout == osWaitForever == UINT32_MAX so we use slightly shorter
// waits.
static constexpr int64_t less_than_forever = static_cast<int64_t>(osWaitForever - 2);

/// Call 'callback' until it returns false or ticks has been consumed.
/// Used to call CMSIS RTOS API functions, which take a 32-bit timeout, repeatedly to exhaust a 64-bit one.
/// See @ref repeat_with_delay for an example.
/// @tparam ResultT Any default-constructible type which should be returned.
/// @param ticks 64-bit ticks counter. The value osWaitForever (UINT32_MAX) does not have its special meaning in this
/// function.
/// @param callback A function which takes a uint32_t input parameter and uint32_t& and ResultT& output parameters. The
/// input parameter is the number of ticks to consume. The first output parameter should be set to the number of ticks
/// actually consumed (if different - otherwise it does not have to be set). The last output parameter can be set
/// to provide the return value of repeat_for_ticks. The function returns true if the loop should continue or false
/// if it should break.
/// @return The value set by callback() if any, otherwise the default value of the type.
template <typename ResultT>
inline ResultT repeat_for_ticks(int64_t ticks, const pw::Function<ResultT(uint32_t, uint32_t &, ResultT &)> &callback)
{
    static_assert(std::is_default_constructible_v<ResultT>);

    ResultT result = ResultT();

    while (ticks > 0) {
        uint32_t ticks32 = static_cast<uint32_t>(std::min(ticks, less_than_forever));
        if (!callback(ticks32, ticks32, result)) {
            break;
        }

        ticks -= ticks32;
    }

    return result;
}

/// Like repeat_for_ticks where the time-consuming call is osDelay().
/// @note This would be a wrapper around @ref repeat_for_ticks, but a lambda which captures another lambda can't bind to
/// @ref pw::Function.
template <typename ResultT>
inline ResultT repeat_with_delay(int64_t ticks, uint32_t delay, const pw::Function<bool(ResultT &)> &callback)
{
    static_assert(std::is_default_constructible_v<ResultT>);

    ResultT result = ResultT();

    while (ticks > 0) {
        if (!callback(result)) {
            break;
        }

        osDelay(delay);

        ticks -= delay;
    }

    return result;
}

/// Version of @ref repeat_for_ticks where no return value is needed. Everything is the same except that the
/// ResultT& parameter to callback is removed.
inline void repeat_for_ticks(int64_t ticks, const pw::Function<bool(uint32_t, uint32_t &)> &callback)
{
    while (ticks > 0) {
        uint32_t ticks32 = static_cast<uint32_t>(std::min(ticks, less_than_forever));
        if (!callback(ticks32, ticks32)) {
            break;
        }

        ticks -= ticks32;
    }
}

/// Version of @ref repeat_with_delay where no return value is needed. Everything is the same except that the
/// ResultT& parameter to callback is removed.
inline void repeat_with_delay(int64_t ticks, uint32_t delay, const pw::Function<bool()> &callback)
{
    while (ticks > 0) {
        if (!callback()) {
            break;
        }

        osDelay(delay);

        ticks -= delay;
    }
}
} // namespace pw_cmsis_rtos_util

#endif // ! PW_CMSIS_RTOS_UTIL_REPEAT_H
