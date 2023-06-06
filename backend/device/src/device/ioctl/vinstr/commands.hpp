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
namespace vinstr {

/** Interface vinstr number. */
constexpr auto iface_number = 0xbe;

namespace command {

/** Commands describing vinstr ioctl interface. */
enum command_type {
    /** Get HW version. */
    get_hwver = _IOR(iface_number, 0x0, uint32_t),
    /** Get HWCNT dump buffer size. */
    get_buffer_size = _IOR(iface_number, 0x1, uint32_t),
    /** Request manual HWCNT dump. */
    dump = _IOW(iface_number, 0x10, uint32_t),
    /** Request HWCNT clear. */
    clear = _IOW(iface_number, 0x11, uint32_t),
    /** Get hardware counters buffer. */
    get_buffer = _IOR(iface_number, 0x20, ::hwcpipe::device::ioctl::vinstr::reader_metadata),
    /** Get hardware counters buffer with cycle counters. */
    get_buffer_with_cycles = _IOR(iface_number, 0x20, ::hwcpipe::device::ioctl::vinstr::reader_metadata_with_cycles),
    /** Put hardware counters buffer. */
    put_buffer = _IOW(iface_number, 0x21, ::hwcpipe::device::ioctl::vinstr::reader_metadata),
    /** Put hardware counters buffer with cycles. */
    put_buffer_with_cycles = _IOW(iface_number, 0x21, ::hwcpipe::device::ioctl::vinstr::reader_metadata_with_cycles),
    /** Set HWCNT sampling interval. Zero for manual sampling. */
    set_interval = _IOW(iface_number, 0x30, uint32_t),
    /** Enable HWCNT event. */
    enable_event = _IOW(iface_number, 0x40, ::hwcpipe::device::ioctl::vinstr::reader_event),
    /** Disable HWCNT event. */
    disable_event = _IOW(iface_number, 0x41, ::hwcpipe::device::ioctl::vinstr::reader_event),
    /** Get HWCNT api version. */
    get_api_version = _IOW(iface_number, 0xff, uint32_t),
    /** Get HWCNT api version with features mask. */
    get_api_version_with_features = _IOW(iface_number, 0xff, ::hwcpipe::device::ioctl::vinstr::reader_api_version),
};

} // namespace command
} // namespace vinstr
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
