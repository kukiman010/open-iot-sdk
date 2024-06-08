/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_INLINE_H
#define PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_INLINE_H

#include "cmsis_os2.h"
#include "pw_log/log.h"
#include "pw_sync/thread_notification.h"
#include "pw_sync_cmsis_rtos/thread_notification_native.h"

namespace pw::sync {
inline ThreadNotification::ThreadNotification()
{
    // Do nothing.
}

inline ThreadNotification::~ThreadNotification()
{
    // Do nothing.
}

inline void ThreadNotification::acquire()
{
    native_type_.acquire();
}

inline bool ThreadNotification::try_acquire()
{
    return native_type_.try_acquire();
}

inline void ThreadNotification::release()
{
    native_type_.release();
}

inline ThreadNotification::native_handle_type ThreadNotification::native_handle()
{
    return native_type_;
}

} // namespace pw::sync

#endif // ! PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_INLINE_H
