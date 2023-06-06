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
 * Hardware counters sampler configuration header.
 */

#pragma once

#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/prfcnt_set.hpp>

#include <bitset>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {

/**
 * Per-block counters configuration.
 */
struct configuration {
    /** Maximum number of hardware counters per block. */
    static constexpr const size_t max_counters_per_block{128};

    /** Type used to represent counter numbers bitmask. */
    using enable_map_type = std::bitset<max_counters_per_block>;

    /** Block type. */
    block_type type{};

    /** Performance counters set to activate for this block type. */
    prfcnt_set set{};

    /** Bitmask of counters numbers to enable for this block type. */
    enable_map_type enable_map{};
};

} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
