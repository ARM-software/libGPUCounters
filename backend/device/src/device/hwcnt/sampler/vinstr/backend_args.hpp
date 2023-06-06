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

#pragma once

#include "sample_layout.hpp"

#include <device/hwcnt/sampler/base/backend_args.hpp>
#include <device/hwcnt/sampler/vinstr/sample_layout.hpp>
#include <device/ioctl/kbase/types.hpp>
#include <device/ioctl/vinstr/types.hpp>

#include <bitset>
#include <cstdint>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace vinstr {

/** Arguments for `vinstr::backend` constructor. */
template <typename syscall_iface_t>
struct backend_args {
    /** Base args type. */
    using base_args_type = base::backend_args<syscall_iface_t>;
    /** Counters buffer memory type. */
    using memory_type = typename base_args_type::memory_type;

    /** Arguments for `base::backend`. */
    base_args_type base_args;
    /** Vinstr reader features. */
    ioctl::vinstr::reader_features features{};
    /** Hardware counters buffer size. */
    size_t buffer_size{0};
    /** Sample layout to use. */
    sample_layout sample_layout_v;
    /** Default number of buffers in the kernel ring buffer.
     *
     * Kbase will use `__get_free_pages` to allocate these buffers as a
     * physically contiguous memory chunk. When fragmentation is high,
     * it may fail to do so. Then we will try to allocate fewer
     * power-of-two buffer numbers -- 16, 8, 4 and 2.
     */
    static const constexpr size_t max_buffer_count{32};
};

template <typename syscall_iface_t>
constexpr size_t backend_args<syscall_iface_t>::max_buffer_count;

} // namespace vinstr
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
