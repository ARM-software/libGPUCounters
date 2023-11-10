/*
 * Copyright (c) 2021-2023 Arm Limited.
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
 * @file
 *
 * Hardware counters block extents header.
 */

#pragma once

#include <device/hwcnt/block_metadata.hpp>

#include <array>
#include <cassert>
#include <cstddef>
#include <type_traits>

namespace hwcpipe {
namespace device {
namespace hwcnt {

/** Type of the hardware counters values. */
enum class sample_values_type : uint8_t {
    uint32 = 0, /**< Counter values are 32 bit unsigned integers. */
    uint64 = 1, /**< Counter values are 64 bit unsigned integers. */
};

/**
 * Blocks extents class.
 *
 * Stores information about block numbers, counter numbers and
 * their types.
 */
class block_extents {
  public:
    /** Number of block types type. */
    using num_block_types_type = std::underlying_type_t<block_type>;

    /** Number of block types. */
    static const constexpr num_block_types_type num_block_types =
        static_cast<num_block_types_type>(block_type::last) + 1;

    using num_blocks_of_type_type = std::array<uint8_t, num_block_types>;

    /**
     * Construct block extents.
     *
     * @param[in] num_blocks_of_type_v Array of number of blocks on per type basis.
     * @param[in] counters_per_block   Number of counters per block.
     * @param[in] values_type          Hardware counters values type.
     */
    block_extents(num_blocks_of_type_type num_blocks_of_type_v, uint16_t counters_per_block,
                  sample_values_type values_type)
        : num_blocks_of_type_(num_blocks_of_type_v)
        , counters_per_block_(counters_per_block)
        , values_type_(values_type) {}

    /** Default ctor. */
    block_extents() = default;
    /** Default copy ctor. */
    block_extents(const block_extents &) = default;
    /** Default assign. */
    block_extents &operator=(const block_extents &) = default;

    /** @return Number of hardware counters blocks. */
    uint8_t num_blocks() const {
        uint8_t result{};

        for (const auto num_blocks : num_blocks_of_type_)
            result = static_cast<uint8_t>(result + num_blocks);

        return result;
    }

    /**
     * Number of blocks of a given type.
     *
     * @param[in] type Block type.
     * @return Number of blocks of type @p type.
     */
    uint8_t num_blocks_of_type(block_type type) const { return num_blocks_of_type_[static_cast<size_t>(type)]; }

    /** @return number of counters per block. */
    uint16_t counters_per_block() const { return counters_per_block_; }

    /** @return hardware counters value type. */
    sample_values_type values_type() const { return values_type_; }

  private:
    num_blocks_of_type_type num_blocks_of_type_{};
    uint16_t counters_per_block_{};
    sample_values_type values_type_{};
};

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
