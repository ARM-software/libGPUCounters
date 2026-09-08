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

#include <device/ioctl/offset_pointer.hpp>
#include <device/ioctl/pointer64.hpp>

#include <cstddef>
#include <cstdint>

namespace hwcpipe {
namespace device {
namespace ioctl {

/** Panthor ioctl interface (HWC subset). */
namespace panthor {
/** DRM_IOCTL_GEM_CLOSE ioctl argument type. */
struct drm_gem_close {
    /** Handle of the object to be closed. */
    uint32_t handle;
    /** MBZ. */
    uint32_t pad;
};

/** Query type */
enum class dev_query_type : uint32_t {
    /** Query GPU information. */
    gpu_info = 0,
    /** Query command-stream interface information. */
    csif_info,
    /** Query timestamp information */
    timestamp_info,
    /** Query allowed group priorities information. */
    group_priorities_info,
    /** Query perf counters interface information. */
    perf_info = ((1 << 30) - 2),
    /** Query GPU instrumentation information. */
    instr_info = ((1 << 30) - 1),
};

/** GPU information. */
struct gpu_info {
    /** GPU ID. */
    uint32_t gpu_id;
    /** GPU revision. */
    uint32_t gpu_rev;
    /** CSF ID. */
    uint32_t csf_id;
    /** L2$ features. */
    uint32_t l2_features;
    /** Tiler features. */
    uint32_t tiler_features;
    /** Memory features. */
    uint32_t mem_features;
    /** MMU features. */
    uint32_t mmu_features;
    /** Thread features. */
    uint32_t thread_features;
    /** Maximum number of threads. */
    uint32_t max_threads;
    /** Maximum workgroup size. */
    uint32_t thread_max_workgroup_size;
    /** Maximum number of threads that can wait simultaneously on a barrier. */
    uint32_t thread_max_barrier_size;
    /** Coherency features. */
    uint32_t coherency_features;
    /** Texture features. */
    uint32_t texture_features[4];
    /** Bitmask encoding the number of address spaces exposed by the MMU. */
    uint32_t as_present;
    /** Padding MBZ. */
    uint32_t pad0;
    /** Bitmask encoding the shader cores exposed by the GPU. */
    uint64_t shader_present;
    /** Bitmask encoding the L2 caches exposed by the GPU. */
    uint64_t l2_present;
    /** Bitmask encoding the tiler units exposed by the GPU. */
    uint64_t tiler_present;
    /** Used to determine core variants when they exist. */
    uint32_t core_features;
    /** Padding MBZ. */
    uint32_t pad;
    /** Bitmask describing GPU-wide features */
    uint64_t gpu_features;
    /** Padding MBZ. */
    uint32_t pad1[19];
    /** Upper 32 bits of GPU_ID. */
    uint32_t gpu_id_hi;
    /** Upper 32 bits of REVIDR. */
    uint32_t gpu_rev_hi;
    /** Upper 32 bits of L2 features. */
    uint32_t l2_features_hi;
    /** Neural accelerator present bitmap. */
    uint64_t neural_present;
    /** Shader core base present bitmap. */
    uint64_t base_present;
    /** Granularity of number of active threads. */
    uint32_t thread_num_active_granularity;
    /** Value of set coherency mode. */
    uint32_t coherency_enable;
};

/** Command stream interface information */
struct csif_info {
    /** Number of command stream group slots exposed by the firmware. */
    uint32_t csg_slot_count;
    /** Number of command stream slots per group. */
    uint32_t cs_slot_count;
    /** Number of command stream registers. */
    uint32_t cs_reg_count;
    /** Number of scoreboard slots. */
    uint32_t scoreboard_slot_count;
    /**
     * Number of command stream registers reserved by the kernel driver to call a
     * userspace command stream.
     */
    uint32_t unpreserved_cs_reg_count;
    /** MBZ. */
    uint32_t pad[9];
    /** Version of the global CSF interface. */
    uint64_t version;
    /** Global CSF interface features. */
    uint32_t features;
    /** Padding field, set to zero. */
    uint32_t pad1;
};

/** Identifier of the clock used to produce the cycle count in the associated block. */
enum class perf_clock : uint8_t {
    /** Top-level CSF clock. */
    toplevel = 0,
    /** Core group clock. */
    coregroup,
    /** Shader core clock */
    shader,
    /** Neural accelerator clock */
    neural,
};

/** The counter set to request from the hardware. */
enum class perf_counter_set : uint8_t {
    /** Primary counter set. */
    primary = 0,
    /** Secondary counter set. */
    secondary = 1,
    /** Tertiary counter set. */
    tertiary = 2,
};

/** Performance counters interface information */
struct perf_info {
    /** Performance counter configuration feature flags. */
    enum class perf_feat_flags : uint32_t {
        /** Coarse-grained block states are supported. */
        block_states_support = (uint32_t{1} << 0),
    };

