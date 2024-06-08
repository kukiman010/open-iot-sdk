/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_THREAD_CMSIS_RTOS_NATIVE_H_
#define PW_THREAD_CMSIS_RTOS_NATIVE_H_

#include "cmsis_os2.h"
#include "pw_thread_cmsis_rtos/config.h"

#include <type_traits> // pw_thread/thread.h depends on std::bool_constant, but doesn't include the header for it ¯\_(ツ)_/¯

namespace pw::thread::cmsis_rtos {
struct Context;
} // namespace pw::thread::cmsis_rtos

namespace pw::thread::backend {
using NativeThread = cmsis_rtos::Context *;

using NativeThreadHandle = NativeThread;
} // namespace pw::thread::backend

#endif /* PW_THREAD_CMSIS_RTOS_NATIVE_H_ */
