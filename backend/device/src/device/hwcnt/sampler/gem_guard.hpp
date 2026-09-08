/*
 * Copyright (c) 2025 Arm Limited.
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

#include <device/ioctl/panthor/commands.hpp>
#include <device/ioctl/panthor/types.hpp>

#include <cstdint>
#include <system_error>
#include <tuple>
#include <utility>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {

/** Helper to close GEM object on scope exit. */
template <typename syscall_iface_t>
class gem_guard : private syscall_iface_t {
  public:
    /**
     * Default construct GEM object guard.
     *
     * @param[in] iface     Syscall interface (testing only).
     */
    explicit gem_guard(syscall_iface_t iface = {})
        : syscall_iface_t(iface) {}

    /**
     * Construct GEM object guard.
     *
     * @param[in] fd        File descriptor this GEM object is associated with.
     * @param[in] handle    Descriptor to manage.
     * @param[in] iface     Syscall interface (testing only).
     */
    gem_guard(int fd, uint32_t handle, syscall_iface_t iface = {})
        : syscall_iface_t(iface)
        , fd_(fd)
        , handle_(handle) {}

    ~gem_guard() { reset(0); }

    gem_guard(const gem_guard &) = delete;
    gem_guard &operator=(const gem_guard &) = delete;

    gem_guard(gem_guard &&other)
        : syscall_iface_t(other)
        , fd_(other.fd_)
        , handle_(other.release()) {}

    gem_guard &operator=(gem_guard &&other) {
        static_cast<syscall_iface_t &>(*this) = other;

        std::swap(handle_, other.handle_);

        return *this;
    }

    /** @return managed GEM object. */
    uint32_t get() const { return handle_; }

    /** Reset GEM handle to manage a new value.
     *
     * @param[in] handle New GEM handle to manage.
     */
    void reset(uint32_t handle) {
        if (handle_ > 0)
            gem_close();
        handle_ = handle;
    }

    /** @return managed file descriptor, and release ownership. */
    uint32_t release() { return std::exchange(handle_, 0); }

  private:
    void gem_close() {
        ioctl::panthor::drm_gem_close close{};
        close.handle = handle_;
        this->ioctl(fd_, ioctl::panthor::command::drm_gem_close, &close);
    }

    int fd_{-1};
    uint32_t handle_{};
};

} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
