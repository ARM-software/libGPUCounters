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
namespace kinstr_prfcnt {

/** Interface kinstr_prfcnt number. */
constexpr auto iface_number = 0xbf;

namespace command {

/** Commands describing kinstr_prfcnt ioctl interface. */
enum command_type {
    /** Issue command. */
    issue_command = _IOW(iface_number, 0x0, ::hwcpipe::device::ioctl::kinstr_prfcnt::control_cmd),
    /** Get sample. */
    get_sample = _IOR(iface_number, 0x1, ::hwcpipe::device::ioctl::kinstr_prfcnt::sample_access),
    /** Put sample. */
    put_sample = _IOW(iface_number, 0x10, ::hwcpipe::device::ioctl::kinstr_prfcnt::sample_access),
};

} // namespace command
} // namespace kinstr_prfcnt
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
