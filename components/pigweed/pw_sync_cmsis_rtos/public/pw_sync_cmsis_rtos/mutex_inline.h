/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_MUTEX_INLINE_H
#define PW_SYNC_CMSIS_RTOS_MUTEX_INLINE_H

#include "cmsis_os2.h"
#include "pw_sync_cmsis_rtos/mutex_native.h"

namespace pw::sync {

inline Mutex::Mutex()
{
    // Do nothing.
}

inline Mutex::~Mutex()
{
    // Do nothing.
}

inline void Mutex::lock()
{
    native_type_.lock();
}

inline bool Mutex::try_lock()
{
    return native_type_.try_lock();
}

inline void Mutex::unlock()
{
    native_type_.unlock();
}

inline Mutex::native_handle_type Mutex::native_handle()
{
    return native_type_.mutex_id();
}

} // namespace pw::sync

#endif // ! PW_SYNC_CMSIS_RTOS_MUTEX_INLINE_H
