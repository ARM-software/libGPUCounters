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
 * Performance counters set header.
 */
#pragma once

#include <cstdint>

namespace hwcpipe {
namespace device {
namespace hwcnt {

/**
 *  Performance counters set.
 *
 * The hardware has limited number of registers to accumulate hardware counters
 * values. On the other hand, the number of performance counters exceeds the number
 * of registers. Therefore we cannot collect all the performance counters all together.
 * There are a few different disjoint sets of counters that can be collected
 * together in a profiling session.
 */
enum class prfcnt_set : uint8_t {
    primary,   /**< Primary. */
    secondary, /**< Secondary. */
    tertiary,  /**< Tertiary. */
};

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
