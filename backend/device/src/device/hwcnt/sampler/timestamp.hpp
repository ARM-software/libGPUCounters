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

/**
 * @file timestamp.hpp
 *
 * Timestamp querying interface.
 */

#pragma once

#include <device/detail/is_empty_class.hpp>

#include <cstdint>
#include <ctime>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {

/** Timestamp querying interface. */
class timestamp_iface {
  public:
    /** @return clock monotonic raw, if supported, or clock monotonic timestamp. */
    static uint64_t clock_gettime() {
        timespec now{};
#if defined(CLOCK_MONOTONIC_RAW)
        ::clock_gettime(CLOCK_MONOTONIC_RAW, &now);
#elif defined(CLOCK_MONOTONIC)
        ::clock_gettime(CLOCK_MONOTONIC, &now);
#else
#error "No clock id defined"
#endif
        constexpr uint64_t nsec_per_sec = 1000000000;

        return static_cast<uint64_t>(now.tv_sec) * nsec_per_sec + static_cast<uint64_t>(now.tv_nsec);
    }
};

static_assert(::hwcpipe::device::detail::is_empty_class<timestamp_iface>::value,
              "timestamp_iface must be an empty class.");

} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
