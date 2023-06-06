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

#include <device/detail/enum_operators.hpp>
#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sample.hpp>
#include <device/hwcnt/sampler/kinstr_prfcnt/convert.hpp>
#include <device/ioctl/kinstr_prfcnt/types.hpp>

#include <array>
#include <system_error>
#include <utility>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {

/** Metadata parser. */
class metadata_parser {
  public:
    /**
     * Constructor.
     *
     * @param metadata[out]    Sample metadata being parsed.
     * @param extents[in]      Block extents.
     */
    metadata_parser(sample_metadata &metadata, const block_extents &extents)
        : metadata_(metadata)
        , extents_(extents) {}

    /**
     * Parse sample metadata from metadata items iterator.
     *
     * @note this method can only be called once during the parser lifetime.
     *
     * @param begin[in]    Begin iterator.
     * @param remap[in]    Block index remaper.
     * @return Error code.
     */
    template <typename iterator_t, typename block_index_remap_t>
    std::error_code parse_sample(iterator_t begin, const block_index_remap_t &remap) {
        std::error_code ec;

        bool exit_loop = false;
        for (auto it = begin; !exit_loop && !ec; ++it) {
            switch (it->hdr.type) {
            case metadata_item_type::item_type::sample:
                ec = parse_sample_item(it->u.sample_md);
                break;
            case metadata_item_type::item_type::clock:
                ec = parse_clock_item(it->u.clock_md);
                break;
            case metadata_item_type::item_type::block:
                ec = validate_block_item(it->u.block_md, remap);
                break;
            case metadata_item_type::item_type::none:
                exit_loop = true;
                break;
            }
        }

        if (!ec)
            ec = parse_sample_done();

        return ec;
    }

    /**
     * Parse block metadata from metadata items iterator.
     *
     * The algorithm finds block metadata item and parses it. Then the iterator
     * is advanced to the element following the parsed one. If the iterator points
     * to a sentinel item, `false` is returned.
     *
     * @param it[in,out] it    Metadata items iterator.
     * @param mapping[in]      Counters buffer mapping.
     * @param remap[in]        Block index remaper.
     * @return Pair of boolean and metadata parsed. The boolean is set to false
     * if there are no more blocks to parse.
     */
    template <typename iterator_t, typename block_index_remap_t>
    static auto parse_block(iterator_t &it, const uint8_t *mapping, const block_index_remap_t &remap) {
        block_metadata metadata{};

        for (;; ++it) {
            switch (it->hdr.type) {
            case metadata_item_type::item_type::block:
                metadata = parse_block_item(it->u.block_md, mapping, remap);
                ++it;
                return std::make_pair(true, metadata);
            case metadata_item_type::item_type::none:
                return std::make_pair(false, metadata);
            default:
                continue;
            }
        }
    }

  private:
    /**
     * Parse sample metadata item.
     *
     * @param metadata[in] Metadata item to parse.
     * @return Error code.
     */
    std::error_code parse_sample_item(const ioctl::kinstr_prfcnt::metadata_item::sample_metadata &metadata) {
        if (sample_parsed_)
            return std::make_error_code(std::errc::invalid_argument);

        metadata_.user_data = metadata.user_data;
        metadata_.flags = convert(metadata.flags);
        metadata_.sample_nr = metadata.seq;
        metadata_.timestamp_ns_begin = metadata.timestamp_start;
        metadata_.timestamp_ns_end = metadata.timestamp_stop;

        sample_parsed_ = true;

        return std::error_code{};
    }

    /**
     * Parse clock metadata item.
     *
     * @param metadata[in] Metadata item to parse.
     * @return Error code.
     */
    std::error_code parse_clock_item(const ioctl::kinstr_prfcnt::metadata_item::clock_metadata &metadata) {
        if (clock_parsed_)
            return std::make_error_code(std::errc::invalid_argument);

        static constexpr size_t gpu_cycle_idx = 0;
        static constexpr size_t sc_cycle_idx = 1;

        if (metadata.num_domains > gpu_cycle_idx) {
            metadata_.gpu_cycle = metadata.cycles[gpu_cycle_idx];
            metadata_.sc_cycle = metadata.cycles[gpu_cycle_idx];
        }

        if (metadata.num_domains > sc_cycle_idx) {
            metadata_.sc_cycle = metadata.cycles[sc_cycle_idx];
        }

        clock_parsed_ = true;

        return std::error_code{};
    }

    /**
     * Validate block metadata item.
     *
     * @param metadata[in] Metadata item to validate.
     * @param remap[in]    Block index remaper.
     * @return Error code.
     */
    template <typename block_index_remap_t>
    std::error_code validate_block_item(const ioctl::kinstr_prfcnt::metadata_item::block_metadata &metadata,
                                        block_index_remap_t &remap) {
        using hwcpipe::device::detail::enum_operators::to_underlying;

        const auto block_type = convert(metadata.type);
        const auto block_type_underlying = to_underlying(block_type);

        /* Invalid block type. */
        if (block_type_underlying >= num_blocks_of_type_.size())
            return std::make_error_code(std::errc::invalid_argument);

        std::error_code ec;
        uint8_t block_index{};
        std::tie(ec, block_index) = remap.remap(block_type, metadata.block_idx);

        if (ec)
            return ec;

        /* Invalid block index. */
        if (num_blocks_of_type_[block_type_underlying] != block_index)
            return std::make_error_code(std::errc::invalid_argument);

        /* Too many blocks of this type. */
        num_blocks_of_type_[block_type_underlying]++;
        if (num_blocks_of_type_[block_type_underlying] > extents_.num_blocks_of_type(block_type))
            return std::make_error_code(std::errc::invalid_argument);

        ++num_blocks_;

        /* Too many blocks. */
        if (num_blocks_ > extents_.num_blocks())
            return std::make_error_code(std::errc::invalid_argument);

        return std::error_code{};
    }

    /**
     * Parse block metadata item.
     *
     * @param metadata[in]   Metadata item to validate.
     * @param mapping[in]    Hardware counters buffer mapping.
     * @param remap[in]      Block index remaper.
     * @return Error code.
     */
    template <typename block_index_remap_t>
    static block_metadata parse_block_item(const ioctl::kinstr_prfcnt::metadata_item::block_metadata &metadata,
                                           const uint8_t *mapping, const block_index_remap_t &remap) {
        block_metadata result{};
        std::error_code ec;

        result.type = convert(metadata.type);

        std::tie(ec, result.index) = remap.remap(result.type, metadata.block_idx);
        assert(!ec);

        result.set = convert(metadata.set);
        result.state = convert(metadata.block_state);
        result.values = mapping + metadata.values_offset;

        return result;
    }

    /**
     * On sample parse done callback.
     *
     * @return Error code.     *
     */
    std::error_code parse_sample_done() {
        if (!sample_parsed_ || !clock_parsed_)
            return std::make_error_code(std::errc::invalid_argument);

        if (num_blocks_ != extents_.num_blocks())
            return std::make_error_code(std::errc::invalid_argument);

        return std::error_code{};
    }

    /** Metadata item type. */
    using metadata_item_type = ioctl::kinstr_prfcnt::metadata_item;

    /** Sample metadata being parsed. */
    sample_metadata &metadata_;
    /** Block extents. */
    const block_extents &extents_;
    /** Number of blocks parsed so far. */
    size_t num_blocks_{0};
    /** Number of blocks of given type parsed so far. */
    std::array<size_t, block_extents::num_block_types> num_blocks_of_type_{};
    /** True if sample metadata was parsed. */
    bool sample_parsed_{false};
    /** True if clock metadata was parsed. */
    bool clock_parsed_{false};
};

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
