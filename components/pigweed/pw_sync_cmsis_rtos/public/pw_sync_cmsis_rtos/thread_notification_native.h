/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_NATIVE_H
#define PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_NATIVE_H

#include "cmsis_os2.h"

namespace pw::sync::backend {
// A single subscriber, multiple notifier notification. One thread waits and may be awoken by any other thread.
struct NativeThreadNotification {
    NativeThreadNotification();

    bool try_acquire();

    bool try_acquire_for(int64_t ticks);

    void acquire();

    void release();

private:
    osThreadId_t blocked_thread_id_;
    bool notified_;

    uint32_t try_acquire_for_with_status(int64_t ticks);
};

using NativeThreadNotificationHandle = NativeThreadNotification &;
} // namespace pw::sync::backend

#endif // ! PW_SYNC_CMSIS_RTOS_THREAD_NOTIFICATION_NATIVE_H
