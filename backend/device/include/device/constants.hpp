/*
 * Copyright (c) 2021-2026 Arm Limited.
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
#include <sstream>
#include <string>

#include <inttypes.h>

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

/**
 * @brief Print out constants in string format.
 *
 * Logging the @ref constants data.
 *
 * Data is output in the same order they appear in the struct @ref constants, it is not annotated
 * for the sake of saving space.
 *
 * @param[in] c     Constants.
 * @return std::string.
 */
inline std::string get_constants_str(const constants &c) {
    char shader_core_mask_hex[64];
    char gpu_id_hex[64];
    snprintf(shader_core_mask_hex, 64, "0x%" PRIX64, c.shader_core_mask);
    snprintf(gpu_id_hex, 64, "0x%" PRIX64, c.gpu_id);
    return "{gpu_id=" + std::string(gpu_id_hex) + ", fw_version=" + std::to_string(c.fw_version) +
           ", axi_bus_width=" + std::to_string(c.axi_bus_width) +
           ", num_shader_cores=" + std::to_string(c.num_shader_cores) + ", shader_core_mask=" + shader_core_mask_hex +
           ", num_l2_slices=" + std::to_string(c.num_l2_slices) + ", l2_slice_size=" + std::to_string(c.l2_slice_size) +
           ", num_exec_engines=" + std::to_string(c.num_exec_engines) + ", tile_size=" + std::to_string(c.tile_size) +
           ", warp_width=" + std::to_string(c.warp_width) + "}";
}

} // namespace device
} // namespace hwcpipe
