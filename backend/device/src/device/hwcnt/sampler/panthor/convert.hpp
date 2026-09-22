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

/**
 * @file convert.hpp
 *
 * Conversion routines for panthor back-end.
 */

#pragma once

#include "device/hwcnt/prfcnt_set.hpp"
#include "device/ioctl/panthor/types.hpp"

#include <device/hwcnt/sample.hpp>
#include <device/hwcnt/sampler/configuration.hpp>
#include <device/ioctl/kbase/types.hpp>

#include <limits>
#include <system_error>
#include <utility>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace panthor {

/**
 * Convert from hwcpipe enable mask to panthor enable mask.
 *
 * @param[in] mask Hwcpipe mask to convert.
 * @return panthor counters mask.
 */
inline std::array<uint64_t, 2> convert(configuration::enable_map_type mask) {
    configuration::enable_map_type mask_low64{std::numeric_limits<uint64_t>::max()};
    constexpr uint64_t uint64_t_bits = 64;
    return {
        (mask & mask_low64).to_ullong(),
        ((mask >> uint64_t_bits) & mask_low64).to_ullong(),
    };
}

/**
 * Convert from panthor set to hwcpipe set
 *
 * @param[in] set Panthor set to convert.
 * @return Hwcpipe counter set
 */
inline auto convert(ioctl::panthor::perf_counter_set set) {
    switch (set) {
    case ioctl::panthor::perf_counter_set::primary:
        return prfcnt_set::primary;
    case ioctl::panthor::perf_counter_set::secondary:
        return prfcnt_set::secondary;
    case ioctl::panthor::perf_counter_set::tertiary:
        return prfcnt_set::tertiary;
    }
    __builtin_unreachable();
}

/**
 * Convert from hwcpipe set to panthor set
 *
 * @param[in] set Hwcpipe set to convert.
 * @return panthor counter set
 */
inline auto convert(prfcnt_set set) {
    switch (set) {
    case prfcnt_set::primary:
        return ioctl::panthor::perf_counter_set::primary;
    case prfcnt_set::secondary:
        return ioctl::panthor::perf_counter_set::secondary;
    case prfcnt_set::tertiary:
        return ioctl::panthor::perf_counter_set::tertiary;
    default:
        __builtin_unreachable();
    }
}

inline auto convert(uint8_t set) {
    switch (set) {
    case 0U:
        return std::make_pair(std::error_code{}, prfcnt_set::primary);
    case 1U:
        return std::make_pair(std::error_code{}, prfcnt_set::secondary);
    case 2U:
        return std::make_pair(std::error_code{}, prfcnt_set::tertiary);
    default:
        return std::make_pair(std::make_error_code(std::errc::invalid_argument), prfcnt_set{});
    }
}

/**
 * Convert from panthor block state to hwcpipe block state.
 *
 * @param[in] value Value to convert.
 * @return Converted value.
 */
inline block_state convert(ioctl::panthor::block_header::block_state value) {
    using block_state_type = ioctl::panthor::block_header::block_state;
    block_state result{};

    result.on = !!(value & block_state_type::on);
    result.off = !!(value & block_state_type::off);
    result.available = !!(value & block_state_type::available);
    result.unavailable = !!(value & block_state_type::unavailable);
    result.normal = !!(value & block_state_type::normal_mode);
    result.protected_mode = !!(value & block_state_type::protected_mode);

    return result;
}

/**
 * Convert from panthor sample flags to hwcpipe sample flags.
 *
 * @param[in] flags Panthor flags to convert.
 * @return Converted flags.
 */
inline auto convert(ioctl::panthor::sample_header::sample_flags flags) {
    using sample_flag_type = ioctl::panthor::sample_header::sample_flags;
    device::hwcnt::sample_flags result{};

    result.error = !!(flags & sample_flag_type::sample_overflow);
    result.stretched = !!(flags & sample_flag_type::sample_stretched);

    return result;
}

/**
 * Convert hwcpipe configuration arguments to panthor config arguments.
 *
 * @param[in] begin Begin iterator.
 * @param[in] end   End iterator.
 * @return A pair of error code and `perf_cmd_config` structure.
 */
inline auto convert(const configuration *begin, const configuration *end) {
    std::error_code ec;
    ioctl::panthor::perf_cmd_setup result{};

    // For now, using the block set from the first block
    result.block_set = static_cast<std::underlying_type<decltype(begin->set)>::type>(convert(begin->set));

    for (const auto *it = begin; it != end; ++it) {
        ioctl::panthor::perf_counter_set curr_block_set = convert(it->set);

        if (ec)
            return std::make_pair(ec, result);

        if (curr_block_set != convert(begin->set))
            return std::make_pair(std::make_error_code(std::errc::not_supported), result);

        auto enable_mask = convert(it->enable_map);
        switch (it->type) {
        case block_type::fe:
            result.cshw_enable_mask[0] = enable_mask[0];
            result.cshw_enable_mask[1] = enable_mask[1];
            break;
        case block_type::tiler:
            result.geometry_enable_mask[0] = enable_mask[0];
            result.geometry_enable_mask[1] = enable_mask[1];
            break;
        case block_type::memory:
            result.memsys_enable_mask[0] = enable_mask[0];
            result.memsys_enable_mask[1] = enable_mask[1];
            break;
        case block_type::core:
            result.shader_enable_mask[0] = enable_mask[0];
            result.shader_enable_mask[1] = enable_mask[1];
            break;
        case block_type::firmware:
            result.fw_enable_mask[0] = enable_mask[0];
            result.fw_enable_mask[1] = enable_mask[1];
            break;
        case block_type::neural_accelerator:
            result.neural_enable_mask[0] = enable_mask[0];
            result.neural_enable_mask[1] = enable_mask[1];
            break;
        case block_type::csg:
            break;
        default:
            return std::make_pair(std::make_error_code(std::errc::invalid_argument), result);
        }
    }

    return std::make_pair(std::error_code{}, result);
}

inline std::pair<std::error_code, block_type> convert(ioctl::panthor::perf_block_type value) {
    switch (value) {
    case ioctl::panthor::perf_block_type::fw_block:
        return std::make_pair(std::error_code{}, block_type::firmware);
    case ioctl::panthor::perf_block_type::cshw_block:
        return std::make_pair(std::error_code{}, block_type::fe);
    case ioctl::panthor::perf_block_type::geometry_block:
        return std::make_pair(std::error_code{}, block_type::tiler);
    case ioctl::panthor::perf_block_type::memsys_block:
        return std::make_pair(std::error_code{}, block_type::memory);
    case ioctl::panthor::perf_block_type::shader_block:
        return std::make_pair(std::error_code{}, block_type::core);
    case ioctl::panthor::perf_block_type::neural_block:
        return std::make_pair(std::error_code{}, block_type::neural_accelerator);
    default:
        return std::make_pair(std::make_error_code(std::errc::invalid_argument), block_type{});
    }

    return std::make_pair(std::make_error_code(std::errc::invalid_argument), block_type{});
}

} // namespace panthor
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
