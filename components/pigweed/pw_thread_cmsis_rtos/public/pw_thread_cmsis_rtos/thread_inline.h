/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_THREAD_CMSIS_RTOS_THREAD_INLINE_H
#define PW_THREAD_CMSIS_RTOS_THREAD_INLINE_H

#include "pw_assert/check.h"
#include "pw_thread/id.h"
#include "pw_thread_cmsis_rtos/config.h"

#include <algorithm>

namespace pw::thread {
inline Thread::Thread() : native_type_(nullptr)
{
}

inline Thread &Thread::operator=(Thread &&other)
{
    native_type_ = other.native_type_;
    other.native_type_ = nullptr;
    return *this;
}

inline Thread::~Thread()
{
    PW_CHECK_PTR_EQ(native_type_, nullptr, "thread not detached or joined");
}

inline void Thread::swap(Thread &other)
{
    std::swap(native_type_, other.native_type_);
}

inline Thread::native_handle_type Thread::native_handle()
{
    return native_type_;
}
} // namespace pw::thread

#endif // ! PW_THREAD_CMSIS_RTOS_THREAD_INLINE_H
