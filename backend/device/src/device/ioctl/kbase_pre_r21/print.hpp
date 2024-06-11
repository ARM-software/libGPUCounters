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
namespace kbase_pre_r21 {

inline std::ostream &operator<<(std::ostream &os, header_id value);
// no `operator<<` for union uk_header
inline std::ostream &operator<<(std::ostream &os, const version_check_args &value);
inline std::ostream &operator<<(std::ostream &os, const set_flags_args &value);
inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::core &value);
inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::l2_cache &value);
inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::tiler &value);
inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::thread &value);
inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::raw &value);
inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::coherent_group_info::coherent_group &value);
inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::coherent_group_info &value);
inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props &value);
inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops &value);
inline std::ostream &operator<<(std::ostream &os, const uk_hwcnt_reader_setup &value);

inline std::ostream &operator<<(std::ostream &os, header_id value) {
    switch (value) {
    case header_id::version_check:
        return os << "version_check";
    case header_id::create_kernel_flags:
        return os << "create_kernel_flags";
    case header_id::uk_func_id:
        return os << "uk_func_id";
    case header_id::hwcnt_reader_setup:
        return os << "hwcnt_reader_setup";
    case header_id::dump:
        return os << "dump";
    case header_id::clear:
        return os << "clear";
    case header_id::get_props:
        return os << "get_props";
    case header_id::set_flags:
        return os << "set_flags";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

// no `operator<<` for union uk_header

inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::core &value) {
    return os << "core {\n" //
              << debug::indent_level::push //
              << debug::indent << ".product_id = " << value.product_id << ",\n" //
              << debug::indent << ".version_status = " << value.version_status << ",\n" //
              << debug::indent << ".minor_revision = " << value.minor_revision << ",\n" //
              << debug::indent << ".major_revision = " << value.major_revision << ",\n" //
              << debug::indent << ".gpu_speed_mhz = " << value.gpu_speed_mhz << ",\n" //
              << debug::indent << ".gpu_freq_khz_max = " << value.gpu_freq_khz_max << ",\n" //
              << debug::indent << ".gpu_freq_khz_min = " << value.gpu_freq_khz_min << ",\n" //
              << debug::indent << ".log2_program_counter_size = " << value.log2_program_counter_size << ",\n" //
              << debug::indent << ".texture_features = " << debug::print_array(value.texture_features) << ",\n" //
              << debug::indent << ".gpu_available_memory_size = " << value.gpu_available_memory_size << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::l2_cache &value) {
    return os << "l2_cache {\n" //
              << debug::indent_level::push //
              << debug::indent << ".log2_line_size = " << static_cast<uint32_t>(value.log2_line_size) << ",\n" //
              << debug::indent << ".log2_cache_size = " << static_cast<uint32_t>(value.log2_cache_size) << ",\n" //
              << debug::indent << ".num_l2_slices = " << static_cast<uint32_t>(value.num_l2_slices) << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::tiler &value) {
    return os << "tiler {\n" //
              << debug::indent_level::push //
              << debug::indent << ".bin_size_bytes = " << value.bin_size_bytes << ",\n" //
              << debug::indent << ".max_active_levels = " << value.max_active_levels << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::thread &value) {
    return os << "thread {\n" //
              << debug::indent_level::push //
              << debug::indent << ".max_threads = " << value.max_threads << ",\n" //
              << debug::indent << ".max_workgroup_size = " << value.max_workgroup_size << ",\n" //
              << debug::indent << ".max_barrier_size = " << value.max_barrier_size << ",\n" //
              << debug::indent << ".max_registers = " << value.max_registers << ",\n" //
              << debug::indent << ".max_task_queue = " << static_cast<uint32_t>(value.max_task_queue) << ",\n" //
              << debug::indent << ".max_thread_group_split = " << static_cast<uint32_t>(value.max_thread_group_split) << ",\n" //
              << debug::indent << ".impl_tech = " << static_cast<uint32_t>(value.impl_tech) << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::raw &value) {
    return os << "raw {\n" //
              << debug::indent_level::push //
              << debug::indent << ".shader_present = " << value.shader_present << ",\n" //
              << debug::indent << ".tiler_present = " << value.tiler_present << ",\n" //
              << debug::indent << ".l2_present = " << value.l2_present << ",\n" //
              << debug::indent << ".unused_1 = " << value.unused_1 << ",\n" //
              << debug::indent << ".l2_features = " << value.l2_features << ",\n" //
              << debug::indent << ".suspend_size = " << value.suspend_size << ",\n" //
              << debug::indent << ".mem_features = " << value.mem_features << ",\n" //
              << debug::indent << ".mmu_features = " << value.mmu_features << ",\n" //
              << debug::indent << ".as_present = " << value.as_present << ",\n" //
              << debug::indent << ".js_present = " << value.js_present << ",\n" //
              << debug::indent << ".js_features = " << debug::print_array(value.js_features) << ",\n" //
              << debug::indent << ".tiler_features = " << value.tiler_features << ",\n" //
              << debug::indent << ".texture_features = " << debug::print_array(value.texture_features) << ",\n" //
              << debug::indent << ".gpu_id = " << value.gpu_id << ",\n" //
              << debug::indent << ".thread_max_threads = " << value.thread_max_threads << ",\n" //
              << debug::indent << ".thread_max_workgroup_size = " << value.thread_max_workgroup_size << ",\n" //
              << debug::indent << ".thread_max_barrier_size = " << value.thread_max_barrier_size << ",\n" //
              << debug::indent << ".thread_features = " << value.thread_features << ",\n" //
              << debug::indent << ".coherency_mode = " << value.coherency_mode << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::coherent_group_info::coherent_group &value) {
    return os << "coherent_group {\n" //
              << debug::indent_level::push //
              << debug::indent << ".core_mask = " << value.core_mask << ",\n" //
              << debug::indent << ".num_cores = " << value.num_cores << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props::coherent_group_info &value) {
    return os << "coherent_group_info {\n" //
              << debug::indent_level::push //
              << debug::indent << ".num_groups = " << value.num_groups << ",\n" //
              << debug::indent << ".num_core_groups = " << value.num_core_groups << ",\n" //
              << debug::indent << ".coherency = " << value.coherency << ",\n" //
              << debug::indent << ".group = " << debug::print_array(value.group) << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops::gpu_props &value) {
    return os << "gpu_props {\n" //
              << debug::indent_level::push //
              << debug::indent << ".core_props = " << value.core_props << ",\n" //
              << debug::indent << ".l2_props = " << value.l2_props << ",\n" //
              << debug::indent << ".unused = " << value.unused << ",\n" //
              << debug::indent << ".tiler_props = " << value.tiler_props << ",\n" //
              << debug::indent << ".thread_props = " << value.thread_props << ",\n" //
              << debug::indent << ".raw_props = " << value.raw_props << ",\n" //
              << debug::indent << ".coherency_info = " << value.coherency_info << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, command::command_type cmd) {
    switch (cmd) {
    case command::version_check:
        return os << "kbase_pre_r21::command::version_check";
    case command::set_flags:
        return os << "kbase_pre_r21::command::set_flags";
    case command::get_gpuprops:
        return os << "kbase_pre_r21::command::get_gpuprops";
    case command::hwcnt_reader_setup:
        return os << "kbase_pre_r21::command::hwcnt_reader_setup";
    }

    assert(!&"Unknown command");
    return os << "::command::<unknown>";
}

} // namespace kbase_pre_r21
} // namespace ioctl
} // namespace device
} // namespace hwcpipe

#include "print_manual.hpp"

// clang-format on
