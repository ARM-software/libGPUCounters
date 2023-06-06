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

/** @file backend_args.hpp base::backend constructor arguments. */

#pragma once

#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/features.hpp>
#include <device/hwcnt/sampler/filefd_guard.hpp>
#include <device/hwcnt/sampler/mapped_memory.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace base {

/** Base sampler constructor arguments. */
template <typename syscall_iface_t>
struct backend_args {
    /** Counters buffer memory type. */
    using memory_type = mapped_memory<syscall_iface_t>;
    /** Filefd guard type. */
    using filefd_guard_type = filefd_guard<syscall_iface_t>;
    /** Hardware counters fd. */
    filefd_guard_type fd{};
    /** Sampling period (nanoseconds). */
    uint64_t period_ns{};
    /** Hardware counters features. */
    features features_v{};
    /** Block extents. */
    block_extents extents{};
    /** Counters buffer memory. */
    memory_type memory{};
};

} // namespace base
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
