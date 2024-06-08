/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bootstrap/mbed_critical.h"
#include "pw_assert/check.h"
#include "pw_iot_sdk_config/config.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_sys_io/sys_io.h"
#include "pw_sys_io_mdh/backend.h"
#include "pw_sys_io_mdh/config.h"

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cinttypes>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <type_traits>

extern "C" {
#include "hal/serial_api.h"

void _pw_sys_io_lock_init(void);
void _pw_sys_io_lock_read(void);
void _pw_sys_io_unlock_read(void);
void _pw_sys_io_lock_write(void);
void _pw_sys_io_unlock_write(void);
}

namespace {

template <void (*lock)(), void (*unlock)()> struct LockGuard {
    LockGuard()
    {
        lock();
    }

    LockGuard(const LockGuard &) = delete;
    LockGuard &operator=(const LockGuard &) = delete;

    ~LockGuard()
    {
        unlock();
    }
};

using CriticalSection = LockGuard<core_util_critical_section_enter, core_util_critical_section_exit>;
using RxLockGuard = LockGuard<_pw_sys_io_lock_read, _pw_sys_io_unlock_read>;
using TxLockGuard = LockGuard<_pw_sys_io_lock_write, _pw_sys_io_unlock_write>;

// Ring buffer that overwrites old data when full.
template <typename T, size_t Size> struct RingBuffer {
    static_assert(Size > 0);

    RingBuffer() : head_(0), tail_(0), full_(false)
    {
    }

    void put(const T &data)
    {
        CriticalSection _;
        buffer_[head_] = data;
        increment(head_);

        if (full_) {
#if PW_SYS_IO_MDH_ABORT_ON_OVERFLOW
            abort();
#endif
            tail_ = head_;
        } else if (head_ == tail_) {
            full_ = true;
        }
    }

    void put(pw::span<const T> src)
    {
        if (src.size() == 0) {
            return;
        }

        if (src.size() == 1) {
            put(src[0]);
            return;
        }

        CriticalSection _;

        if (src.size() > Size) {
#if PW_SYS_IO_MDH_ABORT_ON_OVERFLOW
            abort();
#endif
            tail_ = 0;
            head_ = 0;
            full_ = true;
            std::copy(src.begin() + (src.size() - Size), src.end(), buffer_);
        } else {
            bool adjust_tail = ((Size - size_()) <= src.size());

            auto first_write_size = src.size();

            /* on first pass we write as much as we can to the right of head */
            if ((head_ + first_write_size) > Size) {
                first_write_size = Size - head_;
            }

            auto first_write_end = src.begin() + first_write_size;
            std::copy(src.begin(), first_write_end, buffer_ + head_);

            /* we might need to continue to write from the start of the buffer */
            if (first_write_end != src.end()) {
                std::copy(first_write_end, src.end(), buffer_);
                head_ = src.end() - first_write_end;
            } else {
                increment(head_, first_write_size);
            }

            if (adjust_tail) {
#if PW_SYS_IO_MDH_ABORT_ON_OVERFLOW
                abort();
#endif
                tail_ = head_;
                full_ = true;
            }
        }
    }

    bool get(T &data)
    {
        CriticalSection _;
        if (empty_()) {
            return false;
        }

        data = buffer_[tail_];
        increment(tail_);
        full_ = false;

        return true;
    }

    size_t get(pw::span<T> dest)
    {
        if (dest.size() == 0) {
            return 0;
        }

        if (dest.size() == 1) {
            return get(dest[0]) ? 1 : 0;
        }

        CriticalSection _;
        if (empty_()) {
            return 0;
        }

        // resize dest to fit with available data
        dest = dest.first(std::min(size_(), dest.size()));

        if (tail_ + dest.size() > Size) {
            auto it = std::copy(buffer_ + tail_, buffer_ + Size, dest.begin());
            // set to the future tail
            tail_ = dest.size() - (Size - tail_);
            std::copy(buffer_, buffer_ + tail_, it);
        } else {
            std::copy(buffer_ + tail_, buffer_ + dest.size(), dest.begin());
            increment(tail_, dest.size());
        }

        full_ = false;

        return dest.size();
    }

    bool empty() const
    {
        CriticalSection _;
        return empty_();
    }

    bool full() const
    {
        CriticalSection _;
        return bool(full_);
    }

    size_t size() const
    {
        CriticalSection _;
        return size_();
    }

    size_t capacity() const
    {
        return Size - size();
    }

private:
    T buffer_[Size];
    size_t head_ = 0;
    size_t tail_ = 0;
    bool full_ = false;

    void increment(size_t &val)
    {
        ++val;
        assert(val <= Size);

        if (val == Size) {
            val = 0;
        }
    }

    void increment(size_t &val, size_t incr)
    {
        val += incr;

        if (val >= Size) {
            val = val - Size;
        }
    }

    bool empty_() const
    {
        return head_ == tail_ && !bool(full_);
    }

    size_t size_() const
    {
        if (full_) {
            return Size;
        } else if (head_ < tail_) {
            return Size - (tail_ - head_);
        } else {
            return head_ - tail_;
        };
    }
};

// Link a reception and transmission buffers with a serial port.
struct BufferManager {
    // Explicitly defaulted to suppress cppcheck warnings about uninitialised members.
    BufferManager() = default;

