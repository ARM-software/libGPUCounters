/*
 * Copyright (c) 2022-2026 Arm Limited.
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

/** @file enum_info_parser.hpp */

#pragma once

#include "convert.hpp"
#include "parse_all.hpp"

#include <device/detail/enum_operators.hpp>
#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/detail/to_str.hpp>
#include <device/hwcnt/prfcnt_set.hpp>
#include <device/ioctl/kbase/commands.hpp>
#include <device/ioctl/kinstr_prfcnt/print.hpp>
#include <device/ioctl/kinstr_prfcnt/types.hpp>
#include <device/ioctl/strided_array_view.hpp>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <system_error>
#include <tuple>
#include <utility>
#include <vector>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {

using hwcpipe::device::hwcnt::detail::to_str;

/** Parsed enum info structure. */
struct enum_info {
    /** Performance counters set. */
    prfcnt_set set;
    /** Num values per block. */
    uint16_t num_values;
    /** Num blocks of type for each block. */
    block_extents::num_blocks_of_type_type num_blocks_of_type;
    /** True if GPU supports top cycle counter. */
    bool has_cycles_top;
    /** True if GPU supports core group cycle counter. */
    bool has_cycles_cg;
    /** True if GPU supports shader cores cycle counter. */
    bool has_cycles_sc;
    /** True if GPU supports ne cycle counter. */
    bool has_cycles_ne;
};

/** Enum info parser implementation. */
class enum_info_parser {
    /** Enum item type. */
    using enum_item_type = ioctl::kinstr_prfcnt::enum_item;

  public:
    /** Type tag to member mapping. */
    static constexpr auto type2member = std::make_tuple(
        type2member_entry(enum_item_type::item_type::block, &enum_item_type::enum_union::block_counter),
        type2member_entry(enum_item_type::item_type::request, &enum_item_type::enum_union::request),
        type2member_entry(enum_item_type::item_type::sample_info, &enum_item_type::enum_union::sample_info));

    /** Type to member mapping type. */
    using type2member_type = decltype(type2member);

    enum_info_parser(enum_info &result)
        : result_(result) {}

    /** Prase block enum item.
     *
     * @param[in] block    Block item to parse.
     * @return Error code.
     */
    std::error_code on_item(const enum_item_type::enum_block_counter &block) {
        if (block.num_values == 0 && block.num_instances == 0) {
            parsed_sentinel_ = true;
            return {};
        }

        if (parsed_blocks_.none()) {
            result_.set = convert(block.set);
            result_.num_values = block.num_values;
        }

        /* Performance counters set must be same for all blocks. */
        if (result_.set != convert(block.set))
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::extents_invalid_set,
                                           "Parsing block (type %s) set (%s) doesn't match previous blocks' sets (%s)",
                                           to_str(block.type).c_str(), to_str(block.set).c_str(),
                                           to_str(convert(result_.set)).c_str());

