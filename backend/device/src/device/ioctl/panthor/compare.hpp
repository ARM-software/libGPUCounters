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

#include <algorithm>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace panthor {

inline bool operator==(const drm_gem_close &lhs, const drm_gem_close &rhs);
inline bool operator!=(const drm_gem_close &lhs, const drm_gem_close &rhs) { return !(lhs == rhs); }
inline bool operator==(const gpu_info &lhs, const gpu_info &rhs);
inline bool operator!=(const gpu_info &lhs, const gpu_info &rhs) { return !(lhs == rhs); }
inline bool operator==(const csif_info &lhs, const csif_info &rhs);
inline bool operator!=(const csif_info &lhs, const csif_info &rhs) { return !(lhs == rhs); }
inline bool operator==(const perf_info &lhs, const perf_info &rhs);
inline bool operator!=(const perf_info &lhs, const perf_info &rhs) { return !(lhs == rhs); }
inline bool operator==(const dev_query &lhs, const dev_query &rhs);
inline bool operator!=(const dev_query &lhs, const dev_query &rhs) { return !(lhs == rhs); }
inline bool operator==(const vm_create &lhs, const vm_create &rhs);
inline bool operator!=(const vm_create &lhs, const vm_create &rhs) { return !(lhs == rhs); }
inline bool operator==(const vm_destroy &lhs, const vm_destroy &rhs);
inline bool operator!=(const vm_destroy &lhs, const vm_destroy &rhs) { return !(lhs == rhs); }
inline bool operator==(const bo_create &lhs, const bo_create &rhs);
inline bool operator!=(const bo_create &lhs, const bo_create &rhs) { return !(lhs == rhs); }
inline bool operator==(const bo_mmap_offset &lhs, const bo_mmap_offset &rhs);
inline bool operator!=(const bo_mmap_offset &lhs, const bo_mmap_offset &rhs) { return !(lhs == rhs); }
inline bool operator==(const perf_ringbuf_control &lhs, const perf_ringbuf_control &rhs);
inline bool operator!=(const perf_ringbuf_control &lhs, const perf_ringbuf_control &rhs) { return !(lhs == rhs); }
inline bool operator==(const perf_cmd_setup &lhs, const perf_cmd_setup &rhs);
inline bool operator!=(const perf_cmd_setup &lhs, const perf_cmd_setup &rhs) { return !(lhs == rhs); }
inline bool operator==(const perf_cmd_start &lhs, const perf_cmd_start &rhs);
inline bool operator!=(const perf_cmd_start &lhs, const perf_cmd_start &rhs) { return !(lhs == rhs); }
inline bool operator==(const perf_cmd_stop &lhs, const perf_cmd_stop &rhs);
inline bool operator!=(const perf_cmd_stop &lhs, const perf_cmd_stop &rhs) { return !(lhs == rhs); }
inline bool operator==(const perf_cmd_sample &lhs, const perf_cmd_sample &rhs);
inline bool operator!=(const perf_cmd_sample &lhs, const perf_cmd_sample &rhs) { return !(lhs == rhs); }
inline bool operator==(const perf_control &lhs, const perf_control &rhs);
inline bool operator!=(const perf_control &lhs, const perf_control &rhs) { return !(lhs == rhs); }
inline bool operator==(const sample_header &lhs, const sample_header &rhs);
inline bool operator!=(const sample_header &lhs, const sample_header &rhs) { return !(lhs == rhs); }
inline bool operator==(const block_header &lhs, const block_header &rhs);
inline bool operator!=(const block_header &lhs, const block_header &rhs) { return !(lhs == rhs); }

inline bool operator==(const drm_gem_close &lhs, const drm_gem_close &rhs) {
    return true //
            && lhs.handle == rhs.handle //
            && lhs.pad == rhs.pad //
        ;
}

