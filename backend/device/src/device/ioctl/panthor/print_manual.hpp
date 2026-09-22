/*
 * Copyright (c) 2024-2025 Arm Limited.
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

#include "device/ioctl/panthor/types.hpp"

#include <device/logging/ostream_indent.hpp>
#include <device/logging/print_array.hpp>

#include <ostream>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace panthor {

inline std::ostream &operator<<(std::ostream &os, const dev_query_ptr &dqp) { return os << dqp.u_csif_info; }

inline std::ostream &operator<<(std::ostream &os, const dev_query &value) {
    return os << "dev_query {\n"                                                   //
              << hwcpipe::debug::indent_level::push                                //
              << hwcpipe::debug::indent << ".type = " << value.type << ",\n"       //
              << hwcpipe::debug::indent << ".size = " << value.size << ",\n"       //
              << hwcpipe::debug::indent << ".pointer = " << value.pointer << ",\n" //
              << hwcpipe::debug::indent_level::pop                                 //
              << hwcpipe::debug::indent << "}";                                    //
}

} // namespace panthor
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
