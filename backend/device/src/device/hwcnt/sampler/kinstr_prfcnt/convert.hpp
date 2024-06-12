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

/** @file convert.hpp */

#pragma once

#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sample.hpp>
#include <device/hwcnt/sampler/configuration.hpp>
#include <device/ioctl/kinstr_prfcnt/types.hpp>

#include <array>
#include <bitset>
#include <cassert>
#include <limits>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {

/**
 * Convert from kinstr_prfcnt block_type to hwcpipe block_type.
 *
 * @param[in] value Value to convert.
 * @return A pair of error code and value converted.
 */
inline std::pair<std::error_code, block_type> convert(ioctl::kinstr_prfcnt::block_type value) {
    switch (value) {
    case ioctl::kinstr_prfcnt::block_type::fe:
        return std::make_pair(std::error_code{}, block_type::fe);
    case ioctl::kinstr_prfcnt::block_type::tiler:
        return std::make_pair(std::error_code{}, block_type::tiler);
    case ioctl::kinstr_prfcnt::block_type::memory:
        return std::make_pair(std::error_code{}, block_type::memory);
    case ioctl::kinstr_prfcnt::block_type::shader_core:
        return std::make_pair(std::error_code{}, block_type::core);
    case ioctl::kinstr_prfcnt::block_type::firmware:
        return std::make_pair(std::error_code{}, block_type::firmware);
    case ioctl::kinstr_prfcnt::block_type::csg:
        return std::make_pair(std::error_code{}, block_type::csg);
    }

    return std::make_pair(std::make_error_code(std::errc::invalid_argument), block_type{});
}

/**
 * Convert from hwcpipe block_type to kinstr_prfcnt block_type.
 *
 * @param[in] value Value to convert.
 * @return Converted value.
 */
inline ioctl::kinstr_prfcnt::block_type convert(block_type value) {
    switch (value) {
    case block_type::fe:
        return ioctl::kinstr_prfcnt::block_type::fe;
    case block_type::tiler:
        return ioctl::kinstr_prfcnt::block_type::tiler;
    case block_type::memory:
        return ioctl::kinstr_prfcnt::block_type::memory;
    case block_type::core:
        return ioctl::kinstr_prfcnt::block_type::shader_core;
    case block_type::firmware:
        return ioctl::kinstr_prfcnt::block_type::firmware;
    case block_type::csg:
        return ioctl::kinstr_prfcnt::block_type::csg;
    }

    assert(!&"Unexpected block_type value");
    __builtin_unreachable();
}

/**
 * Convert from kinstr_prfcnt prfcnt_set to hwcpipe prfcnt_set.
 *
 * @param[in] value Value to convert.
 * @return Converted value.
 */
inline prfcnt_set convert(ioctl::kinstr_prfcnt::prfcnt_set value) {
    switch (value) {
    case ioctl::kinstr_prfcnt::prfcnt_set::primary:
        return prfcnt_set::primary;
    case ioctl::kinstr_prfcnt::prfcnt_set::secondary:
        return prfcnt_set::secondary;
    case ioctl::kinstr_prfcnt::prfcnt_set::tertiary:
        return prfcnt_set::tertiary;
    }

    assert(!&"Unexpected ioctl::kinstr_prfcnt::prfcnt_set value");
    __builtin_unreachable();
}

/**
 * Convert from hwcpipe prfcnt_set to kinstr_prfcnt prfcnt_set.
 *
 * @param[in] value Value to convert.
 * @return Converted value.
 */
inline ioctl::kinstr_prfcnt::prfcnt_set convert(prfcnt_set value) {
    switch (value) {
    case prfcnt_set::primary:
        return ioctl::kinstr_prfcnt::prfcnt_set::primary;
    case prfcnt_set::secondary:
        return ioctl::kinstr_prfcnt::prfcnt_set::secondary;
    case prfcnt_set::tertiary:
        return ioctl::kinstr_prfcnt::prfcnt_set::tertiary;
    }

    assert(!&"Unexpected prfcnt_set value");
    __builtin_unreachable();
}

/**
 * Convert from kinstr_prfcnt block state to hwcpipe block state.
 *
 * @param[in] value Value to convert.
 * @return Converted value.
 */
inline block_state convert(ioctl::kinstr_prfcnt::metadata_item::block_metadata::block_state_type value) {
    using block_state_type = ioctl::kinstr_prfcnt::metadata_item::block_metadata::block_state_type;

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
 * Convert from kinstr_prfcnt sample flag to hwcpipe sample flag.
 *
 * @param[in] value    Value to convert.
 * @return Converted value.
 */
inline sample_flags convert(ioctl::kinstr_prfcnt::metadata_item::sample_metadata::sample_flag value) {
    using ioctl::kinstr_prfcnt::metadata_item;

    sample_flags result{};

    result.error = !!(value & metadata_item::sample_metadata::sample_flag::error);
    result.stretched = !!(value & metadata_item::sample_metadata::sample_flag::overflow);

    return result;
}

/**
 * Convert from hwcpipe enable map to kinstr_prfcnt enable map.
 *
 * @param[in] value    Value to convert.
 * @return Converted value.
 */
inline std::array<uint64_t, 2> convert(configuration::enable_map_type value) {
    configuration::enable_map_type mask_low64{std::numeric_limits<uint64_t>::max()};
    constexpr uint64_t uint64_t_bits = 64;
    return {
        (value & mask_low64).to_ullong(),
        ((value >> uint64_t_bits) & mask_low64).to_ullong(),
    };
}

/**
 * Convert from kinstr_prfcnt enable map to hwcpipe enable map.
 *
 * @param[in] value    Value to convert.
 * @return Converted value.
 */
inline configuration::enable_map_type convert(const std::array<uint64_t, 2> &value) {
    constexpr uint64_t uint64_t_bits = 64;

    return (configuration::enable_map_type{value[1]} << uint64_t_bits) | configuration::enable_map_type{value[0]};
}

/**
 * Convert from hwcpipe sampler configuration to kinstr_prfcnt request item.
 *
 * @param[in] value    Value to convert.
 * @return Converted value.
 */
inline ioctl::kinstr_prfcnt::request_item convert(const configuration &value) {
    using ioctl::kinstr_prfcnt::request_item;

    request_item result{};

    result.hdr.type = request_item::item_type::enable;
    result.hdr.item_version = ioctl::kinstr_prfcnt::api_version;

    result.u.req_enable.type = convert(value.type);
    result.u.req_enable.set = convert(value.set);

    const auto enable_map_array = convert(value.enable_map);
    result.u.req_enable.enable_mask[0] = enable_map_array[0];
    result.u.req_enable.enable_mask[1] = enable_map_array[1];

    return result;
}

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
