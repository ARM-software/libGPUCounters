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

#pragma once

#include <system_error>
#include <tuple>

#include <poll.h>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {

namespace detail {

/**
 * Poll file descriptor.
 *
 * @param[in] fd File descriptor to poll.
 * @param[in] timeout Polling timeout. -1 to wait forever.
 * @param[in] iface Syscall iface (testing only).
 * @return A pair of error code and boolean. The boolean is `true` if @p fd is ready.
 */
template <typename syscall_iface_t>
inline auto poll_fd(int fd, int timeout, syscall_iface_t &&iface = {}) {
    pollfd fds{};

    fds.fd = fd;
    fds.events = POLLIN;

    int nelems{};
    std::error_code ec;

    static constexpr nfds_t num_fds = 1;

    std::tie(ec, nelems) = iface.poll(&fds, num_fds, timeout);

    return std::make_pair(ec, nelems == num_fds);
}
} // namespace detail

/**
 * Wait for hardware counters sample.
 *
 * @param[in] fd    Hardware counters file descriptor.
 * @param[in] iface Syscall iface (testing only).
 * @return Error code.
 */
template <typename syscall_iface_t>
inline std::error_code wait_for_sample(int fd, syscall_iface_t &&iface = {}) {
    std::error_code ec;
    bool ready{};

    static constexpr int wait_forever = -1;

    std::tie(ec, ready) = detail::poll_fd(fd, wait_forever, std::forward<syscall_iface_t>(iface));

    if (ec)
        return ec;

    if (!ready)
        return std::make_error_code(std::errc::timed_out);

    return {};
}

/**
 * Check if a sample is ready to be read.
 *
 * @param[in] fd    Hardware counters file descriptor.
 * @param[in] iface Syscall iface (testing only).
 * @return A pair of error code and boolean. The boolean is `true` if @p fd is ready.
 */
template <typename syscall_iface_t>
inline auto check_ready_read(int fd, syscall_iface_t &&iface = {}) {
    static constexpr int no_wait = 0;

    return detail::poll_fd(fd, no_wait, std::forward<syscall_iface_t>(iface));
}

} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
