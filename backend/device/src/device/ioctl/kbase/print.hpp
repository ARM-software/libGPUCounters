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

#include <debug/ostream_indent.hpp>
#include <debug/print_array.hpp>

#include <cassert>
#include <ostream>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace kbase {

inline std::ostream &operator<<(std::ostream &os, const version_check &value);
inline std::ostream &operator<<(std::ostream &os, const set_flags &value);
inline std::ostream &operator<<(std::ostream &os, get_gpuprops::gpuprop_size value);
inline std::ostream &operator<<(std::ostream &os, get_gpuprops::gpuprop_code value);
inline std::ostream &operator<<(std::ostream &os, const get_gpuprops &value);
inline std::ostream &operator<<(std::ostream &os, const cs_get_glb_iface::in_type &value);
inline std::ostream &operator<<(std::ostream &os, const cs_get_glb_iface::out_type &value);
// no `operator<<` for union cs_get_glb_iface
inline std::ostream &operator<<(std::ostream &os, const hwcnt_reader_setup &value);
inline std::ostream &operator<<(std::ostream &os, const kinstr_prfcnt_enum_info &value);
inline std::ostream &operator<<(std::ostream &os, const kinstr_prfcnt_setup::in_type &value);
inline std::ostream &operator<<(std::ostream &os, const kinstr_prfcnt_setup::out_type &value);
// no `operator<<` for union kinstr_prfcnt_setup

