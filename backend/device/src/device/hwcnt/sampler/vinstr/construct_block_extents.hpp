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

#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/sampler/vinstr/construct_block_extents.hpp>
#include <device/hwcnt/sampler/vinstr/sample_layout.hpp>
#include <device/product_id.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace vinstr {

/**
 * Construct block extents.
 *
 * @param[in] pid                   Product ID.
 * @param[in] num_l2_slices         Number of L2 slices, which will be parsed to determine the number of memory blocks.
 * @param[in] num_shader_cores      Number of shader cores.
 * @return  Instance of block extents.
 */
inline hwcnt::block_extents construct_block_extents(product_id pid, uint64_t num_l2_slices, uint64_t num_shader_cores) {
    const uint8_t num_memory_blocks =
        (hwcpipe::device::hwcnt::sampler::vinstr::is_v4_layout(pid)) ? 1 : static_cast<uint8_t>(num_l2_slices);

    hwcnt::block_extents block_extents = hwcnt::block_extents{
        {{
            1,                                      // num_fe_blocks
            1,                                      // num_tiler_blocks
            num_memory_blocks,                      // num_memory_blocks
            static_cast<uint8_t>(num_shader_cores), // num_core_blocks
        }},
        64,                                // counters_per_block
        hwcnt::sample_values_type::uint32, // values_type
    };
    return block_extents;
}

} // namespace vinstr
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
