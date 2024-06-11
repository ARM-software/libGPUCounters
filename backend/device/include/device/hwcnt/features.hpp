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
 * Hardware counters features header.
 */

#pragma once

#include <cstdint>

namespace hwcpipe {
namespace device {
namespace hwcnt {

/**
 *  Features that the kernel-side hardware counters infrastructure supports.
 *
 * The device library supports different versions of the hardware
 * counters ioctl interface. Some of them provide certain counters
 * annotation meta-data, and some of them don't. This structure lists
 * which features are supported.
 *
 * @par Example
 * @code
 * // Check the feature before accessing gpu_cycle
 * if (reader.get_features().has_gpu_cycle)
 *     printf("GPU timestamp = %lu\n", sample.get_metadata().gpu_cycle);
 * else
 *     printf("No GPU Timestamp data!\n");
 * @endcode
 */
struct features {
    /**
     * True if HWC samples are annotated with the number of
     * GPU and shader cores cycles since the last sample.
     *
     * When true, @ref sample_metadata::gpu_cycle and @ref sample_metadata::sc_cycle
     * values are set.
     */
    bool has_gpu_cycle;

    /** True if @ref block_metadata::state power values are set. */
    bool has_power_states;

    /** True if @ref block_metadata::state availability values are set. */
    bool has_vm_states;

    /** True if @ref block_metadata::state protected values are set. */
    bool has_protection_states;

    /**
     * True if hardware counters back-end can detect sample period stretched due to
     * the counters ring buffer overflow. If true, @ref sample_flags::stretched
     * value is meaningful.
     */
    bool has_stretched_flag;

    /**
     * True if overflow behavior is defined.
     *
     * The defined overflow behavior is:
     * When a counter reaches its maximum, the value will saturate when it is incremented,
     * i.e. they will stay maximum until sampled.
     */
    bool overflow_behavior_defined;
};

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
