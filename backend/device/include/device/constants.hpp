/*
 * Copyright (c) 2021-2023 Arm Limited.
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

/**
 * @file
 *
 * Mali Device constants header.
 */

#pragma once

#include <cstdint>

namespace hwcpipe {
namespace device {

/**
 * Mali Device Constants.
 *
 * Various properties of a physical Mali GPU. The values here are to be mainly
 * used for derived counters computation.
 *
 * @warning
 * Do not use this structure to figure out how many blocks of a type are present.
 * Instead, @ref hwcnt::block_extents must be used.
 */
struct constants {
    /** GPU id. */
    uint64_t gpu_id;

    /** CSF Firmware version. */
    uint64_t fw_version;

    /** AXI bus width in bits. */
    uint64_t axi_bus_width;

    /** Number of shader cores. */
    uint64_t num_shader_cores;

    /** The shader core mask. */
    uint64_t shader_core_mask;

    /** Number of L2 cache slices. */
    uint64_t num_l2_slices;

    /** L2 cache slice size in bytes. */
    uint64_t l2_slice_size;

    /** Maximum number of execution engines (per core, over all cores). */
    uint64_t num_exec_engines;

    /** Tile size in pixels. */
    uint64_t tile_size;

    /** Warp width. */
    uint64_t warp_width;
};

} // namespace device
} // namespace hwcpipe
