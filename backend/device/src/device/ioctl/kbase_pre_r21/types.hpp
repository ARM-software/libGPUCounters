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

#include <device/ioctl/offset_pointer.hpp>
#include <device/ioctl/pointer64.hpp>

#include <cstddef>
#include <cstdint>

namespace hwcpipe {
namespace device {
namespace ioctl {

/** Kbase Pre R21 ioctl interface. */
namespace kbase_pre_r21 {
/** Related to mali0 ioctl interface */
enum class header_id : uint32_t {
    /** Version check. */
    version_check = 0x0,
    /** Base Context Create Kernel Flags. */
    create_kernel_flags = 0x2,
    /** Kbase Func UK Func ID. */
    uk_func_id = 512,
    /** Kbase Func Hwcnt Reader Setup. */
    hwcnt_reader_setup = uk_func_id + 36,
    /** Kbase Func Dump. */
    dump = uk_func_id + 11,
    /** Kbase Func Clear. */
    clear = uk_func_id + 12,
    /** Kbase Func Get Props. */
    get_props = uk_func_id + 14,
    /** Kbase Func Set Flags. */
    set_flags = uk_func_id + 18,
};

/** Message header. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
union uk_header {
    /** 32-bit number identifying the UK function to be called. */
    header_id id;
    /** The int return code returned by the called UK function. */
    uint32_t ret;
    /** Used to ensure 64-bit alignment of this union. Do not remove. */
    uint64_t sizer;
};

/** Check version compatibility between kernel and userspace. */
struct version_check_args {
    /** Header. */
    uk_header header;
    /** Major version number. */
    uint16_t major;
    /** Minor version number */
    uint16_t minor;
};

/** IOCTL parameters to set flags */
struct set_flags_args {
    /** Header. */
    uk_header header;
    /** Create Flags. */
    uint32_t create_flags;
    /** Padding. */
    uint32_t padding;
};

/** Base GPU Num Texture Features Registers. */
static constexpr const uint32_t base_gpu_num_texture_features_registers = 3;

/** Base Max Coherent Groups. */
static constexpr const uint32_t base_max_coherent_groups = 16;

/** GPU Max Job Slots. */
static constexpr const uint32_t gpu_max_job_slots = 16;

/** Kbase UK GPU props. */
struct uk_gpuprops {
    /**
     * IOCTL parameters to probe GPU properties
     *
     * NOTE: the raw_props member in this data structure contains the register
     * values from which the value of the other members are derived. The derived
     * members exist to allow for efficient access and/or shielding the details
     * of the layout of the registers.
     *
     */
    struct gpu_props {
        /** Core. */
        struct core {
            /** Product specific value. */
            uint32_t product_id;
            /**
             * Status of the GPU release.
             * No defined values, but starts at 0 and increases by one for each
             * release status (alpha, beta, EAC, etc.).
             * 4 bit values (0-15).
             */
            uint16_t version_status;
            /**
             * Minor release number of the GPU. "P" part of an "RnPn" release number.
             * 8 bit values (0-255).
             */
            uint16_t minor_revision;
            /**
             * Major release number of the GPU. "R" part of an "RnPn" release number.
             * 4 bit values (0-15).
             */
            uint16_t major_revision;
            /** Padding. */
            uint16_t padding;
            /**
             * This property is deprecated since it has not contained the real current
             * value of GPU clock speed. It is kept here only for backwards compatibility.
             * For the new ioctl interface, it is ignored and is treated as a padding
             * to keep the structure of the same size and retain the placement of its
             * members.
             */
            uint32_t gpu_speed_mhz;
            /**
             * GPU clock max speed is required for computing best case
             * in tasks as job scheduling ant irq_throttling. (It is not specified in the
             * Midgard Architecture).
             * Also, GPU clock max speed is used for OpenCL's clGetDeviceInfo() function.
             */
            uint32_t gpu_freq_khz_max;
            /**
             * GPU clock min speed is required for computing worst case
             * in tasks as job scheduling ant irq_throttling. (It is not specified in the
             * Midgard Architecture).
             */
            uint32_t gpu_freq_khz_min;
            /** Size of the shader program counter, in bits. */
            uint32_t log2_program_counter_size;
            /**
             * TEXTURE_FEATURES_x registers, as exposed by the GPU. This is a
             * bitpattern where a set bit indicates that the format is supported.
             *
             * Before using a texture format, it is recommended that the corresponding
             * bit be checked.
             */
            uint32_t texture_features[base_gpu_num_texture_features_registers];
            /**
             * Theoretical maximum memory available to the GPU. It is unlikely that a
             * client will be able to allocate all of this memory for their own
             * purposes, but this at least provides an upper bound on the memory
             * available to the GPU.
             *
             * This is required for OpenCL's clGetDeviceInfo() call when
             * CL_DEVICE_GLOBAL_MEM_SIZE is requested, for OpenCL GPU devices. The
             * client will not be expecting to allocate anywhere near this value.
             */
            uint64_t gpu_available_memory_size;
        };

        /**
         * More information is possible - but associativity and bus width are not
         * required by upper-level apis.
         */
        struct l2_cache {
            /** Log2 Line Size. */
            uint8_t log2_line_size;
            /** Log2 Cache Size. */
            uint8_t log2_cache_size;
            /** Num L2 Slices. */
            uint8_t num_l2_slices;
            /** Padding bytes. */
            uint8_t padding[5];
        };

        /** Tiler. */
        struct tiler {
            /** Max is 4*2^15 */
            uint32_t bin_size_bytes;
            /** Max is 2^15 */
            uint32_t max_active_levels;
        };

