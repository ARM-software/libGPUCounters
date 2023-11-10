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

/**
 * @file
 *
 * Mali Device Driver handle implementation.
 */

#pragma once

#include <device/handle.hpp>

#include <system_error>

#include <fcntl.h>

namespace hwcpipe {
namespace device {

/** Handle interface implementation. */
template <typename syscall_iface_t>
class handle_impl : public handle, private syscall_iface_t {
  public:
    /** Determines how descriptor should be treated. */
    enum class mode {
        /** The descriptor is closed at destruction time. */
        internal,
        /** the descriptor is kept open at destruction time. */
        external,
    };

    /**
     * Handle constructor.
     *
     * @param[in]     fd   Device file descriptor.
     * @param[in]     m    Which descriptor handling mode to use.
     * @param[in,out] args System calls interface constructor args (unit tests only).
     */
    template <typename... args_t>
    handle_impl(int fd, mode m, args_t &&...args)
        : syscall_iface_t(std::forward<args_t>(args)...)
        , fd_(fd)
        , mode_(m) {}

    ~handle_impl() override {
        if (mode_ == mode::internal)
            get_syscall_iface().close(fd_);
    }

    /**
     * Open character device.
     *
     * @param[in]     path  The device path.
     * @param[in,out] iface System calls interface to use (unit tests only).
     * @return Device handle on success, -1 on failure.
     */
    template <typename other_syscall_iface_t = syscall_iface_t>
    static int open(const char *path, other_syscall_iface_t &&iface = {}) {
        int fd = -1;
        std::error_code ec;

        std::tie(ec, fd) = iface.open(path, O_RDONLY);

        if (ec)
            return -1;

        bool is_dev = false;
        std::tie(ec, is_dev) = iface.is_char_device(fd);

        if (ec || !is_dev) {
            iface.close(fd);
            return -1;
        }

        return fd;
    }

    /**
     * Get the file descriptor.
     *
     * @return file descriptor.
     */
    int fd() const { return fd_; }

  private:
    /** Syscall interface type. */
    using syscall_iface_type = syscall_iface_t;
    /** @return syscall interface instance. */
    syscall_iface_type &get_syscall_iface() { return *this; }

    int fd_;
    mode mode_;
};
} // namespace device
} // namespace hwcpipe
