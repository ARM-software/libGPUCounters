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

#pragma once

#include "print.hpp"

#include <debug/ostream_indent.hpp>

#include <cassert>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace kinstr_prfcnt {

inline std::ostream &operator<<(std::ostream &os, const enum_item &value) {
    /* Since we made a mistake of putting enum_item::item_type::block = 0, there
     * is no enum_item::item_type::none value. This heuristic detects a sentinel item
     * and prints it*/
    if (value.hdr.type == enum_item::item_type::block && value.u.block_counter.num_values == 0) {
        return os << "enum_item {\n"                   //
                  << debug::indent_level::push         //
                  << debug::indent << ".hdr = none,\n" //
                  << debug::indent << ".u = {},\n"     //
                  << debug::indent_level::pop          //
                  << debug::indent << "}";             //
    }

    os << "enum_item {\n"                                   //
       << debug::indent_level::push                         //
       << debug::indent << ".hdr = " << value.hdr << ",\n"; //

    bool printed{false};

    switch (value.hdr.type) {
    case enum_item::item_type::block:
        os << debug::indent << ".u.block_counter = " << value.u.block_counter << ",\n";
        printed = true;
        break;
    case enum_item::item_type::request:
        os << debug::indent << ".u.request = " << value.u.request << ",\n";
        printed = true;
        break;
    case enum_item::item_type::sample_info:
        os << debug::indent << ".u.sample_info = " << value.u.sample_info << ",\n";
        printed = true;
        break;
    }

    if (!printed) {
        assert(!&"Unknown enum_item::item_type");
        os << debug::indent << ".u = <unknown>,\n";
    }
    return os << debug::indent_level::pop //
              << debug::indent << "}";
}

inline std::ostream &operator<<(std::ostream &os, const metadata_item &value) {
    os << "metadata_item {\n"                               //
       << debug::indent_level::push                         //
       << debug::indent << ".hdr = " << value.hdr << ",\n"; //

    bool printed{false};

    switch (value.hdr.type) {
    case metadata_item::item_type::none:
        os << debug::indent << ".u = {},\n";
        printed = true;
        break;
    case metadata_item::item_type::block:
        os << debug::indent << ".u.block_md = " << value.u.block_md << ",\n";
        printed = true;
        break;
    case metadata_item::item_type::clock:
        os << debug::indent << ".u.clock_md = " << value.u.clock_md << ",\n";
        printed = true;
        break;
    case metadata_item::item_type::sample:
        os << debug::indent << ".u.sample_md = " << value.u.sample_md << ",\n";
        printed = true;
        break;
    }

    if (!printed) {
        assert(!&"Unknown metadata_item::item_type");
        os << debug::indent << ".u = <unknown>,\n";
    }

    return os << debug::indent_level::pop //
              << debug::indent << "}";
}

inline std::ostream &operator<<(std::ostream &os, const request_item &value) {
    os << "request_item {\n"                                //
       << debug::indent_level::push                         //
       << debug::indent << ".hdr = " << value.hdr << ",\n"; //

    bool printed{false};

    switch (value.hdr.type) {
    case request_item::item_type::none:
        os << debug::indent << ".u = {},\n";
        printed = true;
        break;
    case request_item::item_type::enable:
        os << debug::indent << ".u.req_enable = " << value.u.req_enable << ",\n";
        printed = true;
        break;
    case request_item::item_type::mode:
        os << debug::indent << ".u.req_mode = " << value.u.req_mode << ",\n";
        printed = true;
        break;
    case request_item::item_type::scope:
        os << debug::indent << ".u.req_scope = " << value.u.req_scope << ",\n";
        printed = true;
        break;
    }

    if (!printed) {
        assert(!&"Unknown request_item::item_type");
        os << debug::indent << ".u = <unknown>,\n";
    }

    return os << debug::indent_level::pop //
              << debug::indent << "}";
}

inline std::ostream &operator<<(std::ostream &os, const request_item::request_mode &value) {
    os << "request_mode {\n"                                  //
       << debug::indent_level::push                           //
       << debug::indent << ".mode = " << value.mode << ",\n"; //

    bool printed{false};

    switch (value.mode) {
    case request_item::request_mode::sampling_mode::periodic:
        os << debug::indent << ".mode_config.periodic = " << value.mode_config.periodic << ",\n";
        printed = true;
        break;
    case request_item::request_mode::sampling_mode::manual:
        os << debug::indent << ".mode_config = {},\n";
        printed = true;
        break;
    }

    if (!printed) {
        assert(!&"Unknown request_item::request_mode::sampling_mode");
        os << debug::indent << ".mode_config = <unknown>,\n";
    }

    return os << debug::indent_level::pop //
              << debug::indent << "}";
}
} // namespace kinstr_prfcnt
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
