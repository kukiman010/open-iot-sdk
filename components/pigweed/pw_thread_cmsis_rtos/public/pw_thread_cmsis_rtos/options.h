/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_THREAD_CMSIS_RTOS_OPTIONS_H
#define PW_THREAD_CMSIS_RTOS_OPTIONS_H

#include "cmsis_os2.h"
#include "pw_thread/thread.h"
#include "pw_thread_cmsis_rtos/backend.h"

#include <cstdint>
#include <cstring>

namespace pw::thread::cmsis_rtos {
/// Options for Thread. Wraps osThreadAttr_t.
struct Options : thread::Options {
    Options()
    {
        std::memset(&_attr, 0, sizeof(_attr));

        // CMSIS RTX threads are detached by default but Pigweed threads should be joinable by default.
        set_flags(osThreadJoinable);
    }

    inline ~Options()
    {
        if (is_name_dynamically_allocated()) {
            delete[] _attr.name;
        }
    }

    /// Set the thread's name.
    /// @param name The name to set.
    /// @param dup Whether to duplicate the string. This should be set when 'name' points to dynamic memory.
    inline Options &set_name(const char *name, bool dup = false)
    {
        if (is_name_dynamically_allocated()) {
            delete[] _attr.name;
            set_name_dynamically_allocated(false);
        }

        if (dup) {
            std::size_t len = std::strlen(name);
            char *s = new (std::nothrow) char[len + 1];
            PW_CHECK_NOTNULL(s, "could not allocate %zu B", len + 1);

            s[len] = 0;
            std::memcpy(s, name, len);
            _attr.name = s;
            set_name_dynamically_allocated(true);
        } else {
            _attr.name = name;
        }

        return *this;
    }

    /// Get the name.
    inline const char *get_name() const
    {
        return _attr.name;
    }

    /// Set the flags.
    inline Options &set_flags(uint32_t flags)
    {
        _attr.attr_bits = flags;
        return *this;
    }

    /// Get the flags.
    inline uint32_t get_flags() const
    {
        return _attr.attr_bits;
    }

    /// Get a modifiable reference to the flags.
    inline uint32_t &flags()
    {
        return _attr.attr_bits;
    }

    /// Set the control block memory.
    inline Options &set_cb_mem(void *cb_mem)
    {
        _attr.cb_mem = cb_mem;
        return *this;
    }

    /// Get the control block memory.
    inline void *get_cb_mem()
    {
        return _attr.cb_mem;
    }

    /// Get the control block memory.
    inline const void *get_cb_mem() const
    {
        return _attr.cb_mem;
    }

    /// Set the size of control block memory.
    inline Options &set_cb_size(uint32_t cb_size)
    {
        _attr.cb_size = cb_size;
        return *this;
    }

    /// Get the size of control block memory.
    inline uint32_t get_cb_size() const
    {
        return _attr.cb_size;
    }

    /// Set the stack memory.
    inline Options &set_stack_mem(void *stack_mem)
    {
        _attr.stack_mem = stack_mem;
        return *this;
    }

    /// Get the stack memory.
    inline void *get_stack_mem()
    {
        return _attr.stack_mem;
    }

    /// Get the stack memory.
    inline const void *get_stack_mem() const
    {
        return _attr.stack_mem;
    }

    /// Set the stack size.
    inline Options &set_stack_size(uint32_t stack_size)
    {
        _attr.stack_size = stack_size;
        return *this;
    }

    /// Get the stack size.
    inline uint32_t get_stack_size() const
    {
        return _attr.stack_size;
    }

    /// Set the priority.
    inline Options &set_priority(osPriority_t priority)
    {
        _attr.priority = priority;
        return *this;
    }

    /// Get the priority.
    inline osPriority_t get_priority()
    {
        return _attr.priority;
    }

    /// Get the priority.
    inline osPriority_t get_priority() const
    {
        return _attr.priority;
    }

    /// Set the TrustZone module ID.
    inline Options &set_tz_module(TZ_ModuleId_t tz_module)
    {
        _attr.tz_module = tz_module;
        return *this;
    }

    /// Get the TrustZone module ID.
    inline TZ_ModuleId_t get_tz_module() const
    {
        return _attr.tz_module;
    }

    /// Get a pointer to the wrapped osThreadAttr_t if attribute values have been changed. Otherwise returns nullptr.
    inline const osThreadAttr_t *get_thread_attr() const
    {
        return &_attr;
    }

    Options(Options &other) = delete;
    inline Options &operator=(Options &other) = delete;
    Options(Options &&other) = delete;
    inline Options &operator=(Options &&other) = delete;

private:
    enum { NAME_DYNAMIC = 1 << 1 };

    osThreadAttr_t _attr;
    uint8_t _flags = 0;

    inline bool is_name_dynamically_allocated()
    {
        return (_flags & NAME_DYNAMIC) == NAME_DYNAMIC;
    }

    inline void set_name_dynamically_allocated(bool value)
    {
        if (value) {
            _flags |= NAME_DYNAMIC;
        } else {
            _flags &= ~NAME_DYNAMIC;
        }
    }
};
} // namespace pw::thread::cmsis_rtos

#endif // ! PW_THREAD_CMSIS_RTOS_OPTIONS_H