inline bool operator==(const gpu_info &lhs, const gpu_info &rhs) {
    return true //
            && lhs.gpu_id == rhs.gpu_id //
            && lhs.gpu_rev == rhs.gpu_rev //
            && lhs.csf_id == rhs.csf_id //
            && lhs.l2_features == rhs.l2_features //
            && lhs.tiler_features == rhs.tiler_features //
            && lhs.mem_features == rhs.mem_features //
            && lhs.mmu_features == rhs.mmu_features //
            && lhs.thread_features == rhs.thread_features //
            && lhs.max_threads == rhs.max_threads //
            && lhs.thread_max_workgroup_size == rhs.thread_max_workgroup_size //
            && lhs.thread_max_barrier_size == rhs.thread_max_barrier_size //
            && lhs.coherency_features == rhs.coherency_features //
            && std::equal(std::begin(lhs.texture_features), std::end(lhs.texture_features), std::begin(rhs.texture_features)) //
            && lhs.as_present == rhs.as_present //
            && lhs.pad0 == rhs.pad0 //
            && lhs.shader_present == rhs.shader_present //
            && lhs.l2_present == rhs.l2_present //
            && lhs.tiler_present == rhs.tiler_present //
            && lhs.core_features == rhs.core_features //
            && lhs.pad == rhs.pad //
            && lhs.gpu_features == rhs.gpu_features //
            && std::equal(std::begin(lhs.pad1), std::end(lhs.pad1), std::begin(rhs.pad1)) //
            && lhs.gpu_id_hi == rhs.gpu_id_hi //
            && lhs.gpu_rev_hi == rhs.gpu_rev_hi //
            && lhs.l2_features_hi == rhs.l2_features_hi //
            && lhs.neural_present == rhs.neural_present //
            && lhs.base_present == rhs.base_present //
            && lhs.thread_num_active_granularity == rhs.thread_num_active_granularity //
            && lhs.coherency_enable == rhs.coherency_enable //
        ;
}

inline bool operator==(const csif_info &lhs, const csif_info &rhs) {
    return true //
            && lhs.csg_slot_count == rhs.csg_slot_count //
            && lhs.cs_slot_count == rhs.cs_slot_count //
            && lhs.cs_reg_count == rhs.cs_reg_count //
            && lhs.scoreboard_slot_count == rhs.scoreboard_slot_count //
            && lhs.unpreserved_cs_reg_count == rhs.unpreserved_cs_reg_count //
            && std::equal(std::begin(lhs.pad), std::end(lhs.pad), std::begin(rhs.pad)) //
            && lhs.version == rhs.version //
            && lhs.features == rhs.features //
            && lhs.pad1 == rhs.pad1 //
        ;
}

inline bool operator==(const perf_info &lhs, const perf_info &rhs) {
    return true //
            && lhs.counters_per_block == rhs.counters_per_block //
            && lhs.sample_header_size == rhs.sample_header_size //
            && lhs.block_header_size == rhs.block_header_size //
            && lhs.sample_size == rhs.sample_size //
            && lhs.flags == rhs.flags //
            && lhs.supported_clocks == rhs.supported_clocks //
            && lhs.fw_blocks == rhs.fw_blocks //
            && lhs.cshw_blocks == rhs.cshw_blocks //
            && lhs.geometry_blocks == rhs.geometry_blocks //
            && lhs.memsys_blocks == rhs.memsys_blocks //
            && lhs.shader_blocks == rhs.shader_blocks //
            && lhs.neural_blocks == rhs.neural_blocks //
        ;
}

inline bool operator==(const vm_create &lhs, const vm_create &rhs) {
    return true //
            && lhs.flags == rhs.flags //
            && lhs.id == rhs.id //
            && lhs.user_va_range == rhs.user_va_range //
        ;
}

inline bool operator==(const vm_destroy &lhs, const vm_destroy &rhs) {
    return true //
            && lhs.id == rhs.id //
        ;
}

inline bool operator==(const bo_create &lhs, const bo_create &rhs) {
    return true //
            && lhs.size == rhs.size //
            && lhs.flags == rhs.flags //
            && lhs.exclusive_vm_id == rhs.exclusive_vm_id //
            && lhs.handle == rhs.handle //
            && std::equal(std::begin(lhs.pad), std::end(lhs.pad), std::begin(rhs.pad)) //
            && lhs.alloc_on_fault_granularity == rhs.alloc_on_fault_granularity //
            && lhs.sync_offset == rhs.sync_offset //
        ;
}

