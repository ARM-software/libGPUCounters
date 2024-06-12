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

#include "compare.hpp"

#include <cassert>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace kinstr_prfcnt {

bool operator==(const enum_item &lhs, const enum_item &rhs) {
    if (lhs.hdr != rhs.hdr)
        return false;

    switch (lhs.hdr.type) {
    case enum_item::item_type::block:
        return lhs.u.block_counter == rhs.u.block_counter;
    case enum_item::item_type::request:
        return lhs.u.request == rhs.u.request;
    case enum_item::item_type::sample_info:
        return lhs.u.sample_info == rhs.u.sample_info;
    }

    assert(!&"Unknown enum_item::item_type");
    return false;
}

bool operator==(const metadata_item &lhs, const metadata_item &rhs) {
    if (lhs.hdr != rhs.hdr)
        return false;

    switch (lhs.hdr.type) {
    case metadata_item::item_type::none:
        return true;
    case metadata_item::item_type::block:
        return lhs.u.block_md == rhs.u.block_md;
    case metadata_item::item_type::clock:
        return lhs.u.clock_md == rhs.u.clock_md;
    case metadata_item::item_type::sample:
        return lhs.u.sample_md == rhs.u.sample_md;
    }

    assert(!&"Unknown metadata_item::item_type");
    return false;
}

bool operator==(const request_item &lhs, const request_item &rhs) {
    if (lhs.hdr != rhs.hdr)
        return false;

    switch (lhs.hdr.type) {
    case request_item::item_type::none:
        return true;
    case request_item::item_type::enable:
        return lhs.u.req_enable == rhs.u.req_enable;
    case request_item::item_type::mode:
        return lhs.u.req_mode == rhs.u.req_mode;
    case request_item::item_type::scope:
        return lhs.u.req_scope == rhs.u.req_scope;
    }

    assert(!&"Unknown request_item::item_type");
    return false;
}

bool operator==(const request_item::request_mode &lhs, const request_item::request_mode &rhs) {
    if (lhs.mode != rhs.mode)
        return false;

    switch (lhs.mode) {
    case request_item::request_mode::sampling_mode::manual:
        return true;
    case request_item::request_mode::sampling_mode::periodic:
        return lhs.mode_config.periodic == rhs.mode_config.periodic;
    }

    assert(!&"Unknown request_item::request_mode::sampling_mode");
    return false;
}

} // namespace kinstr_prfcnt
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
