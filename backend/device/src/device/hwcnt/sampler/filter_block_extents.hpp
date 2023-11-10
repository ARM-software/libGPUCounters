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

/** @file filter_block_extents.hpp */

#pragma once

#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/sampler/configuration.hpp>

#include <system_error>
#include <utility>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
/**
 * Filter block extents.
 *
 * @param[in] extents     Extents to filter.
 * @param[in] begin       Counters configuration begin.
 * @param[in] end         Counters configuration end.
 * @return Pair of error code and block extents filtered.
 */
inline auto filter_block_extents(const block_extents &extents, const configuration *begin, const configuration *end) {
    block_extents::num_blocks_of_type_type num_blocks_of_type{};

    for (auto it = begin; it != end; ++it) {
        // Disallow block configuration if it was not advertised by `instance`.
        if (extents.num_blocks_of_type(it->type) == 0)
            return std::make_pair(std::make_error_code(std::errc::invalid_argument), block_extents{});

        const auto block_type_idx = static_cast<size_t>(it->type);

        // Disallow configuring a block twice.
        if (num_blocks_of_type[block_type_idx])
            return std::make_pair(std::make_error_code(std::errc::invalid_argument), block_extents{});

        num_blocks_of_type[block_type_idx] = extents.num_blocks_of_type(it->type);
    }

    const block_extents result{num_blocks_of_type, extents.counters_per_block(), extents.values_type()};
    return std::make_pair(std::error_code{}, result);
}

} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
