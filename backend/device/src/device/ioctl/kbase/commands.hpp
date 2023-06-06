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
namespace kbase {

/** Interface kbase number. */
constexpr auto iface_number = 0x80;

namespace command {

/** Commands describing kbase ioctl interface. */
enum command_type {
    /** Check version compatibility between JM kernel and userspace. */
    version_check_jm = _IOWR(iface_number, 0x0, ::hwcpipe::device::ioctl::kbase::version_check),
    /** Check version compatibility between CSF kernel and userspace. */
    version_check_csf = _IOWR(iface_number, 0x34, ::hwcpipe::device::ioctl::kbase::version_check),
    /** Set kernel context creation flags. */
    set_flags = _IOW(iface_number, 0x1, ::hwcpipe::device::ioctl::kbase::set_flags),
    /** Get GPU properties. */
    get_gpuprops = _IOW(iface_number, 0x3, ::hwcpipe::device::ioctl::kbase::get_gpuprops),
    /** Request the global control block of CSF interface capabilities. */
    cs_get_glb_iface = _IOWR(iface_number, 0x33, ::hwcpipe::device::ioctl::kbase::cs_get_glb_iface),
    /** Setup HWC dumper/reader. */
    hwcnt_reader_setup = _IOW(iface_number, 0x8, ::hwcpipe::device::ioctl::kbase::hwcnt_reader_setup),
    /** Enum Performance counter information. */
    kinstr_prfcnt_enum_info = _IOWR(iface_number, 0x38, ::hwcpipe::device::ioctl::kbase::kinstr_prfcnt_enum_info),
    /** Setup HWC dumper/reader. */
    kinstr_prfcnt_setup = _IOWR(iface_number, 0x39, ::hwcpipe::device::ioctl::kbase::kinstr_prfcnt_setup),
};

} // namespace command
} // namespace kbase
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
