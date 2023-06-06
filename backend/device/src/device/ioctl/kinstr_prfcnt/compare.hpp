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

/* Note, this file is generated, do NOT edit! */

#pragma once

// clang-format off

#include "commands.hpp"

#include <algorithm>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace kinstr_prfcnt {

inline bool operator==(const enum_item::header &lhs, const enum_item::header &rhs);
inline bool operator!=(const enum_item::header &lhs, const enum_item::header &rhs) { return !(lhs == rhs); }
inline bool operator==(const enum_item::enum_block_counter &lhs, const enum_item::enum_block_counter &rhs);
inline bool operator!=(const enum_item::enum_block_counter &lhs, const enum_item::enum_block_counter &rhs) { return !(lhs == rhs); }
inline bool operator==(const enum_item::enum_request &lhs, const enum_item::enum_request &rhs);
inline bool operator!=(const enum_item::enum_request &lhs, const enum_item::enum_request &rhs) { return !(lhs == rhs); }
inline bool operator==(const enum_item::enum_sample_info &lhs, const enum_item::enum_sample_info &rhs);
inline bool operator!=(const enum_item::enum_sample_info &lhs, const enum_item::enum_sample_info &rhs) { return !(lhs == rhs); }
inline bool operator==(const enum_item &lhs, const enum_item &rhs);
inline bool operator!=(const enum_item &lhs, const enum_item &rhs) { return !(lhs == rhs); }
inline bool operator==(const metadata_item::header &lhs, const metadata_item::header &rhs);
inline bool operator!=(const metadata_item::header &lhs, const metadata_item::header &rhs) { return !(lhs == rhs); }
inline bool operator==(const metadata_item::block_metadata &lhs, const metadata_item::block_metadata &rhs);
inline bool operator!=(const metadata_item::block_metadata &lhs, const metadata_item::block_metadata &rhs) { return !(lhs == rhs); }
inline bool operator==(const metadata_item::clock_metadata &lhs, const metadata_item::clock_metadata &rhs);
inline bool operator!=(const metadata_item::clock_metadata &lhs, const metadata_item::clock_metadata &rhs) { return !(lhs == rhs); }
inline bool operator==(const metadata_item::sample_metadata &lhs, const metadata_item::sample_metadata &rhs);
inline bool operator!=(const metadata_item::sample_metadata &lhs, const metadata_item::sample_metadata &rhs) { return !(lhs == rhs); }
inline bool operator==(const metadata_item &lhs, const metadata_item &rhs);
inline bool operator!=(const metadata_item &lhs, const metadata_item &rhs) { return !(lhs == rhs); }
inline bool operator==(const control_cmd &lhs, const control_cmd &rhs);
inline bool operator!=(const control_cmd &lhs, const control_cmd &rhs) { return !(lhs == rhs); }
inline bool operator==(const request_item::header &lhs, const request_item::header &rhs);
inline bool operator!=(const request_item::header &lhs, const request_item::header &rhs) { return !(lhs == rhs); }
inline bool operator==(const request_item::request_mode::periodic_type &lhs, const request_item::request_mode::periodic_type &rhs);
inline bool operator!=(const request_item::request_mode::periodic_type &lhs, const request_item::request_mode::periodic_type &rhs) { return !(lhs == rhs); }
inline bool operator==(const request_item::request_mode &lhs, const request_item::request_mode &rhs);
inline bool operator!=(const request_item::request_mode &lhs, const request_item::request_mode &rhs) { return !(lhs == rhs); }
inline bool operator==(const request_item::request_enable &lhs, const request_item::request_enable &rhs);
inline bool operator!=(const request_item::request_enable &lhs, const request_item::request_enable &rhs) { return !(lhs == rhs); }
inline bool operator==(const request_item::request_scope &lhs, const request_item::request_scope &rhs);
inline bool operator!=(const request_item::request_scope &lhs, const request_item::request_scope &rhs) { return !(lhs == rhs); }
inline bool operator==(const request_item &lhs, const request_item &rhs);
inline bool operator!=(const request_item &lhs, const request_item &rhs) { return !(lhs == rhs); }
inline bool operator==(const sample_access &lhs, const sample_access &rhs);
inline bool operator!=(const sample_access &lhs, const sample_access &rhs) { return !(lhs == rhs); }

