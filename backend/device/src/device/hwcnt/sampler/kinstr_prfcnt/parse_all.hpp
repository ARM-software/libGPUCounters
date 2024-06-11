/*
 * Copyright (c) 2023 Arm Limited.
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

#include <iterator>
#include <system_error>
#include <tuple>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {

namespace detail {
///@cond NO_DOCS
template <typename parser_t, typename item_t, typename item_tag_t, typename mem_ptr_t>
std::error_code call_on_item(parser_t &parser, const item_t &item,
                             std::tuple<item_tag_t, mem_ptr_t> type2member_entry) {
    return parser.on_item(item.u.*std::get<1>(type2member_entry));
}

template <typename parser_t, typename item_t, typename item_tag_t>
std::error_code call_on_item(parser_t &parser, const item_t &, std::tuple<item_tag_t>) {

    return parser.on_item();
}

template <std::size_t index_v, typename parser_t, typename item_t, typename... args_t>
std::enable_if_t<index_v == sizeof...(args_t), std::error_code> dispatch_on_item(parser_t &, const item_t &,
                                                                                 std::tuple<args_t...>) {
    return std::make_error_code(std::errc::invalid_argument);
}

template <std::size_t index_v, typename parser_t, typename item_t, typename... args_t>
    std::enable_if_t < index_v<sizeof...(args_t), std::error_code>
                       dispatch_on_item(parser_t &parser, const item_t &item, std::tuple<args_t...> type2member) {
    const auto type2member_entry = std::get<index_v>(parser_t::type2member);

    if (std::get<0>(type2member_entry) == item.hdr.type) {
        return call_on_item(parser, item, std::get<index_v>(type2member));
    }

    return dispatch_on_item<index_v + 1>(parser, item, type2member);
}

///@endcond NO_DOCS
} // namespace detail

/**
 * Declare mapping from tagged union tag to the union member.
 *
 * @param[in] item_tag     Union member tag.
 * @param[in] member_ptr   Union member pointer corresponding to the tag.
 * @return Declaration data structure.
 */
template <typename item_tag_t, typename member_ptr_t>
constexpr auto type2member_entry(item_tag_t item_tag, member_ptr_t member_ptr) {
    return std::make_tuple(item_tag, member_ptr);
}

/**
 * Declare sentinel item tag.
 *
 * @param[in] item_tag    Sentinel item tag.
 * @return Declaration data structure.
 */
template <typename item_tag_t>
constexpr auto type2member_entry(item_tag_t item_tag) {
    return std::make_tuple(item_tag);
}

/**
 * Parse sequence of tagged unions.
 *
 * @param[in] begin         Unions sequence begin iterator.
 * @param[in] end           Unions sequence end iterator.
 * @param[in,out] parser    The parser class.
 * @return Parse result.
 */
template <typename iterator_t, typename parser_t>
auto parse_all(iterator_t begin, iterator_t end, parser_t &parser) {
    std::error_code ec;

    for (auto it = begin; it != end; ++it) {
        if (parser.sentinel_parsed())
            return std::make_error_code(std::errc::invalid_argument);

        ec = detail::dispatch_on_item<0>(parser, *it, parser_t::type2member);

        if (ec)
            return ec;
    }

    return parser.on_done();
}

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
