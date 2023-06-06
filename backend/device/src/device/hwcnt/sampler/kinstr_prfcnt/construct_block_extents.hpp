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
#include <device/hwcnt/sampler/kinstr_prfcnt/enum_info.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {

/**
 *  Parse enum info to construct an instance of block extents.
 *
 * @param[in] ei Enum Info.
 * @return Block extents instance parsed from enum info.
 */
inline hwcnt::block_extents construct_block_extents(enum_info ei) {
    hwcnt::block_extents block_extents = hwcnt::block_extents{
        {{
            ei.blocks[static_cast<uint8_t>(hwcnt::block_type::fe)].num_instances,     // num_fe_blocks
            ei.blocks[static_cast<uint8_t>(hwcnt::block_type::tiler)].num_instances,  // num_tiler_blocks
            ei.blocks[static_cast<uint8_t>(hwcnt::block_type::memory)].num_instances, // num_memory_blocks
            ei.blocks[static_cast<uint8_t>(hwcnt::block_type::core)].num_instances,   // num_core_blocks
        }},
        ei.num_values,                     // counters_per_block
        hwcnt::sample_values_type::uint64, // values_type
    };
    return block_extents;
}

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
