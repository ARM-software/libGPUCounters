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

/** @file backend.hpp base::backend class. */

#pragma once

#include "backend_args.hpp"

#include <device/hwcnt/reader.hpp>
#include <device/hwcnt/sampler/detail/backend.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace base {

/** Base class for sampler backend / reader implementations. */
template <typename syscall_iface_t>
class backend : public detail::backend, public reader, private syscall_iface_t {
  public:
    reader &get_reader() override { return *this; }

  protected:
    /** Syscall interface type. */
    using syscall_iface_type = syscall_iface_t;
    /** Back-end args type. */
    using args_type = backend_args<syscall_iface_t>;
    /** Counters memory type. */
    using memory_type = typename args_type::memory_type;

    /** Constructor. */
    backend(args_type &&args, const syscall_iface_t &syscall_iface)
        : detail::backend()
        , reader(args.fd.release(), args.features_v, args.extents)
        , syscall_iface_t(syscall_iface)
        , period_ns_(args.period_ns)
        , memory_(std::move(args.memory)) {}

    ~backend() override { get_syscall_iface().close(fd_); }

    /** Sampler type. */
    enum class sampler_type {
        /** Manual sampler. */
        manual,
        /** Periodic sampler. */
        periodic,
    };

    /** @return sampler type. */
    sampler_type sampler_type() const { return period_ns_ ? sampler_type::periodic : sampler_type::manual; }

    /** @return syscall iface reference. */
    syscall_iface_type &get_syscall_iface() { return *this; }

    /** Sampling period. 0 for manual sampling. */
    const uint64_t period_ns_{};
    /** Counters memory. */
    memory_type memory_;
};

} // namespace base
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