inline std::ostream &operator<<(std::ostream &os, const version_check &value) {
    return os << "version_check {\n" //
              << debug::indent_level::push //
              << debug::indent << ".major = " << value.major << ",\n" //
              << debug::indent << ".minor = " << value.minor << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const set_flags &value) {
    return os << "set_flags {\n" //
              << debug::indent_level::push //
              << debug::indent << ".create_flags = " << value.create_flags << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, get_gpuprops::gpuprop_size value) {
    switch (value) {
    case get_gpuprops::gpuprop_size::uint8:
        return os << "uint8";
    case get_gpuprops::gpuprop_size::uint16:
        return os << "uint16";
    case get_gpuprops::gpuprop_size::uint32:
        return os << "uint32";
    case get_gpuprops::gpuprop_size::uint64:
        return os << "uint64";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, get_gpuprops::gpuprop_code value) {
    switch (value) {
    case get_gpuprops::gpuprop_code::product_id:
        return os << "product_id";
    case get_gpuprops::gpuprop_code::version_status:
        return os << "version_status";
    case get_gpuprops::gpuprop_code::minor_revision:
        return os << "minor_revision";
    case get_gpuprops::gpuprop_code::major_revision:
        return os << "major_revision";
    case get_gpuprops::gpuprop_code::gpu_freq_khz_max:
        return os << "gpu_freq_khz_max";
    case get_gpuprops::gpuprop_code::log2_program_counter_size:
        return os << "log2_program_counter_size";
    case get_gpuprops::gpuprop_code::texture_features_0:
        return os << "texture_features_0";
    case get_gpuprops::gpuprop_code::texture_features_1:
        return os << "texture_features_1";
    case get_gpuprops::gpuprop_code::texture_features_2:
        return os << "texture_features_2";
    case get_gpuprops::gpuprop_code::gpu_available_memory_size:
        return os << "gpu_available_memory_size";
    case get_gpuprops::gpuprop_code::l2_log2_line_size:
        return os << "l2_log2_line_size";
    case get_gpuprops::gpuprop_code::l2_log2_cache_size:
        return os << "l2_log2_cache_size";
    case get_gpuprops::gpuprop_code::l2_num_l2_slices:
        return os << "l2_num_l2_slices";
    case get_gpuprops::gpuprop_code::tiler_bin_size_bytes:
        return os << "tiler_bin_size_bytes";
    case get_gpuprops::gpuprop_code::tiler_max_active_levels:
        return os << "tiler_max_active_levels";
    case get_gpuprops::gpuprop_code::max_threads:
        return os << "max_threads";
    case get_gpuprops::gpuprop_code::max_workgroup_size:
        return os << "max_workgroup_size";
    case get_gpuprops::gpuprop_code::max_barrier_size:
        return os << "max_barrier_size";
    case get_gpuprops::gpuprop_code::max_registers:
        return os << "max_registers";
    case get_gpuprops::gpuprop_code::max_task_queue:
        return os << "max_task_queue";
    case get_gpuprops::gpuprop_code::max_thread_group_split:
        return os << "max_thread_group_split";
    case get_gpuprops::gpuprop_code::impl_tech:
        return os << "impl_tech";
    case get_gpuprops::gpuprop_code::raw_shader_present:
        return os << "raw_shader_present";
    case get_gpuprops::gpuprop_code::raw_tiler_present:
        return os << "raw_tiler_present";
    case get_gpuprops::gpuprop_code::raw_l2_present:
        return os << "raw_l2_present";
    case get_gpuprops::gpuprop_code::raw_stack_present:
        return os << "raw_stack_present";
    case get_gpuprops::gpuprop_code::raw_l2_features:
        return os << "raw_l2_features";
    case get_gpuprops::gpuprop_code::raw_core_features:
        return os << "raw_core_features";
    case get_gpuprops::gpuprop_code::raw_mem_features:
        return os << "raw_mem_features";
    case get_gpuprops::gpuprop_code::raw_mmu_features:
        return os << "raw_mmu_features";
    case get_gpuprops::gpuprop_code::raw_as_present:
        return os << "raw_as_present";
    case get_gpuprops::gpuprop_code::raw_js_present:
        return os << "raw_js_present";
    case get_gpuprops::gpuprop_code::raw_js_features_0:
        return os << "raw_js_features_0";
    case get_gpuprops::gpuprop_code::raw_js_features_1:
        return os << "raw_js_features_1";
    case get_gpuprops::gpuprop_code::raw_js_features_2:
        return os << "raw_js_features_2";
    case get_gpuprops::gpuprop_code::raw_js_features_3:
        return os << "raw_js_features_3";
    case get_gpuprops::gpuprop_code::raw_js_features_4:
        return os << "raw_js_features_4";
    case get_gpuprops::gpuprop_code::raw_js_features_5:
        return os << "raw_js_features_5";
    case get_gpuprops::gpuprop_code::raw_js_features_6:
        return os << "raw_js_features_6";
    case get_gpuprops::gpuprop_code::raw_js_features_7:
        return os << "raw_js_features_7";
    case get_gpuprops::gpuprop_code::raw_js_features_8:
        return os << "raw_js_features_8";
    case get_gpuprops::gpuprop_code::raw_js_features_9:
        return os << "raw_js_features_9";
    case get_gpuprops::gpuprop_code::raw_js_features_10:
        return os << "raw_js_features_10";
    case get_gpuprops::gpuprop_code::raw_js_features_11:
        return os << "raw_js_features_11";
    case get_gpuprops::gpuprop_code::raw_js_features_12:
        return os << "raw_js_features_12";
    case get_gpuprops::gpuprop_code::raw_js_features_13:
        return os << "raw_js_features_13";
    case get_gpuprops::gpuprop_code::raw_js_features_14:
        return os << "raw_js_features_14";
    case get_gpuprops::gpuprop_code::raw_js_features_15:
        return os << "raw_js_features_15";
    case get_gpuprops::gpuprop_code::raw_tiler_features:
        return os << "raw_tiler_features";
    case get_gpuprops::gpuprop_code::raw_texture_features_0:
        return os << "raw_texture_features_0";
    case get_gpuprops::gpuprop_code::raw_texture_features_1:
        return os << "raw_texture_features_1";
    case get_gpuprops::gpuprop_code::raw_texture_features_2:
        return os << "raw_texture_features_2";
    case get_gpuprops::gpuprop_code::raw_gpu_id:
        return os << "raw_gpu_id";
    case get_gpuprops::gpuprop_code::raw_thread_max_threads:
        return os << "raw_thread_max_threads";
    case get_gpuprops::gpuprop_code::raw_thread_max_workgroup_size:
        return os << "raw_thread_max_workgroup_size";
    case get_gpuprops::gpuprop_code::raw_thread_max_barrier_size:
        return os << "raw_thread_max_barrier_size";
    case get_gpuprops::gpuprop_code::raw_thread_features:
        return os << "raw_thread_features";
    case get_gpuprops::gpuprop_code::raw_coherency_mode:
        return os << "raw_coherency_mode";
    case get_gpuprops::gpuprop_code::coherency_num_groups:
        return os << "coherency_num_groups";
    case get_gpuprops::gpuprop_code::coherency_num_core_groups:
        return os << "coherency_num_core_groups";
    case get_gpuprops::gpuprop_code::coherency_coherency:
        return os << "coherency_coherency";
    case get_gpuprops::gpuprop_code::coherency_group_0:
        return os << "coherency_group_0";
    case get_gpuprops::gpuprop_code::coherency_group_1:
        return os << "coherency_group_1";
    case get_gpuprops::gpuprop_code::coherency_group_2:
        return os << "coherency_group_2";
    case get_gpuprops::gpuprop_code::coherency_group_3:
        return os << "coherency_group_3";
    case get_gpuprops::gpuprop_code::coherency_group_4:
        return os << "coherency_group_4";
    case get_gpuprops::gpuprop_code::coherency_group_5:
        return os << "coherency_group_5";
    case get_gpuprops::gpuprop_code::coherency_group_6:
        return os << "coherency_group_6";
    case get_gpuprops::gpuprop_code::coherency_group_7:
        return os << "coherency_group_7";
    case get_gpuprops::gpuprop_code::coherency_group_8:
        return os << "coherency_group_8";
    case get_gpuprops::gpuprop_code::coherency_group_9:
        return os << "coherency_group_9";
    case get_gpuprops::gpuprop_code::coherency_group_10:
        return os << "coherency_group_10";
    case get_gpuprops::gpuprop_code::coherency_group_11:
        return os << "coherency_group_11";
    case get_gpuprops::gpuprop_code::coherency_group_12:
        return os << "coherency_group_12";
    case get_gpuprops::gpuprop_code::coherency_group_13:
        return os << "coherency_group_13";
    case get_gpuprops::gpuprop_code::coherency_group_14:
        return os << "coherency_group_14";
    case get_gpuprops::gpuprop_code::coherency_group_15:
        return os << "coherency_group_15";
    case get_gpuprops::gpuprop_code::texture_features_3:
        return os << "texture_features_3";
    case get_gpuprops::gpuprop_code::raw_texture_features_3:
        return os << "raw_texture_features_3";
    case get_gpuprops::gpuprop_code::num_exec_engines:
        return os << "num_exec_engines";
    case get_gpuprops::gpuprop_code::raw_thread_tls_alloc:
        return os << "raw_thread_tls_alloc";
    case get_gpuprops::gpuprop_code::tls_alloc:
        return os << "tls_alloc";
    case get_gpuprops::gpuprop_code::raw_gpu_features:
        return os << "raw_gpu_features";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, const get_gpuprops &value) {
    return os << "get_gpuprops {\n" //
              << debug::indent_level::push //
              << debug::indent << ".buffer = " << value.buffer << ",\n" //
              << debug::indent << ".size = " << value.size << ",\n" //
              << debug::indent << ".flags = " << value.flags << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const cs_get_glb_iface::in_type &value) {
    return os << "in_type {\n" //
              << debug::indent_level::push //
              << debug::indent << ".max_group_num = " << value.max_group_num << ",\n" //
              << debug::indent << ".max_total_stream_num = " << value.max_total_stream_num << ",\n" //
              << debug::indent << ".groups_ptr = " << value.groups_ptr << ",\n" //
              << debug::indent << ".streams_ptr = " << value.streams_ptr << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const cs_get_glb_iface::out_type &value) {
    return os << "out_type {\n" //
              << debug::indent_level::push //
              << debug::indent << ".glb_version = " << value.glb_version << ",\n" //
              << debug::indent << ".features = " << value.features << ",\n" //
              << debug::indent << ".group_num = " << value.group_num << ",\n" //
              << debug::indent << ".prfcnt_size = " << value.prfcnt_size << ",\n" //
              << debug::indent << ".total_stream_num = " << value.total_stream_num << ",\n" //
              << debug::indent << ".instr_features = " << value.instr_features << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

// no `operator<<` for union cs_get_glb_iface

inline std::ostream &operator<<(std::ostream &os, const hwcnt_reader_setup &value) {
    return os << "hwcnt_reader_setup {\n" //
              << debug::indent_level::push //
              << debug::indent << ".buffer_count = " << value.buffer_count << ",\n" //
              << debug::indent << ".fe_bm = " << value.fe_bm << ",\n" //
              << debug::indent << ".shader_bm = " << value.shader_bm << ",\n" //
              << debug::indent << ".tiler_bm = " << value.tiler_bm << ",\n" //
              << debug::indent << ".mmu_l2_bm = " << value.mmu_l2_bm << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const kinstr_prfcnt_enum_info &value) {
    return os << "kinstr_prfcnt_enum_info {\n" //
              << debug::indent_level::push //
              << debug::indent << ".info_item_size = " << value.info_item_size << ",\n" //
              << debug::indent << ".info_item_count = " << value.info_item_count << ",\n" //
              << debug::indent << ".info_list_ptr = " << value.info_list_ptr << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const kinstr_prfcnt_setup::in_type &value) {
    return os << "in_type {\n" //
              << debug::indent_level::push //
              << debug::indent << ".request_item_count = " << value.request_item_count << ",\n" //
              << debug::indent << ".request_item_size = " << value.request_item_size << ",\n" //
              << debug::indent << ".requests_ptr = " << value.requests_ptr << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const kinstr_prfcnt_setup::out_type &value) {
    return os << "out_type {\n" //
              << debug::indent_level::push //
              << debug::indent << ".prfcnt_metadata_item_size = " << value.prfcnt_metadata_item_size << ",\n" //
              << debug::indent << ".prfcnt_mmap_size_bytes = " << value.prfcnt_mmap_size_bytes << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

// no `operator<<` for union kinstr_prfcnt_setup

inline std::ostream &operator<<(std::ostream &os, command::command_type cmd) {
    switch (cmd) {
    case command::version_check_jm:
        return os << "kbase::command::version_check_jm";
    case command::version_check_csf:
        return os << "kbase::command::version_check_csf";
    case command::set_flags:
        return os << "kbase::command::set_flags";
    case command::get_gpuprops:
        return os << "kbase::command::get_gpuprops";
    case command::cs_get_glb_iface:
        return os << "kbase::command::cs_get_glb_iface";
    case command::hwcnt_reader_setup:
        return os << "kbase::command::hwcnt_reader_setup";
    case command::kinstr_prfcnt_enum_info:
        return os << "kbase::command::kinstr_prfcnt_enum_info";
    case command::kinstr_prfcnt_setup:
        return os << "kbase::command::kinstr_prfcnt_setup";
    }

    assert(!&"Unknown command");
    return os << "::command::<unknown>";
}

} // namespace kbase
} // namespace ioctl
} // namespace device
} // namespace hwcpipe

#include "print_manual.hpp"

// clang-format on
