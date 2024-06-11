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
 * Hardware counters block metadata header.
 */

#pragma once

#include <device/hwcnt/prfcnt_set.hpp>

#include <cassert>
#include <cstdint>

namespace hwcpipe {
namespace device {
namespace hwcnt {

class reader;

/** Hardware counters block type. */
enum class block_type : uint8_t {
    /** Front End. */
    fe,
    /** Tiler. */
    tiler,
    /** Memory System. */
    memory,
    /** Shader Core. */
    core,
    /** CSF Firmware. */
    firmware,
    /** Firmware command stream group. */
    csg,
    /** First block type. */
    first = fe,
    last = csg,
};

/**
 * Block state during the counters sample time.
 *
 * @note If no bits are set, the block is in unknown state,
 * backend_features::has_power_states, backend_features::has_vm_states
 * and backend_features::has_protection_states must be false.
 */
struct block_state {
    /** This block was powered on for at least some portion of the sample */
    uint32_t on : 1;

    /** This block was powered off for at least some portion of the sample */
    uint32_t off : 1;

    /** This block was available to this VM for at least some portion of the sample */
    uint32_t available : 1;

    /** This block was not available to this VM for at least some portion of the sample
     *  Note that no data is collected when the block is not available to the VM.
     */
    uint32_t unavailable : 1;

    /** This block was operating in "normal" (non-protected) mode for at least some portion of the sample */
    uint32_t normal : 1;

    /** This block was operating in "protected" mode for at least some portion of the sample.
     * Note that no data is collected when the block is in protected mode.
     */
    uint32_t protected_mode : 1;
};

/**
 * Hardware counters block metadata.
 *
 * A hardware counters sample is structured as an array of block. Each block has its own type
 * and index. A block type represent the hardware unit that these counters
 * were collected from. And the index is the instance number of this hardware block.
 */
struct block_metadata {
    /** Type of this block. */
    block_type type;

    /** Index of this block within the set of blocks of its type. */
    uint8_t index;

    /** Hardware counters set number this block stores. */
    prfcnt_set set;

    /** State of this block during the counters sampling time. */
    block_state state;

    /**
     * Hardware counters values array.
     *
     * The number of array elements is stored in @ref block_extents::counters_per_block.
     * The value types are in @ref block_extents::values_type.
     *
     * Raw pointer to the shared user-kernel memory.
     * The values here are only valid between @ref reader::get_sample @ref reader::put_sample calls.
     */
    const void *values;
};

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