        /* The number of values must be same for all blocks. */
        if (result_.num_values != block.num_values)
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::extents_invalid_num_values,
                                           "Parsing block type (%s) number of samples (%i) doesn't match previous "
                                           "blocks' number of samples (%i)",
                                           to_str(block.type).c_str(), block.num_values, result_.num_values);

        std::error_code ec;
        block_type type{};
        std::tie(ec, type) = convert(block.type);

        /* Ignore unknown block types. */
        if (ec)
            return {};

        using namespace hwcpipe::device::detail::enum_operators;

        const auto block_underlying = to_underlying(type);

        /* There must be only one entry for a block type. */
        if (parsed_blocks_[block_underlying])
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::extents_invalid_block, "BLock type %s already parsed",
                                           to_str(type).c_str());

        if (block.num_instances > std::numeric_limits<uint8_t>::max())
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::extents_invalid_num_instances,
                                           "Block %s has max number of block instances (%u)", to_str(type).c_str(),
                                           block.num_instances);

        parsed_blocks_[block_underlying] = true;
        result_.num_blocks_of_type[block_underlying] = static_cast<uint8_t>(block.num_instances);

        return {};
    }

    /**
     * Parse request enum item.
     *
     * @param[in] request Request item to parse.
     * @return Error code.
     */
    std::error_code on_item(const enum_item_type::enum_request &request) {
        using request_type = enum_item_type::enum_request::request_type;

        /* Ignore unknown requests. */
        switch (request.request_item_type) {
        case request_type::mode:
        case request_type::enable:
            break;
        default:
            return {};
        }

        const bool supported = request.versions_mask & (1 << ioctl::kinstr_prfcnt::api_version);
        if (!supported)
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::extents_version_not_supported,
                                           "Unsupported version mask = 0x%X", request.versions_mask);

        using namespace hwcpipe::device::detail::enum_operators;

        const auto request_type_underlying = to_underlying(request.request_item_type);
        assert(request_type_underlying < parsed_requests_.size());

        if (parsed_requests_[request_type_underlying])
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::extents_duplicate_request_type, "Request %s already parsed",
                                           to_str(request.request_item_type).c_str());

        parsed_requests_[request_type_underlying] = true;

        return {};
    }

    /**
     * Parse sample info enum item.
     *
     * @param[in] sample_info Sample info item to parse.
     * @return Error code.
     */
    std::error_code on_item(const enum_item_type::enum_sample_info &sample_info) {
        /* There must be only one sample_info entry. */
        if (parsed_sample_info_)
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::extents_duplicate_sample_info, "Sample already parsed: %s",
                                           to_str(sample_info).c_str());

        using clock_metadata = ioctl::kinstr_prfcnt::metadata_item::clock_metadata;

        if (sample_info.num_clock_domains > clock_metadata::max_reported_domains)
            return HWCPIPE_MAKE_ERROR_CODE(
                hwcpipe_errc::extents_invalid_num_clock_domains,
                "Sample: %s reporting number of clock domains (%u) > max_reported_domains (%u)",
                to_str(sample_info).c_str(), sample_info.num_clock_domains,
                static_cast<int>(clock_metadata::max_reported_domains));

        /*
         * Kbase tell us the number of clock domains enabled rather than the which specific ones are enabled so we
         * assume that if N clock domains are enabled, then they are the first N domains in the order defined by
         * the clock_metadata structure.
         * Order of clock domains is defined in clock_metadata structure as:
         * - top_clock: GPU top level clock domain
         * - cg_clock: core group clock domain (MMU, L2 cache, Tile, etc.)
         * - sc_clock: shader core clock domain
         * - ne_clock: neural accelerator clock domain
         */
        result_.has_cycles_top = sample_info.num_clock_domains >= 1;
        result_.has_cycles_cg = sample_info.num_clock_domains >= 2;
        result_.has_cycles_sc = sample_info.num_clock_domains >= 3;
        result_.has_cycles_ne = sample_info.num_clock_domains >= 4;

        parsed_sample_info_ = true;

        return {};
    }

    /** @return Error, if not all required items were parsed. */
    std::error_code on_done() const {
        /* Not all requests were parsed. */
        if (!parsed_requests_.all())
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::parser_not_done, "Parsed requests = %s",
                                           to_str(parsed_requests_).c_str());

        /* Sample info was not parsed. */
        if (!parsed_sample_info_ || !parsed_sentinel_)
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::parser_not_done, "These are not parsed: (%s %s)",
                                           parsed_sample_info_ ? "" : "sample_info,",
                                           parsed_sentinel_ ? "" : "sentinel)");

        return std::error_code{};
    }

    /** @return True if the sentinel item was parsed. */
    bool sentinel_parsed() const { return parsed_sentinel_; }

  private:
    /** Enum info being parsed. */
    enum_info &result_;
    /** Set of blocks parsed. */
    std::bitset<block_extents::num_block_types> parsed_blocks_{};
    /** Requests number. */
    static constexpr size_t num_requests = 2;
    /** Set of requests parsed. */
    std::bitset<num_requests> parsed_requests_{};
    /** True if sample info item was parsed. */
    bool parsed_sample_info_{false};
    /** True if sentinel item was parsed. */
    bool parsed_sentinel_{false};
};

template <typename syscal_ifcace_t>
static auto parse_enum_info(int device_fd, syscal_ifcace_t &&iface = {}) {
    ioctl::kbase::kinstr_prfcnt_enum_info ei{};
    std::error_code ec;

    std::tie(ec, std::ignore) = iface.ioctl(device_fd, ioctl::kbase::command::kinstr_prfcnt_enum_info, &ei);

    if (ec)
        return std::make_pair(ec, enum_info{});

    using enum_item_type = ioctl::kinstr_prfcnt::enum_item;

    const auto memory_size = ei.info_item_size * ei.info_item_count;
    std::vector<uint8_t> memory(memory_size);
    void *const memory_ptr = memory.data();

    ei.info_list_ptr.reset(static_cast<enum_item_type *>(memory_ptr));

    std::tie(ec, std::ignore) = iface.ioctl(device_fd, ioctl::kbase::command::kinstr_prfcnt_enum_info, &ei);

    if (ec)
        return std::make_pair(ec, enum_info{});

    ioctl::strided_array_view<enum_item_type> enum_item_view(
        ei.info_list_ptr.get(), static_cast<ptrdiff_t>(ei.info_item_size), ei.info_item_count);

    enum_info result{};
    enum_info_parser parser{result};

    ec = parse_all(enum_item_view.begin(), enum_item_view.end(), parser);

    return std::make_pair(ec, result);
}

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