    /** The number of 8-byte counters available in a block. */
    uint32_t counters_per_block;
    /** The size of the header struct available at the beginning of every sample. */
    uint32_t sample_header_size;
    /** The size of the header struct inline with the counters for a single block. */
    uint32_t block_header_size;
    /** The size of a fully annotated sample. */
    uint32_t sample_size;
    /** combination of the perf_feat_flags */
    perf_feat_flags flags;
    /** Bitmask of the clocks supported by the GPU. */
    uint32_t supported_clocks;
    /** Number of FW blocks available. */
    uint32_t fw_blocks;
    /** Number of CSHW blocks available. */
    uint32_t cshw_blocks;
    /** Number of geometry blocks available. */
    uint32_t geometry_blocks;
    /** Number of memsys blocks available. */
    uint32_t memsys_blocks;
    /** Number of shader core blocks available. */
    uint32_t shader_blocks;
    /** Number of neural core blocks available. */
    uint32_t neural_blocks;
};

/** Fabricated union to satisfy pointer64 type system. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
union dev_query_ptr {
    /** Query GPU info. */
    pointer64<gpu_info> u_gpu_info;
    /** Query CSIF info. */
    pointer64<csif_info> u_csif_info;
    /** Query perf info. */
    pointer64<perf_info> u_perf_info;
};

/** Query device properties. */
struct dev_query {
    /** The query type. */
    dev_query_type type;
    /**
     * Size of the type being queried. If `pointer` is `NULL`, `size` is
     * updated by the driver to provide the output structure size. If it is
     * not `NULL`, the driver will only copy the smaller of `size` or the
     * actual structure size.
     */
    uint32_t size;
    /** User pointer to a query type struct. */
    dev_query_ptr pointer;
};

/** Create a VM. */
struct vm_create {
    /** VM flags, MBZ. */
    uint32_t flags;
    /** Returned VM ID. */
    uint32_t id;
    /**
     * Size of the VA space reserved for user objects (can be set to zero when not
     * relying on
     * the size of the reservation, in which case the kernel will choose an appropriate
     * carveout based on the TASK_SIZE and the GPU MMU supported VA space).
     */
    uint64_t user_va_range;
};

/** Destroy a VM. */
struct vm_destroy {
    /** ID of the VM to destroy. */
    uint32_t id;
    /** MBZ. */
    uint32_t pad;
};

/** Arguments passed to DRM_IOCTL_PANTHOR_BO_CREATE. */
struct bo_create {
    /**
     * Requested size for the object. The (page-aligned) allocated size for the object
     * will be returned.
     */
    uint64_t size;
    /** Flags. Must be a combination of drm_panthor_bo_flags flags. */
    uint32_t flags;
    /**
     * Exclusive VM this buffer object will be mapped to.
     * If not zero, the field must refer to a valid VM ID, and implies that,
     *   * the buffer object will only ever be bound to that VM;
     *   * cannot be exported as a PRIME fd.
     */
    uint32_t exclusive_vm_id;
    /** Returned handle for the object. Object handles are nonzero. */
    uint32_t handle;
    /** MBZ. */
    uint32_t pad[10];
    /** Granularity of the alloc-on-fault behavior, in bytes. */
    uint32_t alloc_on_fault_granularity;
    /** Corresponding offset of the object if used for sync memory. */
    uint64_t sync_offset;
};

/** Arguments passed to DRM_IOCTL_PANTHOR_BO_MMAP_OFFSET. */
struct bo_mmap_offset {
    /** Handle of the object we want an mmap offset for. */
    uint32_t handle;
    /** MBZ. */
    uint32_t pad;
    /** The fake offset to use for subsequent mmap calls. */
    uint64_t offset;
};

/** The hardware block types supported by the interface. */
enum class perf_block_type : uint8_t {
    /** The FW counter block. */
    fw_block = 1,
    /** The CSHW (CSF frontend) counter block. */
    cshw_block,
    /** The geometry counter block. */
    geometry_block,
    /** A memsys (L2/MMU) counter block. */
    memsys_block,
    /** A shader core counter blocks. */
    shader_block,
    /** A neural core counter block. */
    neural_block,
};

/** Command type passed to the DRM_PANTHOR_PERF_CONTROL IOCTL. */
enum class perf_command : uint32_t {
    /** Create a new performance counter sampling context. */
    setup = 0,
    /** Teardown a performance counter sampling context. */
    teardown,
    /** Start a sampling session on the indicated context. */
    start,
    /** Stop the sampling session on the indicated context. */
    stop,
    /** Request a manual sample on the indicated context. */
    sample,
};

/** Struct mapped at the start of the control BO to read and write the indices. */
struct perf_ringbuf_control {
    /** The index of the latest sample that was processed by userspace. */
    uint64_t extract_idx;
    /** The index of the latest sample emitted by the kernel. */
    uint64_t insert_idx;
};

/** Arguments passed to DRM_IOCTL_PANTHOR_PERF_SETUP */
struct perf_cmd_setup {
    /** Set of performance counter blocks. */
    uint8_t block_set;
    /** MBZ. */
    uint8_t pad[7];
    /** eventfd for signalling the availability of a new sample. */
    uint32_t fd;
    /** Handle to the BO to write performance counter samples to. */
    uint32_t ringbuf_handle;
    /** Handle to the BO containing a contiguous 16 byte range. */
    uint32_t control_handle;
    /**
     * The number of slots available in the userspace-provided BO. Must be a power of
     * 2.
     */
    uint32_t sample_slots;
    /** Offset into the control BO where the insert and extract indices are located. */
    uint64_t control_offset;
    /** Sampling period in nanoseconds. Zero turns off periodic sampling. */
    uint64_t sample_freq_ns;
    /** Bitmask of counters to request from the FW counter block. */
    uint64_t fw_enable_mask[2];
    /** Bitmask of counters to request from the CSHW counter block. */
    uint64_t cshw_enable_mask[2];
    /** Bitmask of counters to request from the geometry counter block. */
    uint64_t geometry_enable_mask[2];
    /** Bitmask of counters to request from the memsys counter block. */
    uint64_t memsys_enable_mask[2];
    /** Bitmask of counters to request from the shader core counter block. */
    uint64_t shader_enable_mask[2];
    /** Bitmask of counters to request from the neural accelerator counter block. */
    uint64_t neural_enable_mask[2];
};

/** Arguments passed to DRM_PANTHOR_PERF_CONTROL when starting a session. */
struct perf_cmd_start {
    /** User provided data that will be attached to the automatic sample. */
    uint64_t user_data;
};

/** Arguments passed to DRM_PANTHOR_PERF_CONTROL when stop a session. */
struct perf_cmd_stop {
    /** User provided data that will be attached to samples until the next stop command. */
    uint64_t user_data;
};

/** Arguments passed to DRM_PANTHOR_PERF_CONTROL when requesting a sample manually. */
struct perf_cmd_sample {
    /** User provided data that will be attached to the last sample of the session. */
    uint64_t user_data;
};

/** Fabricated union to satisfy pointer64 type system. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
union perf_control_ptr {
    /** Submit the PERF_CONTROL.SETUP command. */
    pointer64<perf_cmd_setup> u_perf_cmd_setup;
    /** Submit the PERF_CONTROL.TEARDOWN command. */
    pointer64<std::nullptr_t> u_perf_cmd_teardown;
    /** Submit the PERF_CONTROL.START command. */
    pointer64<perf_cmd_start> u_perf_cmd_start;
    /** Submit the PERF_CONTROL.STOP command. */
    pointer64<perf_cmd_stop> u_perf_cmd_stop;
    /** Submit the PERF_CONTROL.SAMPLE command. */
    pointer64<perf_cmd_sample> u_perf_cmd_sample;
};

/** Arguments to the DRM_PANTHOR_PERF_CONTROL IOCTL. */
struct perf_control {
    /** Command from enum drm_panthor_perf_command. */
    perf_command cmd;
    /** Session handle returned by the setup call */
    uint32_t handle;
    /** size of the command structure. */
    uint64_t size;
    /** user pointer to a query type struct */
    perf_control_ptr pointer;
};

/** Header present before every sample. */
struct sample_header {
    /** Sample-wide events that occurred. */
    enum class sample_flags : uint32_t {
        /** This sample contains overflows. */
        sample_overflow = (uint32_t{1} << 0),
        /** The sample period is stretched for this sample. */
        sample_stretched = (uint32_t{1} << 1),
    };

