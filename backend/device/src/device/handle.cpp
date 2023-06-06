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

#include "handle_impl.hpp"
#include "syscall/iface.hpp"

#include <device/handle.hpp>

#include <cassert>
#include <string>

namespace hwcpipe {
namespace device {

using handle_impl_type = handle_impl<syscall::iface>;

handle::~handle() = default;

handle::handle_ptr handle::create(uint32_t instance_number) {
    std::string device_path("/dev/mali" + std::to_string(instance_number));
    int fd = handle_impl_type::open(device_path.c_str());

    if (fd < 0)
        return nullptr;

    return handle::handle_ptr(new handle_impl_type(fd, handle_impl_type::mode::internal));
}

handle::handle_ptr handle::create(const char *device_path) {
    int fd = handle_impl_type::open(device_path);

    if (fd < 0)
        return nullptr;

    return handle::handle_ptr(new handle_impl_type(fd, handle_impl_type::mode::internal));
}

handle::handle_ptr handle::from_external_fd(int fd) {
    assert(fd >= 0);
    return handle::handle_ptr(new handle_impl_type(fd, handle_impl_type::mode::external));
}

} // namespace device
} // namespace hwcpipe
