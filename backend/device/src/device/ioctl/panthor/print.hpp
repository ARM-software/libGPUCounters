/*
 * Copyright (c) 2024-2026 Arm Limited.
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

#include <device/logging/ostream_indent.hpp>
#include <device/logging/print_array.hpp>

#include <cassert>
#include <ostream>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace panthor {

inline std::ostream &operator<<(std::ostream &os, const drm_gem_close &value);
inline std::ostream &operator<<(std::ostream &os, dev_query_type value);
inline std::ostream &operator<<(std::ostream &os, const gpu_info &value);
inline std::ostream &operator<<(std::ostream &os, const csif_info &value);
inline std::ostream &operator<<(std::ostream &os, perf_clock value);
inline std::ostream &operator<<(std::ostream &os, perf_counter_set value);
inline std::ostream &operator<<(std::ostream &os, perf_info::perf_feat_flags value);
inline std::ostream &operator<<(std::ostream &os, const perf_info &value);
// no `operator<<` for union dev_query_ptr
inline std::ostream &operator<<(std::ostream &os, const dev_query &value);
inline std::ostream &operator<<(std::ostream &os, const vm_create &value);
inline std::ostream &operator<<(std::ostream &os, const vm_destroy &value);
inline std::ostream &operator<<(std::ostream &os, const bo_create &value);
inline std::ostream &operator<<(std::ostream &os, const bo_mmap_offset &value);
inline std::ostream &operator<<(std::ostream &os, perf_block_type value);
inline std::ostream &operator<<(std::ostream &os, perf_command value);
inline std::ostream &operator<<(std::ostream &os, const perf_ringbuf_control &value);
inline std::ostream &operator<<(std::ostream &os, const perf_cmd_setup &value);
inline std::ostream &operator<<(std::ostream &os, const perf_cmd_start &value);
inline std::ostream &operator<<(std::ostream &os, const perf_cmd_stop &value);
inline std::ostream &operator<<(std::ostream &os, const perf_cmd_sample &value);
// no `operator<<` for union perf_control_ptr
inline std::ostream &operator<<(std::ostream &os, const perf_control &value);
inline std::ostream &operator<<(std::ostream &os, sample_header::sample_flags value);
inline std::ostream &operator<<(std::ostream &os, const sample_header &value);
inline std::ostream &operator<<(std::ostream &os, block_header::block_state value);
inline std::ostream &operator<<(std::ostream &os, const block_header &value);

inline std::ostream &operator<<(std::ostream &os, const drm_gem_close &value) {
    return os << "drm_gem_close {\n" //
              << debug::indent_level::push //
              << debug::indent << ".handle = " << value.handle << ",\n" //
              << debug::indent << ".pad = " << value.pad << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, dev_query_type value) {
    switch (value) {
    case dev_query_type::gpu_info:
        return os << "gpu_info";
    case dev_query_type::csif_info:
        return os << "csif_info";
    case dev_query_type::timestamp_info:
        return os << "timestamp_info";
    case dev_query_type::group_priorities_info:
        return os << "group_priorities_info";
    case dev_query_type::perf_info:
        return os << "perf_info";
    case dev_query_type::instr_info:
        return os << "instr_info";
    }

    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, const gpu_info &value) {
    return os << "gpu_info {\n" //
              << debug::indent_level::push //
              << debug::indent << ".gpu_id = " << value.gpu_id << ",\n" //
              << debug::indent << ".gpu_rev = " << value.gpu_rev << ",\n" //
              << debug::indent << ".csf_id = " << value.csf_id << ",\n" //
              << debug::indent << ".l2_features = " << value.l2_features << ",\n" //
              << debug::indent << ".tiler_features = " << value.tiler_features << ",\n" //
              << debug::indent << ".mem_features = " << value.mem_features << ",\n" //
              << debug::indent << ".mmu_features = " << value.mmu_features << ",\n" //
              << debug::indent << ".thread_features = " << value.thread_features << ",\n" //
              << debug::indent << ".max_threads = " << value.max_threads << ",\n" //
              << debug::indent << ".thread_max_workgroup_size = " << value.thread_max_workgroup_size << ",\n" //
              << debug::indent << ".thread_max_barrier_size = " << value.thread_max_barrier_size << ",\n" //
              << debug::indent << ".coherency_features = " << value.coherency_features << ",\n" //
              << debug::indent << ".texture_features = " << debug::print_array(value.texture_features) << ",\n" //
              << debug::indent << ".as_present = " << value.as_present << ",\n" //
              << debug::indent << ".pad0 = " << value.pad0 << ",\n" //
              << debug::indent << ".shader_present = " << value.shader_present << ",\n" //
              << debug::indent << ".l2_present = " << value.l2_present << ",\n" //
              << debug::indent << ".tiler_present = " << value.tiler_present << ",\n" //
              << debug::indent << ".core_features = " << value.core_features << ",\n" //
              << debug::indent << ".pad = " << value.pad << ",\n" //
              << debug::indent << ".gpu_features = " << value.gpu_features << ",\n" //
              << debug::indent << ".pad1 = " << debug::print_array(value.pad1) << ",\n" //
              << debug::indent << ".gpu_id_hi = " << value.gpu_id_hi << ",\n" //
              << debug::indent << ".gpu_rev_hi = " << value.gpu_rev_hi << ",\n" //
              << debug::indent << ".l2_features_hi = " << value.l2_features_hi << ",\n" //
              << debug::indent << ".neural_present = " << value.neural_present << ",\n" //
              << debug::indent << ".base_present = " << value.base_present << ",\n" //
              << debug::indent << ".thread_num_active_granularity = " << value.thread_num_active_granularity << ",\n" //
              << debug::indent << ".coherency_enable = " << value.coherency_enable << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const csif_info &value) {
    return os << "csif_info {\n" //
              << debug::indent_level::push //
              << debug::indent << ".csg_slot_count = " << value.csg_slot_count << ",\n" //
              << debug::indent << ".cs_slot_count = " << value.cs_slot_count << ",\n" //
              << debug::indent << ".cs_reg_count = " << value.cs_reg_count << ",\n" //
              << debug::indent << ".scoreboard_slot_count = " << value.scoreboard_slot_count << ",\n" //
              << debug::indent << ".unpreserved_cs_reg_count = " << value.unpreserved_cs_reg_count << ",\n" //
              << debug::indent << ".pad = " << debug::print_array(value.pad) << ",\n" //
              << debug::indent << ".version = " << value.version << ",\n" //
              << debug::indent << ".features = " << value.features << ",\n" //
              << debug::indent << ".pad1 = " << value.pad1 << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, perf_clock value) {
    switch (value) {
    case perf_clock::toplevel:
        return os << "toplevel";
    case perf_clock::coregroup:
        return os << "coregroup";
    case perf_clock::shader:
        return os << "shader";
    case perf_clock::neural:
        return os << "neural";
    }

    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, perf_counter_set value) {
    switch (value) {
    case perf_counter_set::primary:
        return os << "primary";
    case perf_counter_set::secondary:
        return os << "secondary";
    case perf_counter_set::tertiary:
        return os << "tertiary";
    }

    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, perf_info::perf_feat_flags value) {
    if (!value)
        return os << 0;

    if (!!(value & perf_info::perf_feat_flags::block_states_support)) {
        value ^= perf_info::perf_feat_flags::block_states_support;
        os << "block_states_support";
    }

    if (!!value)
        os << " | ";

    if (!!value) {
        assert(!&"Unknown bits");
        os << std::hex << static_cast<uint32_t>(value);
    }

    return os;
}

inline std::ostream &operator<<(std::ostream &os, const perf_info &value) {
    return os << "perf_info {\n" //
              << debug::indent_level::push //
              << debug::indent << ".counters_per_block = " << value.counters_per_block << ",\n" //
              << debug::indent << ".sample_header_size = " << value.sample_header_size << ",\n" //
              << debug::indent << ".block_header_size = " << value.block_header_size << ",\n" //
              << debug::indent << ".sample_size = " << value.sample_size << ",\n" //
              << debug::indent << ".flags = " << value.flags << ",\n" //
              << debug::indent << ".supported_clocks = " << value.supported_clocks << ",\n" //
              << debug::indent << ".fw_blocks = " << value.fw_blocks << ",\n" //
              << debug::indent << ".cshw_blocks = " << value.cshw_blocks << ",\n" //
              << debug::indent << ".geometry_blocks = " << value.geometry_blocks << ",\n" //
              << debug::indent << ".memsys_blocks = " << value.memsys_blocks << ",\n" //
              << debug::indent << ".shader_blocks = " << value.shader_blocks << ",\n" //
              << debug::indent << ".neural_blocks = " << value.neural_blocks << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

// no `operator<<` for union dev_query_ptr

inline std::ostream &operator<<(std::ostream &os, const vm_create &value) {
    return os << "vm_create {\n" //
              << debug::indent_level::push //
              << debug::indent << ".flags = " << value.flags << ",\n" //
              << debug::indent << ".id = " << value.id << ",\n" //
              << debug::indent << ".user_va_range = " << value.user_va_range << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const vm_destroy &value) {
    return os << "vm_destroy {\n" //
              << debug::indent_level::push //
              << debug::indent << ".id = " << value.id << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const bo_create &value) {
    return os << "bo_create {\n" //
              << debug::indent_level::push //
              << debug::indent << ".size = " << value.size << ",\n" //
              << debug::indent << ".flags = " << value.flags << ",\n" //
              << debug::indent << ".exclusive_vm_id = " << value.exclusive_vm_id << ",\n" //
              << debug::indent << ".handle = " << value.handle << ",\n" //
              << debug::indent << ".pad = " << debug::print_array(value.pad) << ",\n" //
              << debug::indent << ".alloc_on_fault_granularity = " << value.alloc_on_fault_granularity << ",\n" //
              << debug::indent << ".sync_offset = " << value.sync_offset << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const bo_mmap_offset &value) {
    return os << "bo_mmap_offset {\n" //
              << debug::indent_level::push //
              << debug::indent << ".handle = " << value.handle << ",\n" //
              << debug::indent << ".pad = " << value.pad << ",\n" //
              << debug::indent << ".offset = " << value.offset << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, perf_block_type value) {
    switch (value) {
    case perf_block_type::fw_block:
        return os << "fw_block";
    case perf_block_type::cshw_block:
        return os << "cshw_block";
    case perf_block_type::geometry_block:
        return os << "geometry_block";
    case perf_block_type::memsys_block:
        return os << "memsys_block";
    case perf_block_type::shader_block:
        return os << "shader_block";
    case perf_block_type::neural_block:
        return os << "neural_block";
    }

    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, perf_command value) {
    switch (value) {
    case perf_command::setup:
        return os << "setup";
    case perf_command::teardown:
        return os << "teardown";
    case perf_command::start:
        return os << "start";
    case perf_command::stop:
        return os << "stop";
    case perf_command::sample:
        return os << "sample";
    }

    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, const perf_ringbuf_control &value) {
    return os << "perf_ringbuf_control {\n" //
              << debug::indent_level::push //
              << debug::indent << ".extract_idx = " << value.extract_idx << ",\n" //
              << debug::indent << ".insert_idx = " << value.insert_idx << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const perf_cmd_setup &value) {
    return os << "perf_cmd_setup {\n" //
              << debug::indent_level::push //
              << debug::indent << ".block_set = " << static_cast<uint32_t>(value.block_set) << ",\n" //
              << debug::indent << ".fd = " << value.fd << ",\n" //
              << debug::indent << ".ringbuf_handle = " << value.ringbuf_handle << ",\n" //
              << debug::indent << ".control_handle = " << value.control_handle << ",\n" //
              << debug::indent << ".sample_slots = " << value.sample_slots << ",\n" //
              << debug::indent << ".control_offset = " << value.control_offset << ",\n" //
              << debug::indent << ".sample_freq_ns = " << value.sample_freq_ns << ",\n" //
              << debug::indent << ".fw_enable_mask = " << debug::print_array(value.fw_enable_mask) << ",\n" //
              << debug::indent << ".cshw_enable_mask = " << debug::print_array(value.cshw_enable_mask) << ",\n" //
              << debug::indent << ".geometry_enable_mask = " << debug::print_array(value.geometry_enable_mask) << ",\n" //
              << debug::indent << ".memsys_enable_mask = " << debug::print_array(value.memsys_enable_mask) << ",\n" //
              << debug::indent << ".shader_enable_mask = " << debug::print_array(value.shader_enable_mask) << ",\n" //
              << debug::indent << ".neural_enable_mask = " << debug::print_array(value.neural_enable_mask) << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const perf_cmd_start &value) {
    return os << "perf_cmd_start {\n" //
              << debug::indent_level::push //
              << debug::indent << ".user_data = " << value.user_data << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const perf_cmd_stop &value) {
    return os << "perf_cmd_stop {\n" //
              << debug::indent_level::push //
              << debug::indent << ".user_data = " << value.user_data << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const perf_cmd_sample &value) {
    return os << "perf_cmd_sample {\n" //
              << debug::indent_level::push //
              << debug::indent << ".user_data = " << value.user_data << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

// no `operator<<` for union perf_control_ptr

inline std::ostream &operator<<(std::ostream &os, sample_header::sample_flags value) {
    if (!value)
        return os << 0;

    if (!!(value & sample_header::sample_flags::sample_overflow)) {
        value ^= sample_header::sample_flags::sample_overflow;
        os << "sample_overflow";
    }

    if (!!value)
        os << " | ";

    if (!!(value & sample_header::sample_flags::sample_stretched)) {
        value ^= sample_header::sample_flags::sample_stretched;
        os << "sample_stretched";
    }

    if (!!value)
        os << " | ";

    if (!!value) {
        assert(!&"Unknown bits");
        os << std::hex << static_cast<uint32_t>(value);
    }

    return os;
}

inline std::ostream &operator<<(std::ostream &os, const sample_header &value) {
    return os << "sample_header {\n" //
              << debug::indent_level::push //
              << debug::indent << ".timestamp_start_ns = " << value.timestamp_start_ns << ",\n" //
              << debug::indent << ".timestamp_end_ns = " << value.timestamp_end_ns << ",\n" //
              << debug::indent << ".block_set = " << static_cast<uint32_t>(value.block_set) << ",\n" //
              << debug::indent << ".flags = " << value.flags << ",\n" //
              << debug::indent << ".user_data = " << value.user_data << ",\n" //
              << debug::indent << ".toplevel_clock_cycles = " << value.toplevel_clock_cycles << ",\n" //
              << debug::indent << ".coregroup_clock_cycles = " << value.coregroup_clock_cycles << ",\n" //
              << debug::indent << ".shader_clock_cycles = " << value.shader_clock_cycles << ",\n" //
              << debug::indent << ".neural_clock_cycles = " << value.neural_clock_cycles << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, block_header::block_state value) {
    if (!value)
        return os << 0;

    if (!!(value & block_header::block_state::on)) {
        value ^= block_header::block_state::on;
        os << "on";
    }

    if (!!value)
        os << " | ";

    if (!!(value & block_header::block_state::off)) {
        value ^= block_header::block_state::off;
        os << "off";
    }

    if (!!value)
        os << " | ";

    if (!!(value & block_header::block_state::available)) {
        value ^= block_header::block_state::available;
        os << "available";
    }

    if (!!value)
        os << " | ";

    if (!!(value & block_header::block_state::unavailable)) {
        value ^= block_header::block_state::unavailable;
        os << "unavailable";
    }

    if (!!value)
        os << " | ";

    if (!!(value & block_header::block_state::normal_mode)) {
        value ^= block_header::block_state::normal_mode;
        os << "normal_mode";
    }

    if (!!value)
        os << " | ";

    if (!!(value & block_header::block_state::protected_mode)) {
        value ^= block_header::block_state::protected_mode;
        os << "protected_mode";
    }

    if (!!value)
        os << " | ";

    if (!!value) {
        assert(!&"Unknown bits");
        os << std::hex << static_cast<uint32_t>(value);
    }

    return os;
}

inline std::ostream &operator<<(std::ostream &os, const block_header &value) {
    return os << "block_header {\n" //
              << debug::indent_level::push //
              << debug::indent << ".block_type = " << value.block_type << ",\n" //
              << debug::indent << ".block_idx = " << static_cast<uint32_t>(value.block_idx) << ",\n" //
              << debug::indent << ".block_states = " << value.block_states << ",\n" //
              << debug::indent << ".clock = " << value.clock << ",\n" //
              << debug::indent << ".enable_mask = " << debug::print_array(value.enable_mask) << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, command::command_type cmd) {
    switch (cmd) {
    case command::drm_gem_close:
        return os << "panthor::command::drm_gem_close";
    case command::dev_query:
        return os << "panthor::command::dev_query";
    case command::vm_create:
        return os << "panthor::command::vm_create";
    case command::vm_destroy:
        return os << "panthor::command::vm_destroy";
    case command::bo_create:
        return os << "panthor::command::bo_create";
    case command::bo_mmap_offset:
        return os << "panthor::command::bo_mmap_offset";
    case command::perf_control:
        return os << "panthor::command::perf_control";
    }

    assert(!&"Unknown command");
    return os << "::command::<unknown>";
}

} // namespace panthor
} // namespace ioctl
} // namespace device
} // namespace hwcpipe

#include "print_manual.hpp"

// clang-format on