    /** Earlier timestamp that values in this sample represent, in ns. */
    uint64_t timestamp_start_ns;
    /** Earlier timestamp that values in this sample represent, in ns. */
    uint64_t timestamp_end_ns;
    /** Set of performance counter blocks. */
    uint8_t block_set;
    /** MBZ. */
    uint8_t pad[3];
    /** Flags */
    sample_flags flags;
    /** User data provided as part of the command that triggered this sample. */
    uint64_t user_data;
    /**
     * The number of cycles timestamp_start_ns and timestamp_end_ns on the top-level
     * clock if it exists.
     */
    uint64_t toplevel_clock_cycles;
    /**
     * The number of cycles timestamp_start_ns and timestamp_end_ns on the coregroup
     * clock if it exists.
     */
    uint64_t coregroup_clock_cycles;
    /**
     * The number of cycles timestamp_start_ns and timestamp_end_ns on the shader core
     * clock if it exists.
     */
    uint64_t shader_clock_cycles;
    /**
     * The number of cycles timestamp_start_ns and timestamp_end_ns on the neural
     * accelerators clock if it exists.
     */
    uint64_t neural_clock_cycles;
};

/** Header present before every block in the sample ringbuffer. */
struct block_header {
    /** Block state transitions */
    enum class block_state : uint8_t {
        /** This block was powered on for some or all of the sampling period. */
        on = (uint8_t{1} << 0),
        /** This block was powered off for some or all of the sampling period. */
        off = (uint8_t{1} << 1),
        /** This block was available for execution for some or all of the sampling period. */
        available = (uint8_t{1} << 2),
        /** This block was available for execution for some or all of the sampling period. */
        unavailable = (uint8_t{1} << 3),
        /** This block was executing in normal mode for some or all of the sampling period. */
        normal_mode = (uint8_t{1} << 4),
        /**
         * This block was executing in protected mode for some or all of the sampling
         * period.
         */
        protected_mode = (uint8_t{1} << 5),
    };

