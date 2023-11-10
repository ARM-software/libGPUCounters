/*
 * Copyright (c) 2022-2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/** @file mapped_memory.hpp */

#pragma once

#include <cstddef>
#include <ostream>
#include <system_error>
#include <tuple>
#include <utility>

#include <sys/mman.h>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {

/**
 * Hardware counters sampler memory mapping.
 *
 * RAII class to map `size` bytes of read only memory in constructor,
 * and unmap it in destructor.
 *
 * @par Example
 * @code
 * std::error_code ec;
 * // Map 1024 bytes using `fd` file descriptor.
 * mapped_memory<syscall::iface> m{fd, 1024, ec};
 * // Access the memory.
 * const void *data = m.data();
 * @endcode
 */
template <typename syscall_iface_t>
class mapped_memory : private syscall_iface_t {
  public:
    /** Default constructor. */
    mapped_memory() = default;
    /** No copy. */
    mapped_memory(const mapped_memory &) = delete;
    /** No assign. */
    mapped_memory &operator=(const mapped_memory &) = delete;

    /** Move constructor. */
    mapped_memory(mapped_memory &&other)
        : syscall_iface_t(other)
        , data_(std::exchange(other.data_, nullptr))
        , size_(std::exchange(other.size_, 0)) {}

    /** Move assignment. */
    mapped_memory &operator=(mapped_memory &&other) {
        static_cast<syscall_iface_t &>(*this) = other;

        std::swap(data_, other.data_);
        std::swap(size_, other.size_);

        return *this;
    }

    /**
     * Memory mapping constructor.
     *
     * @param[in] fd     File descriptor to map.
     * @param[in] size   Mapping size.
     * @param[out] ec    Error code.
     * @param[in] iface  Syscall iface (testing only).
     */
    explicit mapped_memory(int fd, size_t size, std::error_code &ec, const syscall_iface_t &iface)
        : syscall_iface_t(iface)
        , size_(size) {
        std::tie(ec, data_) = get_syscall_iface().mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd, 0);

        if (ec)
            data_ = nullptr;
    }

    explicit mapped_memory(void *data, size_t size, const syscall_iface_t &iface)
        : syscall_iface_t(iface)
        , data_(data)
        , size_(size) {}

    /** @return true if memory is mapped, false otherwise. */
    operator bool() const { return data_ != nullptr; }

    /** @return base address of mapped memory. */
    const void *data() const { return data_; }
    /** @return size of the memory mapping. */
    size_t size() const { return size_; }

    /** Destructor. */
    ~mapped_memory() {
        if (data_ == nullptr)
            return;

        // Note, that data_ has to be non const.
        get_syscall_iface().munmap(data_, size_);
    }

  private:
    /** @return Syscall iface reference. */
    syscall_iface_t &get_syscall_iface() { return *this; }

    /** Memory mapping address. */
    void *data_{};
    /** Memory mapping size. */
    size_t size_{};

    template <typename other_syscall_iface_t>
    friend std::ostream &operator<<(std::ostream &os, const mapped_memory<other_syscall_iface_t> &value);
};

template <typename other_syscall_iface_t>
inline std::ostream &operator<<(std::ostream &os, const mapped_memory<other_syscall_iface_t> &value) {
    return os << "mapping { .data = " << value.data_ << ", .size = " << value.size_ << " }";
}

} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
