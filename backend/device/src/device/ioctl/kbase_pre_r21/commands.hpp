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

/* Note, this file is generated, do NOT edit! */

#pragma once

#include "types.hpp"

#include <linux/ioctl.h>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace kbase_pre_r21 {

/** Interface kbase_pre_r21 number. */
constexpr auto iface_number = 0x80;

namespace command {

/** Commands describing kbase_pre_r21 ioctl interface. */
enum command_type {
    /** Check version compatibility between JM kernel and userspace. */
    version_check = _IOWR(iface_number, 0x0, ::hwcpipe::device::ioctl::kbase_pre_r21::version_check_args),
    /** Set kernel context creation flags. */
    set_flags = _IOWR(iface_number, 0x212, ::hwcpipe::device::ioctl::kbase_pre_r21::set_flags_args),
    /** Get GPU properties. */
    get_gpuprops = _IOWR(iface_number, 0x20e, ::hwcpipe::device::ioctl::kbase_pre_r21::uk_gpuprops),
    /** Hardware counter reader setup. */
    hwcnt_reader_setup = _IOWR(iface_number, 0x224, ::hwcpipe::device::ioctl::kbase_pre_r21::uk_hwcnt_reader_setup),
};

} // namespace command
} // namespace kbase_pre_r21
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
