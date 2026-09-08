/*
 * Copyright (c) 2024-2026 Arm Limited.
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

#include "device/hwcnt/sampler/filefd_guard.hpp"

#include <device/hwcnt/sampler/base/backend_args.hpp>
#include <device/hwcnt/sampler/gem_guard.hpp>
#include <device/hwcnt/sampler/panthor/sample_layout.hpp>
#include <device/ioctl/kbase/types.hpp>
#include <device/ioctl/panthor/types.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace panthor {

/** Arguments for `panthor::backend` constructor. */
template <typename syscall_iface_t>
struct backend_args {
    /** Base args type. */
    using base_args_type = base::backend_args<syscall_iface_t>;
    /** Counters buffer memory type. */
    using memory_type = typename base_args_type::memory_type;
    /** GEM object guard type. */
    using gem_guard_type = gem_guard<syscall_iface_t>;
    /** File descriptor guard type. */
    using filefd_guard_type = filefd_guard<syscall_iface_t>;
    /** Arguments for `base::backend`. */
    base_args_type base_args;
    /** Size of a single sample. */
    size_t sample_size{0};
    /** Handle (guard) for the BO to write samples into. */
    gem_guard_type ringbuffer_bo{};
    /** Handle (guard) for the BO to read and write the insert and extract indices. */
    gem_guard_type control_idx_bo{};
    /** Memory mapping containing ring buffer insert and extract indices. */
    memory_type control_mem{};
    /** Sample layout to use. */
    sample_layout sample_layout_v;
    /** Eventfd guard */
    filefd_guard_type panthor_fd{};
    /** Panthor manual sampler sync Eventfd guard */
    filefd_guard_type manual_sync_fd{};
    /** Panthor session identifier. */
    uint32_t session_id{0};
    /** Number of slots in the user-visible ringbuffer. Must be a power of two. */
    static const constexpr uint32_t slots{32};
};

} // namespace panthor
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
