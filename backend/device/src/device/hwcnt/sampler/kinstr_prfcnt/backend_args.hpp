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

/** @file backend_args.hpp kinstr_prfcnt::backend constructor arguments. */

#pragma once

#include <device/hwcnt/sampler/base/backend_args.hpp>
#include <device/shader_core_bitset.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {

/** Arguments for `kinstr_prfcnt::backend` constructor. */
template <typename syscall_iface_t>
struct backend_args {
    /** Base args type. */
    using base_args_type = base::backend_args<syscall_iface_t>;
    /** Counters buffer memory type. */
    using memory_type = typename base_args_type::memory_type;
    /** Max number of shader cores blocks. */
    static constexpr size_t max_blocks_sc = 64;

    /** Arguments for `base::backend`. */
    base_args_type base_args;

    /** Shader cores mask. */
    shader_core_bitset sc_mask;
    /** Metadata item size. */
    size_t metadata_item_size{};
};

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
