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

/**
 * @file convert.hpp
 *
 * Conversion routines for vinstr back-end.
 */

#pragma once

#include <device/hwcnt/sampler/configuration.hpp>
#include <device/ioctl/kbase/types.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace vinstr {

/**
 * Convert from hwcpipe enable mask to vinstr enable mask.
 *
 * @param[in] mask Hwcpipe mask to convert.
 * @return vinstr counters mask.
 */
inline uint32_t convert(configuration::enable_map_type mask) {
    constexpr size_t vinstr_mask_bitcount = 32;
    constexpr size_t counters_per_bit{4};
    constexpr configuration::enable_map_type counters_bit_mask{0b1111};

    static_assert(vinstr_mask_bitcount * counters_per_bit == mask.size(), "Unexpected bit count.");

    std::bitset<vinstr_mask_bitcount> result{};

    for (size_t idx = 0; mask.any(); mask >>= counters_per_bit, idx++)
        result[idx] = (mask & counters_bit_mask).any();

    return static_cast<uint32_t>(result.to_ulong());
}

/**
 * Convert hwcpipe configuration arguments to vinstr setup arguments.
 *
 * @param[in] begin    Begin iterator.
 * @param[in] end End iterator.
 * @return A pair of error code and `hwcnt_reader_setup` structure.
 */
inline auto convert(const configuration *begin, const configuration *end) {
    ioctl::kbase::hwcnt_reader_setup result{};

    for (auto it = begin; it != end; ++it) {
        if (it->set != prfcnt_set::primary)
            return std::make_pair(std::make_error_code(std::errc::not_supported), result);

        switch (it->type) {
        case block_type::fe:
            result.fe_bm |= convert(it->enable_map);
            break;
        case block_type::tiler:
            result.tiler_bm |= convert(it->enable_map);
            break;
        case block_type::memory:
            result.mmu_l2_bm |= convert(it->enable_map);
            break;
        case block_type::core:
            result.shader_bm |= convert(it->enable_map);
            break;
        case block_type::firmware:
        case block_type::csg:
            return std::make_pair(std::make_error_code(std::errc::invalid_argument), result);
        }
    }

    return std::make_pair(std::error_code{}, result);
}

} // namespace vinstr
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
