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

#pragma once

#include <utility>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {

/** Helper to close descriptor on scope exit. */
template <typename syscall_iface_t>
class filefd_guard : private syscall_iface_t {
  public:
    /**
     * Default construct descriptor guard.
     *
     * @param[in] iface     Syscall interface (testing only).
     */
    explicit filefd_guard(syscall_iface_t iface = {})
        : syscall_iface_t(iface)
        , fd_(-1) {}

    /**
     * Construct descriptor guard.
     *
     * @param[in] fd        Descriptor to manage.
     * @param[in] iface     Syscall interface (testing only).
     */
    filefd_guard(int fd, syscall_iface_t iface = {})
        : syscall_iface_t(iface)
        , fd_(fd) {}

    ~filefd_guard() { reset(-1); }

    filefd_guard(const filefd_guard &) = delete;
    filefd_guard &operator=(const filefd_guard &) = delete;

    filefd_guard(filefd_guard &&other)
        : syscall_iface_t(other)
        , fd_(other.release()) {}

    filefd_guard &operator=(filefd_guard &&other) {
        static_cast<syscall_iface_t &>(*this) = other;

        std::swap(fd_, other.fd_);

        return *this;
    }

    /** @return managed file descriptor. */
    int get() const { return fd_; }

    /** Reset file handle to manage a new value.
     *
     * @param[in] fd    New file handle to manage.
     */
    void reset(int fd) {
        if (fd_ >= 0)
            this->close(fd_);
        fd_ = fd;
    }

    /** @return managed file descriptor, and release ownership. */
    int release() { return std::exchange(fd_, -1); }

  private:
    int fd_;
};

} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
