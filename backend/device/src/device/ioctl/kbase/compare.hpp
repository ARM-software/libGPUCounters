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
namespace kbase {

inline bool operator==(const version_check &lhs, const version_check &rhs);
inline bool operator!=(const version_check &lhs, const version_check &rhs) { return !(lhs == rhs); }
inline bool operator==(const set_flags &lhs, const set_flags &rhs);
inline bool operator!=(const set_flags &lhs, const set_flags &rhs) { return !(lhs == rhs); }
inline bool operator==(const get_gpuprops &lhs, const get_gpuprops &rhs);
inline bool operator!=(const get_gpuprops &lhs, const get_gpuprops &rhs) { return !(lhs == rhs); }
inline bool operator==(const cs_get_glb_iface::in_type &lhs, const cs_get_glb_iface::in_type &rhs);
inline bool operator!=(const cs_get_glb_iface::in_type &lhs, const cs_get_glb_iface::in_type &rhs) { return !(lhs == rhs); }
inline bool operator==(const cs_get_glb_iface::out_type &lhs, const cs_get_glb_iface::out_type &rhs);
inline bool operator!=(const cs_get_glb_iface::out_type &lhs, const cs_get_glb_iface::out_type &rhs) { return !(lhs == rhs); }
inline bool operator==(const hwcnt_reader_setup &lhs, const hwcnt_reader_setup &rhs);
inline bool operator!=(const hwcnt_reader_setup &lhs, const hwcnt_reader_setup &rhs) { return !(lhs == rhs); }
inline bool operator==(const kinstr_prfcnt_enum_info &lhs, const kinstr_prfcnt_enum_info &rhs);
inline bool operator!=(const kinstr_prfcnt_enum_info &lhs, const kinstr_prfcnt_enum_info &rhs) { return !(lhs == rhs); }
inline bool operator==(const kinstr_prfcnt_setup::in_type &lhs, const kinstr_prfcnt_setup::in_type &rhs);
inline bool operator!=(const kinstr_prfcnt_setup::in_type &lhs, const kinstr_prfcnt_setup::in_type &rhs) { return !(lhs == rhs); }
inline bool operator==(const kinstr_prfcnt_setup::out_type &lhs, const kinstr_prfcnt_setup::out_type &rhs);
inline bool operator!=(const kinstr_prfcnt_setup::out_type &lhs, const kinstr_prfcnt_setup::out_type &rhs) { return !(lhs == rhs); }

inline bool operator==(const version_check &lhs, const version_check &rhs) {
    return true //
            && lhs.major == rhs.major //
            && lhs.minor == rhs.minor //
        ;
}

inline bool operator==(const set_flags &lhs, const set_flags &rhs) {
    return true //
            && lhs.create_flags == rhs.create_flags //
        ;
}

inline bool operator==(const get_gpuprops &lhs, const get_gpuprops &rhs) {
    return true //
            && lhs.buffer == rhs.buffer //
            && lhs.size == rhs.size //
            && lhs.flags == rhs.flags //
        ;
}

inline bool operator==(const cs_get_glb_iface::in_type &lhs, const cs_get_glb_iface::in_type &rhs) {
    return true //
            && lhs.max_group_num == rhs.max_group_num //
            && lhs.max_total_stream_num == rhs.max_total_stream_num //
            && lhs.groups_ptr == rhs.groups_ptr //
            && lhs.streams_ptr == rhs.streams_ptr //
        ;
}

inline bool operator==(const cs_get_glb_iface::out_type &lhs, const cs_get_glb_iface::out_type &rhs) {
    return true //
            && lhs.glb_version == rhs.glb_version //
            && lhs.features == rhs.features //
            && lhs.group_num == rhs.group_num //
            && lhs.prfcnt_size == rhs.prfcnt_size //
            && lhs.total_stream_num == rhs.total_stream_num //
            && lhs.instr_features == rhs.instr_features //
        ;
}

inline bool operator==(const hwcnt_reader_setup &lhs, const hwcnt_reader_setup &rhs) {
    return true //
            && lhs.buffer_count == rhs.buffer_count //
            && lhs.fe_bm == rhs.fe_bm //
            && lhs.shader_bm == rhs.shader_bm //
            && lhs.tiler_bm == rhs.tiler_bm //
            && lhs.mmu_l2_bm == rhs.mmu_l2_bm //
        ;
}

inline bool operator==(const kinstr_prfcnt_enum_info &lhs, const kinstr_prfcnt_enum_info &rhs) {
    return true //
            && lhs.info_item_size == rhs.info_item_size //
            && lhs.info_item_count == rhs.info_item_count //
            && lhs.info_list_ptr == rhs.info_list_ptr //
        ;
}

inline bool operator==(const kinstr_prfcnt_setup::in_type &lhs, const kinstr_prfcnt_setup::in_type &rhs) {
    return true //
            && lhs.request_item_count == rhs.request_item_count //
            && lhs.request_item_size == rhs.request_item_size //
            && lhs.requests_ptr == rhs.requests_ptr //
        ;
}

inline bool operator==(const kinstr_prfcnt_setup::out_type &lhs, const kinstr_prfcnt_setup::out_type &rhs) {
    return true //
            && lhs.prfcnt_metadata_item_size == rhs.prfcnt_metadata_item_size //
            && lhs.prfcnt_mmap_size_bytes == rhs.prfcnt_mmap_size_bytes //
        ;
}

} // namespace kbase
} // namespace ioctl
} // namespace device
} // namespace hwcpipe

#include "compare_manual.hpp"

// clang-format on
