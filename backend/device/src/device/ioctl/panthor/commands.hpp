/*
 * Copyright (c) 2024-2026 Arm Limited.
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
namespace panthor {

/** Interface panthor number. */
constexpr auto iface_number = 0x64;

namespace command {

/** Commands describing panthor ioctl interface. */
enum command_type {
    /** Close GEM handle. */
    drm_gem_close = _IOW(iface_number, 0x9, ::hwcpipe::device::ioctl::panthor::drm_gem_close),
    /** Query device properties. */
    dev_query = _IOWR(iface_number, 0x40, ::hwcpipe::device::ioctl::panthor::dev_query),
    /** Create a VM. */
    vm_create = _IOWR(iface_number, 0x41, ::hwcpipe::device::ioctl::panthor::vm_create),
    /** Destroy a VM. */
    vm_destroy = _IOWR(iface_number, 0x42, ::hwcpipe::device::ioctl::panthor::vm_destroy),
    /** Create a buffer object. */
    bo_create = _IOWR(iface_number, 0x45, ::hwcpipe::device::ioctl::panthor::bo_create),
    /** Get file offset to map GEM object */
    bo_mmap_offset = _IOWR(iface_number, 0x46, ::hwcpipe::device::ioctl::panthor::bo_mmap_offset),
    /** Setup a performance counter sampling session. */
    perf_control = _IOWR(iface_number, 0x9e, ::hwcpipe::device::ioctl::panthor::perf_control),
};

} // namespace command
} // namespace panthor
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