    /** Type of the block. */
    perf_block_type block_type;
    /** Block index. */
    uint8_t block_idx;
    /** Coarse-grained block transitions. */
    block_state block_states;
    /** Clock used to produce the cycle count for this block. */
    perf_clock clock;
    /** MBZ. */
    uint8_t pad[4];
    /** Bitmask of counters requested during the session startup. */
    uint64_t enable_mask[2];
};

// clang-format off
inline perf_info::perf_feat_flags operator~(perf_info::perf_feat_flags val) {
    const auto val_raw = static_cast<uint32_t>(val);
    return static_cast<perf_info::perf_feat_flags>(~val_raw);
}

inline bool operator!(perf_info::perf_feat_flags val) {
    const auto val_raw = static_cast<uint32_t>(val);
    return val_raw == 0;
}

inline perf_info::perf_feat_flags operator|(perf_info::perf_feat_flags lhs, perf_info::perf_feat_flags rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<perf_info::perf_feat_flags>(lhs_raw | rhs_raw);
}

inline perf_info::perf_feat_flags operator&(perf_info::perf_feat_flags lhs, perf_info::perf_feat_flags rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<perf_info::perf_feat_flags>(lhs_raw & rhs_raw);
}

inline perf_info::perf_feat_flags operator^(perf_info::perf_feat_flags lhs, perf_info::perf_feat_flags rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<perf_info::perf_feat_flags>(lhs_raw ^ rhs_raw);
}

inline perf_info::perf_feat_flags &operator|=(perf_info::perf_feat_flags &lhs, perf_info::perf_feat_flags rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline perf_info::perf_feat_flags &operator&=(perf_info::perf_feat_flags &lhs, perf_info::perf_feat_flags rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline perf_info::perf_feat_flags &operator^=(perf_info::perf_feat_flags &lhs, perf_info::perf_feat_flags rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

inline sample_header::sample_flags operator~(sample_header::sample_flags val) {
    const auto val_raw = static_cast<uint32_t>(val);
    return static_cast<sample_header::sample_flags>(~val_raw);
}

inline bool operator!(sample_header::sample_flags val) {
    const auto val_raw = static_cast<uint32_t>(val);
    return val_raw == 0;
}

inline sample_header::sample_flags operator|(sample_header::sample_flags lhs, sample_header::sample_flags rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<sample_header::sample_flags>(lhs_raw | rhs_raw);
}

inline sample_header::sample_flags operator&(sample_header::sample_flags lhs, sample_header::sample_flags rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<sample_header::sample_flags>(lhs_raw & rhs_raw);
}

inline sample_header::sample_flags operator^(sample_header::sample_flags lhs, sample_header::sample_flags rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<sample_header::sample_flags>(lhs_raw ^ rhs_raw);
}

inline sample_header::sample_flags &operator|=(sample_header::sample_flags &lhs, sample_header::sample_flags rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline sample_header::sample_flags &operator&=(sample_header::sample_flags &lhs, sample_header::sample_flags rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline sample_header::sample_flags &operator^=(sample_header::sample_flags &lhs, sample_header::sample_flags rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

inline block_header::block_state operator~(block_header::block_state val) {
    const auto val_raw = static_cast<uint8_t>(val);
    return static_cast<block_header::block_state>(~val_raw);
}

inline bool operator!(block_header::block_state val) {
    const auto val_raw = static_cast<uint8_t>(val);
    return val_raw == 0;
}

inline block_header::block_state operator|(block_header::block_state lhs, block_header::block_state rhs) {
    const auto lhs_raw = static_cast<uint8_t>(lhs);
    const auto rhs_raw = static_cast<uint8_t>(rhs);
    return static_cast<block_header::block_state>(lhs_raw | rhs_raw);
}

inline block_header::block_state operator&(block_header::block_state lhs, block_header::block_state rhs) {
    const auto lhs_raw = static_cast<uint8_t>(lhs);
    const auto rhs_raw = static_cast<uint8_t>(rhs);
    return static_cast<block_header::block_state>(lhs_raw & rhs_raw);
}

inline block_header::block_state operator^(block_header::block_state lhs, block_header::block_state rhs) {
    const auto lhs_raw = static_cast<uint8_t>(lhs);
    const auto rhs_raw = static_cast<uint8_t>(rhs);
    return static_cast<block_header::block_state>(lhs_raw ^ rhs_raw);
}

inline block_header::block_state &operator|=(block_header::block_state &lhs, block_header::block_state rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline block_header::block_state &operator&=(block_header::block_state &lhs, block_header::block_state rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline block_header::block_state &operator^=(block_header::block_state &lhs, block_header::block_state rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

// clang-format on
} // namespace panthor
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