inline bool operator==(const enum_item::header &lhs, const enum_item::header &rhs) {
    return true //
            && lhs.type == rhs.type //
            && lhs.item_version == rhs.item_version //
        ;
}

inline bool operator==(const enum_item::enum_block_counter &lhs, const enum_item::enum_block_counter &rhs) {
    return true //
            && lhs.type == rhs.type //
            && lhs.set == rhs.set //
            && lhs.num_instances == rhs.num_instances //
            && lhs.num_values == rhs.num_values //
            && std::equal(std::begin(lhs.counter_mask), std::end(lhs.counter_mask), std::begin(rhs.counter_mask)) //
        ;
}

inline bool operator==(const enum_item::enum_request &lhs, const enum_item::enum_request &rhs) {
    return true //
            && lhs.request_item_type == rhs.request_item_type //
            && lhs.versions_mask == rhs.versions_mask //
        ;
}

inline bool operator==(const enum_item::enum_sample_info &lhs, const enum_item::enum_sample_info &rhs) {
    return true //
            && lhs.num_clock_domains == rhs.num_clock_domains //
        ;
}

inline bool operator==(const metadata_item::header &lhs, const metadata_item::header &rhs) {
    return true //
            && lhs.type == rhs.type //
            && lhs.item_version == rhs.item_version //
        ;
}

inline bool operator==(const metadata_item::block_metadata &lhs, const metadata_item::block_metadata &rhs) {
    return true //
            && lhs.type == rhs.type //
            && lhs.block_idx == rhs.block_idx //
            && lhs.set == rhs.set //
            && lhs.block_state == rhs.block_state //
            && lhs.values_offset == rhs.values_offset //
        ;
}

inline bool operator==(const metadata_item::clock_metadata &lhs, const metadata_item::clock_metadata &rhs) {
    return true //
            && lhs.num_domains == rhs.num_domains //
            && std::equal(std::begin(lhs.cycles), std::end(lhs.cycles), std::begin(rhs.cycles)) //
        ;
}

inline bool operator==(const metadata_item::sample_metadata &lhs, const metadata_item::sample_metadata &rhs) {
    return true //
            && lhs.timestamp_start == rhs.timestamp_start //
            && lhs.timestamp_stop == rhs.timestamp_stop //
            && lhs.seq == rhs.seq //
            && lhs.user_data == rhs.user_data //
            && lhs.flags == rhs.flags //
        ;
}

inline bool operator==(const control_cmd &lhs, const control_cmd &rhs) {
    return true //
            && lhs.cmd == rhs.cmd //
            && lhs.user_data == rhs.user_data //
        ;
}

inline bool operator==(const request_item::header &lhs, const request_item::header &rhs) {
    return true //
            && lhs.type == rhs.type //
            && lhs.item_version == rhs.item_version //
        ;
}

inline bool operator==(const request_item::request_mode::periodic_type &lhs, const request_item::request_mode::periodic_type &rhs) {
    return true //
            && lhs.period_ns == rhs.period_ns //
        ;
}

inline bool operator==(const request_item::request_enable &lhs, const request_item::request_enable &rhs) {
    return true //
            && lhs.type == rhs.type //
            && lhs.set == rhs.set //
            && std::equal(std::begin(lhs.enable_mask), std::end(lhs.enable_mask), std::begin(rhs.enable_mask)) //
        ;
}

inline bool operator==(const request_item::request_scope &lhs, const request_item::request_scope &rhs) {
    return true //
            && lhs.scope == rhs.scope //
        ;
}

inline bool operator==(const sample_access &lhs, const sample_access &rhs) {
    return true //
            && lhs.sequence == rhs.sequence //
            && lhs.sample_metadata_ptr == rhs.sample_metadata_ptr //
        ;
}

} // namespace kinstr_prfcnt
} // namespace ioctl
} // namespace device
} // namespace hwcpipe

#include "compare_manual.hpp"

// clang-format on
