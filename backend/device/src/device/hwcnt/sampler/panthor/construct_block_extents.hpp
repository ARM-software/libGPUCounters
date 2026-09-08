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

#pragma once

#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/clock_extents.hpp>
#include <device/hwcnt/sampler/panthor/backend.hpp>
#include <device/ioctl/panthor/commands.hpp>
#include <device/ioctl/panthor/types.hpp>
#include <device/product_id.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace panthor {

/**
 * Construct block extents.
 *
 * @param[in] perf_info       Device performance counter info
 * @return  Instance of block extents.
 */
inline auto construct_block_extents(ioctl::panthor::perf_info &info) {
    hwcnt::block_extents result;

    result = hwcnt::block_extents{
        {{
            static_cast<uint8_t>(info.cshw_blocks),     // num_fe_blocks
            static_cast<uint8_t>(info.geometry_blocks), // num_geometry_blocks
            static_cast<uint8_t>(info.memsys_blocks),   // num_memory_blocks
            static_cast<uint8_t>(info.shader_blocks),   // num_core_blocks
            static_cast<uint8_t>(info.fw_blocks),       // num_fw_blocks
            0,                                          // num_csg_blocks
            static_cast<uint8_t>(info.neural_blocks)    // num_ne_blocks
        }},
        static_cast<uint16_t>(info.counters_per_block), // counters_per_block
        hwcnt::sample_values_type::uint64,              // values_type
    };
    return result;
}

static uint32_t bit(ioctl::panthor::perf_clock clk) {
    return 1 << static_cast<std::underlying_type<decltype(clk)>::type>(clk);
}

/**
 * Construct clock extents.
 *
 * @param[in] perf_info       Device performance counter info
 * @return  Instance of clock extents.
 */
inline auto construct_clock_extents(ioctl::panthor::perf_info &info) {
    hwcnt::clock_extents clock_extents =
        hwcnt::clock_extents{(info.supported_clocks & bit(ioctl::panthor::perf_clock::toplevel)) > 0,
                             (info.supported_clocks & bit(ioctl::panthor::perf_clock::coregroup)) > 0,
                             (info.supported_clocks & bit(ioctl::panthor::perf_clock::shader)) > 0,
                             (info.supported_clocks & bit(ioctl::panthor::perf_clock::neural)) > 0};

    return clock_extents;
}

} // namespace panthor
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
