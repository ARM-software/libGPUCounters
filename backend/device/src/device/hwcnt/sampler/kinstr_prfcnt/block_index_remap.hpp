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

/** @file metadata_parser.hpp */

#pragma once

#include <device/hwcnt/block_metadata.hpp>
#include <device/shader_core_bitset.hpp>

#include <algorithm>
#include <array>
#include <limits>
#include <system_error>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {

/**
 * Shader core block index remapper.
 *
 * Old implementation of kinstr_prfcnt interface has a bug. For shader_core
 * hardware counters block `block_metadata::block_idx` is set to the
 * shader core index rather to an index of the counters block. For example,
 * if `core_mask` is `0b1011`, the indices will be 0, 1, and 3. This class
 * re-enumerates the shader cores counters blocks s.t. the indexes are
 * contiguous.
 */
class block_index_remap {
    /** Index type. */
    using index_type = uint8_t;

  public:
    /**
     * Constructor.
     *
     * @param sc_mask Shader cores bitset.
     */
    block_index_remap(shader_core_bitset sc_mask) {
        std::fill(map_.begin(), map_.end(), invalid_index);

        uint8_t blk_index{};

        for (size_t sc_index = 0; sc_index < sc_mask.size(); ++sc_index) {
            if (sc_mask.test(sc_index)) {
                map_[sc_index] = blk_index;
                ++blk_index;
            }
        }
    }

    /**
     * Remap block index.
     *
     * @param type Block type.
     * @param index Index to remap.
     * @return A pair of error code and remapped index value.
     */
    auto remap(block_type type, index_type index) const {
        if (type != block_type::core)
            return std::make_pair(std::error_code{}, index);

        if (index >= map_.size() || map_[index] == invalid_index)
            return std::make_pair(std::make_error_code(std::errc::invalid_argument), index_type{});

        return std::make_pair(std::error_code{}, map_[index]);
    }

  private:
    /** Invalid index value. */
    static constexpr index_type invalid_index = std::numeric_limits<index_type>::max();
    /** Indices map for remapping. */
    std::array<uint8_t, max_shader_cores> map_{};
};

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
