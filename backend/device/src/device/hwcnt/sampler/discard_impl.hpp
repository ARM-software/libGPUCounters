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

/** @file discard_impl.hpp Discard function implementation. */

#pragma once

#include "poll.hpp"

#include <device/hwcnt/sample.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {

/**
 * Default `reader::discard` implementation.
 *
 * @param backend            Back-end instance.
 * @param syscall_iface      Syscall interface.
 * @param timestamp_iface    Timestam interface.
 * @return Error code.
 */
template <typename backend_t, typename syscall_iface_t, typename timestamp_iface_t>
std::error_code discard_impl(backend_t &backend, syscall_iface_t &&syscall_iface, timestamp_iface_t &&timestamp_iface) {
    const auto now = timestamp_iface.clock_gettime();
    for (;;) {
        std::error_code ec;
        bool ready_read{false};

        std::tie(ec, ready_read) = check_ready_read(backend.get_fd(), syscall_iface);
        if (ec)
            return ec;

        if (!ready_read)
            break;

        sample_metadata sm{};
        sample_handle sample_hndl{};

        ec = backend.get_sample(sm, sample_hndl);
        if (ec)
            return ec;

        ec = backend.put_sample(sample_hndl);
        if (ec)
            return ec;

        /* If samples are produced faster than discarded, this condition
         * prevents from infinite looping.
         */
        if (sm.timestamp_ns_end >= now)
            break;
    }

    return {};
}

} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
