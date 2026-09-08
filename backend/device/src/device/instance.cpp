/*
 * Copyright (c) 2022-2025 Arm Limited.
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
 * Mali Device Driver instance implementation.
 */

#include "handle_impl.hpp"
#include "instance_impl.hpp"
#include "instance_impl_panthor.hpp"
#include "syscall/iface.hpp"

#include <device/detail/cast_to_impl.hpp>
#include <device/error.hpp>

#include <memory>

#include <sys/stat.h>

namespace hwcpipe {
namespace device {

using handle_impl_type = handle_impl<syscall::iface>;
using instance_impl_kbase_type = instance_impl<syscall::iface>;
using instance_impl_panthor_type = instance_impl_panthor<syscall::iface>;

instance::~instance() = default;

instance::instance_ptr instance::create(handle &hndl, std::error_code &ec) {
    const auto &hndl_impl = detail::cast_to_impl(hndl);

    switch (hndl_impl.type()) {
    case handle_impl_type::type::kbase: {
        auto result = std::make_unique<instance_impl_kbase_type>(hndl_impl.fd(), ec);
        if (!result || !result->valid())
            return {};

        return result;
    }
    case handle_impl_type::type::panthor: {
        auto result = std::make_unique<instance_impl_panthor_type>(hndl_impl.fd(), ec);
        if (!result || !result->valid())
            return {};

        return result;
    }
    }
    __builtin_unreachable();
    return {};
}

instance::instance_ptr instance::create(handle &hndl) {
    std::error_code ec{};
    return create(hndl, ec);
}

} // namespace device
} // namespace hwcpipe
