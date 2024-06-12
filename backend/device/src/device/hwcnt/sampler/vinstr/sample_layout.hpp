/*
 * Copyright (c) 2022-2024 Arm Limited.
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

#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/product_id.hpp>
#include <device/shader_core_bitset.hpp>

#include <array>
#include <bitset>
#include <cassert>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace vinstr {

/** Sample layout type. */
enum class sample_layout_type {
    /** v4 layout type. */
    v4,
    /** v5 layout or newer.*/
    non_v4,
};

/**
 * Determine if the block layout is v4 or v5/6 from the product id
 *
 * @param id Product id
 * @return true if the product id given is a gpu, which uses a v4 block layout. false otherwise.
 */
inline bool is_v4_layout(product_id id) {
    switch (id) {
    case product_id::t60x:
    case product_id::t62x:
    case product_id::t720:
    case product_id::t760:
        return true;
    default:
        return false;
    }
}

/** Hardware counters sample memory layout. */
class sample_layout {
  public:
    sample_layout() = default;

    sample_layout(const block_extents &extents, uint64_t num_l2_slices, shader_core_bitset sc_mask,
                  sample_layout_type sample_layout_type)
        : sample_layout_type_(sample_layout_type) {
        assert(extents.counters_per_block() == counters_per_block);
        assert(extents.values_type() == sample_values_type::uint32);
        assert(extents.num_blocks_of_type(block_type::memory) <= num_l2_slices);

        switch (sample_layout_type) {
        case sample_layout_type::v4:
            sample_layout_v4(extents, sc_mask);
            break;
        case sample_layout_type::non_v4:
            sample_layout_non_v4(extents, num_l2_slices, sc_mask);
            break;
        }
    }

    sample_layout(const sample_layout &) = default;
    sample_layout &operator=(const sample_layout &) = default;

    /** Sample layout entry. */
    struct entry {
        /** Block type. */
        block_type type;
        /** Block index. */
        uint8_t index;
        /** Block offset from the sample start. */
        size_t offset;
    };

    /** @return sample layout type. */
    sample_layout_type get_sample_layout_type() const { return sample_layout_type_; }

    /**
     * Look up block entry at a given index.
     *
     * @param[in] index    Block index.
     * @return Block entry.
     */
    const entry &operator[](size_t index) const { return layout_[index]; }

    /** @return Number of block entries. */
    size_t size() const { return num_blocks_; }

  private:
    /**
     * Populate the relevant blocks to support a v4 block layout
     */
    void sample_layout_v4(const block_extents &extents, shader_core_bitset sc_mask) {
        /* Populate shader cores block. */
        static constexpr size_t offset_sc{0};
        if (extents.num_blocks_of_type(block_type::core) != 0) {
            uint8_t sc_index = 0;

            // There can be up to four shader cores.
            assert((sc_mask & shader_core_bitset{0b1111}) == sc_mask);

            for (uint8_t i = 0; i < static_cast<uint8_t>(sc_mask.size()); ++i) {
                if (!sc_mask.test(i))
                    continue;

                push_back({block_type::core, sc_index, offset_sc + block_size * i});
                ++sc_index;
            }
            assert(sc_index == extents.num_blocks_of_type(block_type::core));
        }

        constexpr uint32_t num_shader_cores = 4;

        /* Populate tiler block. */
        constexpr size_t offset_tiler{static_cast<size_t>(offset_sc + block_size * num_shader_cores)};
        if (extents.num_blocks_of_type(block_type::tiler) != 0)
            push_back({block_type::tiler, 0, offset_tiler});

        /* Populate memory block. */
        constexpr size_t offset_memory{offset_tiler + block_size};
        if (extents.num_blocks_of_type(block_type::memory) != 0) {
            assert(extents.num_blocks_of_type(block_type::memory) == 1);
            push_back({block_type::memory, 0, offset_memory});
        }

        /* Populate front-end block. */
        constexpr size_t offset_fe{offset_memory + block_size * 2};
        if (extents.num_blocks_of_type(block_type::fe) != 0)
            push_back({block_type::fe, 0, offset_fe});
    }

    /**
     * Populate the relevant blocks to support a v5/6 block layout
     */
    void sample_layout_non_v4(const block_extents &extents, uint64_t num_l2_slices, shader_core_bitset sc_mask) {
        /* Populate front-end block. */
        static constexpr size_t offset_fe{0};
        if (extents.num_blocks_of_type(block_type::fe) != 0)
            push_back({block_type::fe, 0, offset_fe});

        /* Populate tiler block. */
        constexpr size_t offset_tiler{offset_fe + block_size};
        if (extents.num_blocks_of_type(block_type::tiler) != 0)
            push_back({block_type::tiler, 0, offset_tiler});

        /* Populate memory block. */
        constexpr size_t offset_memory{offset_tiler + block_size};
        if (extents.num_blocks_of_type(block_type::memory) != 0) {
            assert(extents.num_blocks_of_type(block_type::memory) == num_l2_slices);

            for (uint8_t i = 0; i < static_cast<uint8_t>(num_l2_slices); ++i)
                push_back({block_type::memory, i, offset_memory + i * block_size});
        }

        /* Populate shader cores block. */
        const auto offset_sc{static_cast<size_t>(offset_memory + block_size * num_l2_slices)};
        if (extents.num_blocks_of_type(block_type::core) != 0) {
            uint8_t sc_index = 0;

            for (uint8_t i = 0; i < static_cast<uint8_t>(sc_mask.size()); ++i) {
                if (!sc_mask.test(i))
                    continue;

                push_back({block_type::core, sc_index, offset_sc + block_size * i});
                ++sc_index;
            }

            assert(sc_index == extents.num_blocks_of_type(block_type::core));
        }

        assert(size() == extents.num_blocks());
    }

    /**
     * Add layout entry for a block.
     *
     * @param[in] value    Entry to add.
     */
    void push_back(const entry &value) {
        assert(num_blocks_ < layout_.size());

        layout_[num_blocks_] = value;
        ++num_blocks_;
    }

    /** Max number of front end blocks. */
    static constexpr size_t max_blocks_fe = 1;
    /** Max number of tiler blocks. */
    static constexpr size_t max_blocks_tiler = 1;
    /** Max number of memory blocks. */
    static constexpr size_t max_blocks_memory = 16;
    /** Max number of blocks. */
    static constexpr size_t max_blocks = max_blocks_fe + max_blocks_tiler + max_blocks_memory + max_shader_cores;
    /** Counters value type. */
    using counter_value_type = uint32_t;
    /** Number of counters per block. */
    static constexpr size_t counters_per_block = 64;
    /** Counters block size in bytes. */
    static constexpr size_t block_size = counters_per_block * sizeof(uint32_t);

    /** Block entries requested by the user. */
    std::array<entry, max_blocks> layout_{};
    /** Number of blocks requested. */
    size_t num_blocks_{0};
    /** Sample layout type. */
    sample_layout_type sample_layout_type_{};
};

} // namespace vinstr
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