    void setup(mdh_serial_t *serial)
    {
        serial_ = serial;
        mdh_serial_set_irq_callback(serial, serial_irq_cb, this);
        mdh_serial_set_irq_availability(serial, MDH_SERIAL_IRQ_TYPE_RX, true);
        mdh_serial_set_irq_availability(serial, MDH_SERIAL_IRQ_TYPE_TX, true);
    }

    bool put(std::byte data)
    {
        if (tx_buffer_.full()) {
            return false;
        }
        tx_buffer_.put(data);

        trigger_send();
        return true;
    }

    size_t put(const pw::ConstByteSpan &span)
    {
        auto capacity = tx_buffer_.capacity();
        auto data = span.first(std::min(capacity, span.size()));
        tx_buffer_.put(data);
        trigger_send();
        return data.size();
    }

    bool get(std::byte &dest)
    {
        return rx_buffer_.get(dest);
    }

    size_t get(pw::ByteSpan &span)
    {
        return rx_buffer_.get(span);
    }

private:
    mdh_serial_t *serial_ = nullptr;
    RingBuffer<std::byte, PW_SYS_IO_MDH_TX_BUFFER_SIZE> tx_buffer_;
    bool tx_sending_ = false;
    RingBuffer<std::byte, PW_SYS_IO_MDH_RX_BUFFER_SIZE> rx_buffer_;

    static void serial_irq_cb(void *p_instance, mdh_serial_irq_type_t event)
    {
        if ((event != MDH_SERIAL_IRQ_TYPE_RX && event != MDH_SERIAL_IRQ_TYPE_TX) || !p_instance) {
            return;
        }

        auto instance = reinterpret_cast<BufferManager *>(p_instance);

        if (event == MDH_SERIAL_IRQ_TYPE_RX) {
            instance->serial_irq_rx_cb();
        } else if (event == MDH_SERIAL_IRQ_TYPE_TX) {
            instance->serial_irq_tx_cb();
        }
    }

    void serial_irq_tx_cb()
    {
        if (tx_buffer_.empty()) {
            tx_sending_ = false;
        } else {
            std::byte c;

            do {
                auto available = tx_buffer_.get(c);
                if (available) {
                    tx_sending_ = true;
                    mdh_serial_put_data(serial_, (uint32_t)c);
                } else {
                    break;
                }
            } while (mdh_serial_is_writable(serial_));
        }
    }

    void serial_irq_rx_cb()
    {
        std::byte buf[PW_SYS_IO_MDH_PERIPHERAL_RX_BUFFER_SIZE];
        int i = 0;

        while (mdh_serial_is_readable(serial_)) {
            // Gather as much as possible data bytes
            buf[i] = std::byte(mdh_serial_get_data(serial_));
            ++i;
            if (i == PW_SYS_IO_MDH_PERIPHERAL_RX_BUFFER_SIZE) {
                rx_buffer_.put(pw::ConstByteSpan(buf));
                i = 0;
            }
        }

        rx_buffer_.put(pw::ConstByteSpan(buf, i));
    }

    void trigger_send()
    {
        std::byte c;
        CriticalSection _;
        if (tx_sending_) {
            return;
        }

        do {
            auto available = tx_buffer_.get(c);
            if (available) {
                tx_sending_ = true;
                mdh_serial_put_data(serial_, (uint32_t)c);
            } else {
                break;
            }
        } while (mdh_serial_is_writable(serial_));
    }
} buffer_manager;

} // namespace

extern "C" void pw_sys_io_init(mdh_serial_t *serial)
{
    buffer_manager.setup(serial);
    _pw_sys_io_lock_init();
}

namespace pw::sys_io {
Status ReadByte(std::byte *dest)
{
    RxLockGuard _;

    while (true) {
        Status status = TryReadByte(dest);

        // Return status when it's OK or an actual error.
        if (status.code() != PW_STATUS_UNAVAILABLE) {
            return status;
        }
    }
}

Status TryReadByte(std::byte *dest)
{
    RxLockGuard _;

    if (buffer_manager.get(*dest)) {
        return OkStatus();
    }

    return Status::Unavailable();
}

StatusWithSize ReadBytes(ByteSpan dest)
{
    RxLockGuard _;
    size_t count = buffer_manager.get(dest);
    return StatusWithSize(OkStatus(), count);
}

Status WriteByte(std::byte b)
{
    TxLockGuard _;
    // Note: the API expects to block if the serial is not yet available.
    while (!buffer_manager.put(b))
        ;
    return OkStatus();
}

StatusWithSize WriteBytes(ConstByteSpan data)
{
    TxLockGuard _;
    auto bytes = data;
    while (bytes.size()) {
        size_t count = buffer_manager.put(bytes);
        bytes = bytes.subspan(count);
    }

    return StatusWithSize(OkStatus(), data.size());
}

StatusWithSize WriteLine(const std::string_view &s)
{
    TxLockGuard _;
    WriteBytes(ConstByteSpan(reinterpret_cast<const std::byte *>(s.data()), s.size()));
    WriteByte(std::byte('\n'));

    return StatusWithSize(OkStatus(), s.size() + 1);
}
} // namespace pw::sys_io
