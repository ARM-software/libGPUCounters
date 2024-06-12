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

/** @file enum_info_parser.hpp */

#pragma once

#include "convert.hpp"
#include "parse_all.hpp"

#include <device/detail/enum_operators.hpp>
#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/prfcnt_set.hpp>
#include <device/ioctl/kbase/commands.hpp>
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
    /** True if GPU supports shader cores cycle counter. */
    bool has_cycles_sc;
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
            return std::make_error_code(std::errc::invalid_argument);

        /* The number of values must be same for all blocks. */
        if (result_.num_values != block.num_values)
            return std::make_error_code(std::errc::invalid_argument);

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
            return std::make_error_code(std::errc::invalid_argument);

        if (block.num_instances > std::numeric_limits<uint8_t>::max())
            return std::make_error_code(std::errc::invalid_argument);

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
            return std::make_error_code(std::errc::function_not_supported);

        using namespace hwcpipe::device::detail::enum_operators;

        const auto request_type_underlying = to_underlying(request.request_item_type);
        assert(request_type_underlying < parsed_requests_.size());

        if (parsed_requests_[request_type_underlying])
            return std::make_error_code(std::errc::invalid_argument);

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
            return std::make_error_code(std::errc::invalid_argument);

        using clock_metadata = ioctl::kinstr_prfcnt::metadata_item::clock_metadata;

        if (sample_info.num_clock_domains > clock_metadata::max_reported_domains)
            return std::make_error_code(std::errc::invalid_argument);

        result_.has_cycles_top = sample_info.num_clock_domains >= 1;
        result_.has_cycles_sc = sample_info.num_clock_domains >= 2;

        parsed_sample_info_ = true;

        return {};
    }

    /** @return Error, if not all required items were parsed. */
    std::error_code on_done() const {
        /* Not all requests were parsed. */
        if (!parsed_requests_.all())
            return std::make_error_code(std::errc::invalid_argument);

        /* Sample info was not parsed. */
        if (!parsed_sample_info_ || !parsed_sentinel_)
            return std::make_error_code(std::errc::invalid_argument);

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
