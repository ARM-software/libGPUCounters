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
 * @file unix.hpp Unix system call functions.
 */

#pragma once

#include <utility>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#if defined(unix)
#undef unix
#endif

namespace hwcpipe {
namespace device {
namespace syscall {
namespace funcs {

/** Unix syscall functions class. */
struct unix {
    template <typename... args_t>
    static auto open(args_t &&...args) {
        return ::open(std::forward<args_t>(args)...);
    }
    template <typename... args_t>
    static auto close(args_t &&...args) {
        return ::close(std::forward<args_t>(args)...);
    }
    template <typename... args_t>
    static auto ioctl(args_t &&...args) {
        return ::ioctl(std::forward<args_t>(args)...);
    }

    template <typename... args_t>
    static auto mmap(args_t &&...args) {
        return ::mmap(std::forward<args_t>(args)...);
    }

    template <typename... args_t>
    static auto munmap(args_t &&...args) {
        return ::munmap(std::forward<args_t>(args)...);
    }
};

} // namespace funcs
} // namespace syscall
} // namespace device
} // namespace hwcpipe
