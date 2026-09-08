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

/** @file sample_layout.hpp */

#pragma once

#include <device/error.hpp>
#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sampler/configuration.hpp>
#include <device/product_id.hpp>
#include <device/shader_core_bitset.hpp>

#include <array>
#include <cassert>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace panthor {

/** Hardware counters sample memory layout. */
class sample_layout {
  public:
    sample_layout() = default;
    ~sample_layout() = default;

    /**
     * Construct sample layout for Panthor.
     *
     * Sample structure (byte layout):
     *   - [sample header]            sample_header_size_ bytes at offset 0
     *   - [block 0 header]           block_header_size_ bytes
     *   - [block 0 counters]         counters_per_block_ * sizeof(uint64_t) bytes
     *   - [block 1 header]
     *   - [block 1 counters]
     *   - ...
     *
     * Each block is contiguous and has a fixed size:
     *   block_size = block_header_size_ + counters_per_block_ * sizeof(uint64_t).
     *
     * The layout enumerates all blocks in the order of block types and indices
     * reported by the extents. Panthor samples contain all available blocks,
     * even if they are disabled at collection time.
     */
    sample_layout(const block_extents &extents, size_t sample_header_size, size_t block_header_size)
        : counters_per_block_{extents.counters_per_block()}
        , block_size_{counters_per_block_ * sizeof(uint64_t)}
        , sample_header_size_(sample_header_size)
        , block_header_size_(block_header_size) {
        assert(extents.values_type() == sample_values_type::uint64);

        size_t offset = sample_header_size_;
        const size_t block_size = block_header_size_ + block_size_;

        for (uint8_t i = 0; i < extents.num_block_types; i++) {
            for (uint8_t j = 0; j < extents.num_blocks_of_type(static_cast<block_type>(i)); ++j) {
                push_back({static_cast<block_type>(i), j, offset, offset + block_header_size_});
                offset += block_size;
            }
        }

        sample_size_ = offset;

        assert(size() == extents.num_blocks());
    }

    sample_layout(const sample_layout &) = default;
    sample_layout &operator=(const sample_layout &) = default;

    /** Sample layout entry. */
    struct entry {
        /** Block type. */
        block_type type;
        /** Block index. */
        uint8_t index;
        /** Header offset from the sample start. */
        size_t header_offset;
        /** Block offset from the sample start. */
        size_t block_offset;
    };

    /**
     * Look up block entry at a given index.
     *
     * @param[in] index    Block index.
     * @return Block entry.
     */
    const entry &operator[](size_t index) const {
        assert(index < num_blocks_);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        return layout_[index];
    }

    /** @return Number of block entries. */
    size_t size() const { return num_blocks_; }

    size_t sample_size() const { return sample_size_; }

    size_t block_size() const { return block_header_size_ + block_size_; }

    size_t sample_header_size() const { return sample_header_size_; }

    size_t block_header_size() const { return block_header_size_; }

  private:
    /**
     * Add layout entry for a block.
     *
     * @param[in] value    Entry to add.
     */
    void push_back(const entry &value) {
        assert(num_blocks_ < layout_.size());

        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        layout_[num_blocks_] = value;
        ++num_blocks_;
    }

    /** Max number of FW blocks. */
    static constexpr size_t max_blocks_fw{1};
    /** Max number of CSG blocks. */
    static constexpr size_t max_blocks_csg{64};
    /** Max number of front end blocks. */
    static constexpr size_t max_blocks_fe{1};
    /** Max number of tiler blocks. */
    static constexpr size_t max_blocks_tiler{64};
    /** Max number of memory blocks. */
    static constexpr size_t max_blocks_memory{16};
    /** Max number of shader cores. */
    static constexpr size_t max_shader_cores{64};
    /** Max number of neural accelerator cores. */
    static constexpr size_t max_neural_accelerators{64};
    /** Max number of blocks. */
    static constexpr size_t max_blocks{max_blocks_fw + max_blocks_fe + max_blocks_tiler + max_blocks_memory +
                                       max_shader_cores + max_neural_accelerators};
    /** Counters value type. */
    using counter_value_type = uint64_t;
    /** Number of counters per block. */
    size_t counters_per_block_{};
    /** Counters block size in bytes. */
    size_t block_size_{};
    /** Block entries requested by the user. */
    std::array<entry, max_blocks> layout_{};
    /** Number of blocks requested. */
    size_t num_blocks_{0};
    size_t sample_header_size_{};
    size_t block_header_size_{};
    size_t sample_size_{};
};

} // namespace panthor
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