inline bool operator==(const bo_mmap_offset &lhs, const bo_mmap_offset &rhs) {
    return true //
            && lhs.handle == rhs.handle //
            && lhs.pad == rhs.pad //
            && lhs.offset == rhs.offset //
        ;
}

inline bool operator==(const perf_ringbuf_control &lhs, const perf_ringbuf_control &rhs) {
    return true //
            && lhs.extract_idx == rhs.extract_idx //
            && lhs.insert_idx == rhs.insert_idx //
        ;
}

inline bool operator==(const perf_cmd_setup &lhs, const perf_cmd_setup &rhs) {
    return true //
            && lhs.block_set == rhs.block_set //
            && lhs.fd == rhs.fd //
            && lhs.ringbuf_handle == rhs.ringbuf_handle //
            && lhs.control_handle == rhs.control_handle //
            && lhs.sample_slots == rhs.sample_slots //
            && lhs.control_offset == rhs.control_offset //
            && lhs.sample_freq_ns == rhs.sample_freq_ns //
            && std::equal(std::begin(lhs.fw_enable_mask), std::end(lhs.fw_enable_mask), std::begin(rhs.fw_enable_mask)) //
            && std::equal(std::begin(lhs.cshw_enable_mask), std::end(lhs.cshw_enable_mask), std::begin(rhs.cshw_enable_mask)) //
            && std::equal(std::begin(lhs.geometry_enable_mask), std::end(lhs.geometry_enable_mask), std::begin(rhs.geometry_enable_mask)) //
            && std::equal(std::begin(lhs.memsys_enable_mask), std::end(lhs.memsys_enable_mask), std::begin(rhs.memsys_enable_mask)) //
            && std::equal(std::begin(lhs.shader_enable_mask), std::end(lhs.shader_enable_mask), std::begin(rhs.shader_enable_mask)) //
            && std::equal(std::begin(lhs.neural_enable_mask), std::end(lhs.neural_enable_mask), std::begin(rhs.neural_enable_mask)) //
        ;
}

inline bool operator==(const perf_cmd_start &lhs, const perf_cmd_start &rhs) {
    return true //
            && lhs.user_data == rhs.user_data //
        ;
}

inline bool operator==(const perf_cmd_stop &lhs, const perf_cmd_stop &rhs) {
    return true //
            && lhs.user_data == rhs.user_data //
        ;
}

inline bool operator==(const perf_cmd_sample &lhs, const perf_cmd_sample &rhs) {
    return true //
            && lhs.user_data == rhs.user_data //
        ;
}

inline bool operator==(const sample_header &lhs, const sample_header &rhs) {
    return true //
            && lhs.timestamp_start_ns == rhs.timestamp_start_ns //
            && lhs.timestamp_end_ns == rhs.timestamp_end_ns //
            && lhs.block_set == rhs.block_set //
            && lhs.flags == rhs.flags //
            && lhs.user_data == rhs.user_data //
            && lhs.toplevel_clock_cycles == rhs.toplevel_clock_cycles //
            && lhs.coregroup_clock_cycles == rhs.coregroup_clock_cycles //
            && lhs.shader_clock_cycles == rhs.shader_clock_cycles //
            && lhs.neural_clock_cycles == rhs.neural_clock_cycles //
        ;
}

inline bool operator==(const block_header &lhs, const block_header &rhs) {
    return true //
            && lhs.block_type == rhs.block_type //
            && lhs.block_idx == rhs.block_idx //
            && lhs.block_states == rhs.block_states //
            && lhs.clock == rhs.clock //
            && std::equal(std::begin(lhs.enable_mask), std::end(lhs.enable_mask), std::begin(rhs.enable_mask)) //
        ;
}

} // namespace panthor
} // namespace ioctl
} // namespace device
} // namespace hwcpipe

#include "compare_manual.hpp"

// clang-format on