        /** GPU threading system details. */
        struct thread {
            /** Max. number of threads per core */
            uint32_t max_threads;
            /** Max. number of threads per workgroup */
            uint32_t max_workgroup_size;
            /** Max. number of threads that can synchronize on a simple barrier */
            uint32_t max_barrier_size;
            /** Total size [1..65535] of the register file available per core. */
            uint16_t max_registers;
            /** Max. tasks [1..255] which may be sent to a core before it becomes blocked. */
            uint8_t max_task_queue;
            /** Max. allowed value [1..15] of the Thread Group Split field. */
            uint8_t max_thread_group_split;
            /** 0 = Not specified, 1 = Silicon, 2 = FPGA, 3 = SW Model/Emulation */
            uint8_t impl_tech;
            /** Padding bytes. */
            uint8_t padding[7];
        };

        /**
         * A complete description of the GPU's Hardware Configuration Discovery
         * registers.
         *
         * The information is presented inefficiently for access. For frequent access,
         * the values should be better expressed in an unpacked form in the
         * base_gpu_props structure.
         *
         * The raw properties in @c gpu_raw_gpu_props are necessary to
         * allow a user of the Mali Tools (e.g. PAT) to determine "Why is this device
         * behaving differently?". In this case, all information about the
         * configuration is potentially useful, but it <b>does not need to be processed
         * by the driver</b>. Instead, the raw registers can be processed by the Mali
         * Tools software on the host PC.
         */
        struct raw {
            /** Shader Present. */
            uint64_t shader_present;
            /** Tiler Present. */
            uint64_t tiler_present;
            /** L2 Present. */
            uint64_t l2_present;
            /** Unused 1. */
            uint64_t unused_1;
            /** L2 Features. */
            uint32_t l2_features;
            /** Suspend Size. */
            uint32_t suspend_size;
            /** Mem Features. */
            uint32_t mem_features;
            /** Mmu Features. */
            uint32_t mmu_features;
            /** As Present. */
            uint32_t as_present;
            /** Js Present. */
            uint32_t js_present;
            /** Js Features. */
            uint32_t js_features[gpu_max_job_slots];
            /** Tiler Features. */
            uint32_t tiler_features;
            /** Texture Features. */
            uint32_t texture_features[3];
            /** GPU ID. */
            uint32_t gpu_id;
            /** Thread Max Threads. */
            uint32_t thread_max_threads;
            /** Thread Max Workgroup Size. */
            uint32_t thread_max_workgroup_size;
            /** Thread Max Barrier Size. */
            uint32_t thread_max_barrier_size;
            /** Thread Features. */
            uint32_t thread_features;
            /**
             * Coherency Mode.
             * Note: This is the _selected_ coherency mode rather than the
             * available modes as exposed in the coherency_features register.
             */
            uint32_t coherency_mode;
        };

        /**
         * Coherency group information
         *
         * Note that the sizes of the members could be reduced. However, the \c group
         * member might be 8-byte aligned to ensure the u64 core_mask is 8-byte
         * aligned, thus leading to wastage if the other members sizes were reduced.
         *
         * The groups are sorted by core mask. The core masks are non-repeating and do
         * not intersect.
         */
        struct coherent_group_info {
            /**
             * descriptor for a coherent group
             *
             * \c core_mask exposes all cores in that coherent group, and \c num_cores
             * provides a cached population-count for that mask.
             *
             * @note Whilst all cores are exposed in the mask, not all may be available to
             * the application, depending on the Kernel Power policy.
             *
             * @note if u64s must be 8-byte aligned, then this structure has 32-bits of
             * wastage.
             */
            struct coherent_group {
                /** Core restriction mask required for the group */
                uint64_t core_mask;
                /** Number of cores in the group */
                uint16_t num_cores;
                /** Padding bytes. */
                uint16_t padding[3];
            };

            /** Num Groups. */
            uint32_t num_groups;
            /**
             * Number of core groups (coherent or not) in the GPU. Equivalent to the number of
             * L2 Caches.
             * The GPU Counter dumping writes 2048 bytes per core group, regardless of whether
             * the core groups are coherent or not. Hence this member is needed to calculate
             * how much memory is required for dumping.
             * @note Do not use it to work out how many valid elements are in the group[]
             * member. Use num_groups instead.
             */
            uint32_t num_core_groups;
            /** Coherency features of the memory, accessed by @c gpu_mem_features methods. */
            uint32_t coherency;
            /** Padding. */
            uint32_t padding;
            /** Descriptors of coherent groups */
            coherent_group group[base_max_coherent_groups];
        };

        /** Core Props. */
        core core_props;
        /** L2 Props. */
        l2_cache l2_props;
        /** Unused to keep for backwards compatibility. */
        uint64_t unused;
        /** Tiler Props. */
        tiler tiler_props;
        /** Thread Props. */
        thread thread_props;
        /** This member is large, likely to be 128 bytes. */
        raw raw_props;
        /** This must be last member of the structure. */
        coherent_group_info coherency_info;
    };

    /** Header. */
    uk_header header;
    /** Props. */
    gpu_props props;
};

/** Kbase UK hardware counter reader setup. */
struct uk_hwcnt_reader_setup {
    /** UK structure header. */
    uk_header header;
    /** Requested number of dumping buffers. */
    uint32_t buffer_count;
    /** counters selection bitmask (JM) */
    uint32_t jm_bm;
    /** counters selection bitmask (Shader) */
    uint32_t shader_bm;
    /** counters selection bitmask (Tiler) */
    uint32_t tiler_bm;
    /** counters selection bitmask (MMU_L2) */
    uint32_t mmu_l2_bm;
    /** dumping notification file descriptor */
    int32_t fd;
};

// clang-format off
// clang-format on
} // namespace kbase_pre_r21
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
