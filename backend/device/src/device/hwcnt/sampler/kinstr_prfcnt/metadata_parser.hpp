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

#include "block_index_remap.hpp"
#include "parse_all.hpp"

#include <device/detail/enum_operators.hpp>
#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sample.hpp>
#include <device/hwcnt/sampler/kinstr_prfcnt/convert.hpp>

#include <tuple>
#include <utility>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {

/** Metadata parser. */
class metadata_parser {
    /** Metadata item type. */
    using metadata_item = ioctl::kinstr_prfcnt::metadata_item;

  public:
    /** Type tag to member mapping. */
    static constexpr auto type2member =
        std::make_tuple(type2member_entry(metadata_item::item_type::none),
                        type2member_entry(metadata_item::item_type::sample, &metadata_item::metadata_union::sample_md),
                        type2member_entry(metadata_item::item_type::clock, &metadata_item::metadata_union::clock_md),
                        type2member_entry(metadata_item::item_type::block, &metadata_item::metadata_union::block_md));

    using type2member_type = decltype(type2member);

    /**
     * Constructor.
     *
     * @param[out] metadata Sample metadata being parsed.
     * @param[in]  extents  Block extents.
     * @param[in]  remap    Block index remap.
     */
    metadata_parser(sample_metadata &metadata, const block_extents &extents, const block_index_remap *remap)
        : result_(metadata)
        , extents_(extents)
        , remap_(remap) {}

    /**
     * Parse sample metadata item.
     *
     * @param[in] metadata Metadata item to parse.
     * @return Error code.
     */
    std::error_code on_item(const ioctl::kinstr_prfcnt::metadata_item::sample_metadata &metadata) {
        if (sample_parsed_)
            return std::make_error_code(std::errc::invalid_argument);

        result_.user_data = metadata.user_data;
        result_.flags = convert(metadata.flags);
        result_.sample_nr = metadata.seq;
        result_.timestamp_ns_begin = metadata.timestamp_start;
        result_.timestamp_ns_end = metadata.timestamp_stop;

        sample_parsed_ = true;

        return std::error_code{};
    }

    /**
     * Parse clock metadata item.
     *
     * @param[in] metadata Metadata item to parse.
     * @return Error code.
     */
    std::error_code on_item(const ioctl::kinstr_prfcnt::metadata_item::clock_metadata &metadata) {
        if (clock_parsed_)
            return std::make_error_code(std::errc::invalid_argument);

        static constexpr size_t gpu_cycle_idx = 0;
        static constexpr size_t sc_cycle_idx = 1;

        if (metadata.num_domains > gpu_cycle_idx) {
            result_.gpu_cycle = metadata.cycles[gpu_cycle_idx];
            result_.sc_cycle = metadata.cycles[gpu_cycle_idx];
        }

        if (metadata.num_domains > sc_cycle_idx) {
            result_.sc_cycle = metadata.cycles[sc_cycle_idx];
        }

        clock_parsed_ = true;

        return std::error_code{};
    }

    /**
     * Validate block metadata item.
     *
     * @param[in] metadata Metadata item to validate.
     * @return Error code.
     */
    std::error_code on_item(const ioctl::kinstr_prfcnt::metadata_item::block_metadata &metadata) {
        using hwcpipe::device::detail::enum_operators::to_underlying;

        std::error_code ec;
        block_type type{};

        std::tie(ec, type) = convert(metadata.type);
        if (ec)
            return ec;

        const auto block_type_underlying = to_underlying(type);

        /* Invalid block type. */
        if (block_type_underlying >= num_blocks_of_type_.size())
            return std::make_error_code(std::errc::invalid_argument);

        uint8_t block_index = metadata.block_idx;
        if (remap_ != nullptr)
            std::tie(ec, block_index) = remap_->remap(type, metadata.block_idx);

        if (ec)
            return ec;

        /* Invalid block index. */
        if (num_blocks_of_type_[block_type_underlying] != block_index)
            return std::make_error_code(std::errc::invalid_argument);

        /* Too many blocks of this type. */
        num_blocks_of_type_[block_type_underlying]++;
        if (num_blocks_of_type_[block_type_underlying] > extents_.num_blocks_of_type(type))
            return std::make_error_code(std::errc::invalid_argument);

        ++num_blocks_;

        return std::error_code{};
    }

    /**
     * Parse sentinel item.
     *
     * @return Error code.
     */
    std::error_code on_item() {
        sentinel_parsed_ = true;
        return {};
    }

    /**
     * On sample parse done callback.
     *
     * @return Error code.     *
     */
    std::error_code on_done() {
        if (!sample_parsed_ || !clock_parsed_)
            return std::make_error_code(std::errc::invalid_argument);

        if (num_blocks_ != extents_.num_blocks())
            return std::make_error_code(std::errc::invalid_argument);

        return std::error_code{};
    }

    /** True if sentinel item was parsed. */
    bool sentinel_parsed() const { return sentinel_parsed_; }

    /**
     * Obtain metadata end iterator.
     *
     * @param[in] begin Metadata items begin iterator.
     * @return Metadata items end iterator.
     */
    template <typename iterator_t>
    iterator_t end(iterator_t begin) const {
        static constexpr size_t non_block_metadata_items = 3;
        return begin + extents_.num_blocks() + non_block_metadata_items;
    }

  private:
    /** Sample metadata being parsed. */
    sample_metadata &result_;
    /** Block extents. */
    const block_extents &extents_;
    /** Block index remap. */
    const block_index_remap *remap_;
    /** Number of blocks parsed so far. */
    size_t num_blocks_{0};
    /** Number of blocks of given type parsed so far. */
    std::array<size_t, block_extents::num_block_types> num_blocks_of_type_{};
    /** True if sample metadata was parsed. */
    bool sample_parsed_{false};
    /** True if clock metadata was parsed. */
    bool clock_parsed_{false};
    /** True if sentinel item was parsed. */
    bool sentinel_parsed_{false};
};

/**
 * Parse block metadata from metadata items iterator.
 *
 * The algorithm finds block metadata item and parses it. Then the iterator
 * is advanced to the element following the parsed one. If the iterator points
 * to a sentinel item, `false` is returned.
 *
 * @param[out]    result   Block metadata parsed.
 * @param[in,out] it       Metadata items iterator.
 * @param[in]     mapping  Counters buffer mapping.
 * @param[in]     remap    Block index remaper.
 * @return Pair of boolean and metadata parsed. The boolean is set to false
 * if there are no more blocks to parse.
 */
template <typename iterator_t>
inline bool parse_block_item(block_metadata &result, iterator_t &it, const uint8_t *mapping,
                             const block_index_remap *remap) {
    using metadata_item = ioctl::kinstr_prfcnt::metadata_item;
    std::error_code ec;

    for (;; ++it) {
        switch (it->hdr.type) {
        case metadata_item::item_type::block: {
            const auto &metadata = it->u.block_md;

            std::tie(ec, result.type) = convert(metadata.type);
            assert(!ec);

            result.index = metadata.block_idx;

            if (remap != nullptr) {
                std::tie(ec, result.index) = remap->remap(result.type, metadata.block_idx);
                assert(!ec);
            }

            result.set = convert(metadata.set);
            result.state = convert(metadata.block_state);
            result.values = mapping + metadata.values_offset;

            ++it;
            return true;
        }
        case metadata_item::item_type::none:
            return false;
        default:
            continue;
        }
    }
}

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
