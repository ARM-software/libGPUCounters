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
namespace kbase_pre_r21 {

inline bool operator==(const version_check_args &lhs, const version_check_args &rhs);
inline bool operator!=(const version_check_args &lhs, const version_check_args &rhs) { return !(lhs == rhs); }
inline bool operator==(const set_flags_args &lhs, const set_flags_args &rhs);
inline bool operator!=(const set_flags_args &lhs, const set_flags_args &rhs) { return !(lhs == rhs); }
inline bool operator==(const uk_gpuprops::gpu_props::core &lhs, const uk_gpuprops::gpu_props::core &rhs);
inline bool operator!=(const uk_gpuprops::gpu_props::core &lhs, const uk_gpuprops::gpu_props::core &rhs) { return !(lhs == rhs); }
inline bool operator==(const uk_gpuprops::gpu_props::l2_cache &lhs, const uk_gpuprops::gpu_props::l2_cache &rhs);
inline bool operator!=(const uk_gpuprops::gpu_props::l2_cache &lhs, const uk_gpuprops::gpu_props::l2_cache &rhs) { return !(lhs == rhs); }
inline bool operator==(const uk_gpuprops::gpu_props::tiler &lhs, const uk_gpuprops::gpu_props::tiler &rhs);
inline bool operator!=(const uk_gpuprops::gpu_props::tiler &lhs, const uk_gpuprops::gpu_props::tiler &rhs) { return !(lhs == rhs); }
inline bool operator==(const uk_gpuprops::gpu_props::thread &lhs, const uk_gpuprops::gpu_props::thread &rhs);
inline bool operator!=(const uk_gpuprops::gpu_props::thread &lhs, const uk_gpuprops::gpu_props::thread &rhs) { return !(lhs == rhs); }
inline bool operator==(const uk_gpuprops::gpu_props::raw &lhs, const uk_gpuprops::gpu_props::raw &rhs);
inline bool operator!=(const uk_gpuprops::gpu_props::raw &lhs, const uk_gpuprops::gpu_props::raw &rhs) { return !(lhs == rhs); }
inline bool operator==(const uk_gpuprops::gpu_props::coherent_group_info::coherent_group &lhs, const uk_gpuprops::gpu_props::coherent_group_info::coherent_group &rhs);
inline bool operator!=(const uk_gpuprops::gpu_props::coherent_group_info::coherent_group &lhs, const uk_gpuprops::gpu_props::coherent_group_info::coherent_group &rhs) { return !(lhs == rhs); }
inline bool operator==(const uk_gpuprops::gpu_props::coherent_group_info &lhs, const uk_gpuprops::gpu_props::coherent_group_info &rhs);
inline bool operator!=(const uk_gpuprops::gpu_props::coherent_group_info &lhs, const uk_gpuprops::gpu_props::coherent_group_info &rhs) { return !(lhs == rhs); }
inline bool operator==(const uk_gpuprops::gpu_props &lhs, const uk_gpuprops::gpu_props &rhs);
inline bool operator!=(const uk_gpuprops::gpu_props &lhs, const uk_gpuprops::gpu_props &rhs) { return !(lhs == rhs); }
inline bool operator==(const uk_gpuprops &lhs, const uk_gpuprops &rhs);
inline bool operator!=(const uk_gpuprops &lhs, const uk_gpuprops &rhs) { return !(lhs == rhs); }
inline bool operator==(const uk_hwcnt_reader_setup &lhs, const uk_hwcnt_reader_setup &rhs);
inline bool operator!=(const uk_hwcnt_reader_setup &lhs, const uk_hwcnt_reader_setup &rhs) { return !(lhs == rhs); }

inline bool operator==(const uk_gpuprops::gpu_props::core &lhs, const uk_gpuprops::gpu_props::core &rhs) {
    return true //
            && lhs.product_id == rhs.product_id //
            && lhs.version_status == rhs.version_status //
            && lhs.minor_revision == rhs.minor_revision //
            && lhs.major_revision == rhs.major_revision //
            && lhs.gpu_speed_mhz == rhs.gpu_speed_mhz //
            && lhs.gpu_freq_khz_max == rhs.gpu_freq_khz_max //
            && lhs.gpu_freq_khz_min == rhs.gpu_freq_khz_min //
            && lhs.log2_program_counter_size == rhs.log2_program_counter_size //
            && std::equal(std::begin(lhs.texture_features), std::end(lhs.texture_features), std::begin(rhs.texture_features)) //
            && lhs.gpu_available_memory_size == rhs.gpu_available_memory_size //
        ;
}

inline bool operator==(const uk_gpuprops::gpu_props::l2_cache &lhs, const uk_gpuprops::gpu_props::l2_cache &rhs) {
    return true //
            && lhs.log2_line_size == rhs.log2_line_size //
            && lhs.log2_cache_size == rhs.log2_cache_size //
            && lhs.num_l2_slices == rhs.num_l2_slices //
        ;
}

inline bool operator==(const uk_gpuprops::gpu_props::tiler &lhs, const uk_gpuprops::gpu_props::tiler &rhs) {
    return true //
            && lhs.bin_size_bytes == rhs.bin_size_bytes //
            && lhs.max_active_levels == rhs.max_active_levels //
        ;
}

inline bool operator==(const uk_gpuprops::gpu_props::thread &lhs, const uk_gpuprops::gpu_props::thread &rhs) {
    return true //
            && lhs.max_threads == rhs.max_threads //
            && lhs.max_workgroup_size == rhs.max_workgroup_size //
            && lhs.max_barrier_size == rhs.max_barrier_size //
            && lhs.max_registers == rhs.max_registers //
            && lhs.max_task_queue == rhs.max_task_queue //
            && lhs.max_thread_group_split == rhs.max_thread_group_split //
            && lhs.impl_tech == rhs.impl_tech //
        ;
}

inline bool operator==(const uk_gpuprops::gpu_props::raw &lhs, const uk_gpuprops::gpu_props::raw &rhs) {
    return true //
            && lhs.shader_present == rhs.shader_present //
            && lhs.tiler_present == rhs.tiler_present //
            && lhs.l2_present == rhs.l2_present //
            && lhs.unused_1 == rhs.unused_1 //
            && lhs.l2_features == rhs.l2_features //
            && lhs.suspend_size == rhs.suspend_size //
            && lhs.mem_features == rhs.mem_features //
            && lhs.mmu_features == rhs.mmu_features //
            && lhs.as_present == rhs.as_present //
            && lhs.js_present == rhs.js_present //
            && std::equal(std::begin(lhs.js_features), std::end(lhs.js_features), std::begin(rhs.js_features)) //
            && lhs.tiler_features == rhs.tiler_features //
            && std::equal(std::begin(lhs.texture_features), std::end(lhs.texture_features), std::begin(rhs.texture_features)) //
            && lhs.gpu_id == rhs.gpu_id //
            && lhs.thread_max_threads == rhs.thread_max_threads //
            && lhs.thread_max_workgroup_size == rhs.thread_max_workgroup_size //
            && lhs.thread_max_barrier_size == rhs.thread_max_barrier_size //
            && lhs.thread_features == rhs.thread_features //
            && lhs.coherency_mode == rhs.coherency_mode //
        ;
}

inline bool operator==(const uk_gpuprops::gpu_props::coherent_group_info::coherent_group &lhs, const uk_gpuprops::gpu_props::coherent_group_info::coherent_group &rhs) {
    return true //
            && lhs.core_mask == rhs.core_mask //
            && lhs.num_cores == rhs.num_cores //
        ;
}

inline bool operator==(const uk_gpuprops::gpu_props::coherent_group_info &lhs, const uk_gpuprops::gpu_props::coherent_group_info &rhs) {
    return true //
            && lhs.num_groups == rhs.num_groups //
            && lhs.num_core_groups == rhs.num_core_groups //
            && lhs.coherency == rhs.coherency //
            && std::equal(std::begin(lhs.group), std::end(lhs.group), std::begin(rhs.group)) //
        ;
}

inline bool operator==(const uk_gpuprops::gpu_props &lhs, const uk_gpuprops::gpu_props &rhs) {
    return true //
            && lhs.core_props == rhs.core_props //
            && lhs.l2_props == rhs.l2_props //
            && lhs.unused == rhs.unused //
            && lhs.tiler_props == rhs.tiler_props //
            && lhs.thread_props == rhs.thread_props //
            && lhs.raw_props == rhs.raw_props //
            && lhs.coherency_info == rhs.coherency_info //
        ;
}

} // namespace kbase_pre_r21
} // namespace ioctl
} // namespace device
} // namespace hwcpipe

#include "compare_manual.hpp"

// clang-format on
