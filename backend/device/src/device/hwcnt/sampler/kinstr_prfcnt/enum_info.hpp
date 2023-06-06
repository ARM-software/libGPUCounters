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

/** @file enum_info.hpp */

#pragma once

#include "convert.hpp"

#include <device/detail/enum_operators.hpp>
#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/prfcnt_set.hpp>
#include <device/ioctl/kbase/commands.hpp>
#include <device/ioctl/kinstr_prfcnt/types.hpp>
#include <device/ioctl/strided_array_view.hpp>

#include <algorithm>
#include <cstddef>
#include <system_error>
#include <tuple>
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
    /** Blocks description. */
    struct block {
        uint16_t num_instances;
    };
    /** Blocks data. */
    std::array<block, block_extents::num_block_types> blocks;
    /** True if GPU supports top cycle counter. */
    bool has_cycles_top;
    /** True if GPU supports shader cores cycle counter. */
    bool has_cycles_sc;
};

namespace detail {
/** Enum info parser implementation. */
class parser_impl {
  public:
    /**
     * Parse enum info.
     *
     * @note this method can only be called once during the parser lifetime.
     *
     * @param begin[in]    Enum info items begin iterator.
     * @param end[in]      Enum info items end iterator.
     * @return A pair or error code and enum info parsed.
     */
    template <typename iterator_t>
    auto parse(iterator_t begin, iterator_t end) {
        std::error_code ec;

        for (auto it = begin; it != end && !ec; ++it) {
            /* We do not expect items after sentinel. */
            if (parsed_sentinel_)
                return std::make_pair(std::make_error_code(std::errc::invalid_argument), result_);

            switch (it->hdr.type) {
            case enum_item_type::item_type::block:
                ec = parse_block(it->u.block_counter);
                break;
            case enum_item_type::item_type::request:
                ec = parse_request(it->u.request);
                break;
            case enum_item_type::item_type::sample_info:
                ec = parse_sample(it->u.sample_info);
                break;
            }
        }

        if (!ec)
            ec = parse_done();

        return std::make_pair(ec, result_);
    }

  private:
    /** Enum item type. */
    using enum_item_type = ioctl::kinstr_prfcnt::enum_item;

    /** Prase block enum item.
     *
     * @param block[in]    Block item to parse.
     * @return Error code.
     */
    std::error_code parse_block(const enum_item_type::enum_block_counter &block) {
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

        const auto block_type = convert(block.type);

        using namespace hwcpipe::device::detail::enum_operators;
        const auto block_underlying = to_underlying(block_type);

        /* Bad block index. */
        if (block_underlying >= result_.blocks.size())
            return std::make_error_code(std::errc::invalid_argument);

        assert(block_underlying < parsed_blocks_.size());

        /* There must be only one entry for a block type. */
        if (parsed_blocks_[block_underlying])
            return std::make_error_code(std::errc::invalid_argument);

        parsed_blocks_[block_underlying] = true;
        result_.blocks[block_underlying].num_instances = block.num_instances;

        return {};
    }

    /**
     * Parse request enum item.
     *
     * @param request[in] Request item to parse.
     * @return Error code.
     */
    std::error_code parse_request(const enum_item_type::enum_request &request) {
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
     * @param request[in] Sample info item to parse.
     * @return Error code.
     */
    std::error_code parse_sample(const enum_item_type::enum_sample_info &sample_info) {
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
    std::error_code parse_done() {
        /* Not all blocks were parsed. */
        if (!parsed_blocks_.all())
            return std::make_error_code(std::errc::invalid_argument);

        /* Not all requests were parsed. */
        if (!parsed_requests_.all())
            return std::make_error_code(std::errc::invalid_argument);

        /* Sample info was not parsed. */
        if (!parsed_sample_info_ || !parsed_sentinel_)
            return std::make_error_code(std::errc::invalid_argument);

        return {};
    }

    /** Enum info being parsed. */
    enum_info result_{};
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
} // namespace detail

/** Enum info parser. */
class enum_info_parser {
  public:
    /**
     * Parse enum_info using `kinstr_prfcnt_enum_info` command.
     *
     * @param device_fd[in]    Device file descriptor.
     * @param iface[in]        Syscall iface (test only).
     * @return A pair of error code and enum_info instance parsed.
     */
    template <typename syscal_ifcace_t, typename parser_t = detail::parser_impl>
    static auto parse_enum_info(int device_fd, syscal_ifcace_t &&iface = {}, parser_t &&parser = {}) {
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

        return parser.parse(enum_item_view.begin(), enum_item_view.end());
    }
};

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
