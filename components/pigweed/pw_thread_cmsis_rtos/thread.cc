/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pw_thread/thread.h"

#include "cmsis_os2.h"
#include "pw_cmsis_rtos_util/kernel_lock.h"
#include "pw_log/log.h"
#include "pw_sync/thread_notification.h"
#include "pw_thread/id.h"
#include "pw_thread/thread_core.h"
#include "pw_thread_cmsis_rtos/config.h"
#include "pw_thread_cmsis_rtos/id_native.h"
#include "pw_thread_cmsis_rtos/options.h"
#include "pw_thread_cmsis_rtos/thread_native.h"

#include <cinttypes>
#include <cstddef>
#include <cstdint>

// Supply our own implementation of join when joining is enabled but not supplied by CMSIS RTOS (FreeRTOS backend).
#define REQUIRE_CUSTOM_JOIN_DETACH ((PW_THREAD_JOINING_ENABLED) && !(PW_THREAD_CMSIS_RTOS_HAVE_JOIN_DETACH))

using pw::sync::ThreadNotification;
using pw_cmsis_rtos_util::KernelLockGuard;

namespace pw::thread {
namespace cmsis_rtos {
static void thread_entry_point(void *context_ptr);

struct Context {
    enum { COMPLETED = 1 << 1, DETACHED = 1 << 2 };

    Context(const Options &options_, osThreadFunc_t thread_func_, void *thread_func_arg_)
        : options_(options_), thread_func_(thread_func_), thread_func_arg_(thread_func_arg_), bits_(0)
    {
        KernelLockGuard guard;

        osThreadAttr_t thread_attr = *options_.get_thread_attr();

        // attr_bits is either osThreadDetached (0) or osThreadJoinable (1).
        // osThreadNew returns nullptr when osThreadJoinable is set and the FreeRTOS backend is in use.
        // To support detached/joinable threads with FreeRTOS we remember the caller's intention and remove the joinable
        // flag if set.
        if (thread_attr.attr_bits == osThreadDetached) {
            set_detached();
#if !PW_THREAD_CMSIS_RTOS_HAVE_JOIN_DETACH
        } else {
            thread_attr.attr_bits = osThreadDetached;
#endif // !PW_THREAD_CMSIS_RTOS_HAVE_JOIN_DETACH
        }

        thread_id_ = osThreadNew(reinterpret_cast<osThreadFunc_t>(thread_entry_point), this, &thread_attr);
        PW_CHECK_NOTNULL(thread_id_, "osThreadNew failed");
    }

    void invoke()
    {
        thread_func_(thread_func_arg_);
    }

#if REQUIRE_CUSTOM_JOIN_DETACH
    void join()
    {
        join_notification_.acquire();
    }
#endif // REQUIRE_CUSTOM_JOIN_DETACH

    inline const Options &options() const
    {
        return options_;
    }

    inline osThreadId_t thread_id()
    {
        return thread_id_;
    }

    inline bool completed() const
    {
        return (bits_ & COMPLETED) == COMPLETED;
    }

    inline bool detached() const
    {
        return (bits_ & DETACHED) == DETACHED;
    }

    inline void set_completed()
    {
        bits_ |= COMPLETED;
#if REQUIRE_CUSTOM_JOIN_DETACH
        join_notification_.release();
#endif // REQUIRE_CUSTOM_JOIN_DETACH
    }

    inline void set_detached()
    {
        bits_ |= DETACHED;
    }

    inline void clear_completed()
    {
        bits_ &= ~COMPLETED;
    }

    inline void clear_detached()
    {
        bits_ &= ~DETACHED;
    }

private:
    const Options &options_;
    osThreadId_t thread_id_;
    osThreadFunc_t thread_func_;
    void *thread_func_arg_;
    uint8_t bits_;

#if REQUIRE_CUSTOM_JOIN_DETACH
    ThreadNotification join_notification_;
#endif // REQUIRE_CUSTOM_JOIN_DETACH
};

static void thread_entry_point(void *context_ptr)
{
    Context *context = reinterpret_cast<Context *>(context_ptr);

    context->invoke();

    {
        // Enter critical section to prevent being joined or detached during cleanup.
        KernelLockGuard guard;

        // If the thread was already detached, we can clean up the resources here. Otherwise it will be done by join().
        if (context->detached()) {
            delete context;
        } else {
            context->set_completed();
        }
    }

    // Exited critical section before terminating the thread.
    osThreadExit();
    PW_UNREACHABLE;
}

} // namespace cmsis_rtos

using RtosOptions = pw::thread::cmsis_rtos::Options;
using cmsis_rtos::Context;
using cmsis_rtos::thread_entry_point;

Thread::Thread(const Options &options, ThreadRoutine entry, void *arg)
    : native_type_(
        new Context(reinterpret_cast<const RtosOptions &>(options), reinterpret_cast<osThreadFunc_t>(entry), arg))
{
    if (native_type_->detached()) {
        native_type_ = nullptr;
    }
}

#if PW_THREAD_JOINING_ENABLED
void Thread::join()
{
    PW_CHECK_NOTNULL(native_type_);
    PW_CHECK(joinable());
    PW_CHECK(!(native_type_->detached()));
    PW_CHECK(Id(osThreadGetId()) != get_id(), "thread tried to join itself");

#if PW_THREAD_CMSIS_RTOS_HAVE_JOIN_DETACH
    auto status = osThreadJoin(native_type_->thread_id());
    PW_CHECK_INT_EQ(status, osOK, "osThreadJoin failed");
#else
    native_type_->join();
#endif

    KernelLockGuard guard;
    delete native_type_;
    native_type_ = nullptr;
}
#endif // PW_THREAD_JOINING_ENABLED

void Thread::detach()
{
    PW_CHECK_NOTNULL(native_type_);
    PW_CHECK(joinable());
    PW_CHECK(!(native_type_->detached()));

    KernelLockGuard guard;

    if (native_type_->completed()) {
        delete native_type_;
        native_type_ = nullptr;
        return;
    }

#if PW_THREAD_CMSIS_RTOS_HAVE_JOIN_DETACH
    auto status = osThreadDetach(native_type_->thread_id());
    PW_CHECK_INT_EQ(status, osOK, "osThreadDetach failed");
#endif

    native_type_->set_detached();
    native_type_ = nullptr;
}

Id Thread::get_id() const
{
    if (native_type_ == nullptr) {
        return Id(nullptr);
    }

    return native_type_->thread_id();
}

} // namespace pw::thread
