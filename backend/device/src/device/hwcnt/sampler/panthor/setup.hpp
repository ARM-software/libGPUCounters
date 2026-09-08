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

#include "backend_args.hpp"
#include "convert.hpp"
#include "device/hwcnt/sampler/panthor/sample_layout.hpp"
#include "device/ioctl/panthor/types.hpp"

#include <device/hwcnt/backend_type.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sampler/filefd_guard.hpp>
#include <device/hwcnt/sampler/filter_block_extents.hpp>
#include <device/hwcnt/sampler/gem_guard.hpp>
#include <device/hwcnt/sampler/mapped_memory.hpp>
#include <device/ioctl/panthor/commands.hpp>
#include <device/ioctl/panthor/print.hpp>
#include <device/product_id.hpp>
#include <device/syscall/iface.hpp>

#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>

#include <sys/eventfd.h>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace panthor {
namespace detail {

template <typename memory_t, typename syscall_iface_t = syscall::iface>
inline auto create_mapped_bo(const int fd, size_t mapping_size, syscall_iface_t &&iface = {}, int flags = PROT_READ) {
    using syscall_iface_type = std::remove_reference_t<syscall_iface_t>;

    std::error_code ec{};
    gem_guard<syscall_iface_type> bo_guard;
    memory_t mem;
    ioctl::panthor::bo_create bo{};
    bo.size = mapping_size;
    bo.exclusive_vm_id = 0;

    std::tie(ec, std::ignore) = iface.ioctl(fd, ioctl::panthor::command::bo_create, &bo);
    if (ec)
        return std::make_tuple(ec, std::move(bo_guard), std::move(mem));

    bo_guard = gem_guard<syscall_iface_type>{fd, bo.handle};

    ioctl::panthor::bo_mmap_offset offset{};
    offset.handle = bo.handle;
    std::tie(ec, std::ignore) = iface.ioctl(fd, ioctl::panthor::command::bo_mmap_offset, &offset);
    if (ec)
        return std::make_tuple(ec, std::move(bo_guard), std::move(mem));

    mem = memory_t{fd, static_cast<size_t>(bo.size), static_cast<off_t>(offset.offset), ec, iface, flags};

    return std::make_tuple(ec, std::move(bo_guard), std::move(mem));
}

} // namespace detail

/**
 * Setup panthor hardware counters.
 *
 * @param[in]     instance     Mali device instance.
 * @param[in]     period_ns    Period in nanoseconds between samples taken. Zero for manual context.
 * @param[in]     begin        Counters configuration begin iterator.
 * @param[in]     end          Counters configuration end iterator.
 * @param[in,out] iface        System calls interface to use (unit tests only).
 *
 * @return A pair of error code and `backend_args` structure.
 */
template <typename instance_t, typename syscall_iface_t = syscall::iface>
auto setup(const instance_t &instance, uint64_t period_ns, const configuration *begin, const configuration *end,
           syscall_iface_t &&iface = {}) {
    std::error_code ec;

    using syscall_iface_type = std::remove_reference_t<syscall_iface_t>;
    using backend_args_type = backend_args<syscall_iface_type>;
    using memory_type = typename backend_args_type::memory_type;

    backend_args_type result{};
    gem_guard<syscall_iface_type> ringbuffer_guard{};
    gem_guard<syscall_iface_type> control_guard{};
    memory_type ringbuffer_mem;
    memory_type control_mem;

    block_extents extents{};
    std::tie(ec, extents) = filter_block_extents(instance.get_hwcnt_block_extents(), begin, end);
    if (ec)
        return std::make_pair(ec, std::move(result));

    auto buffer_sizes = instance.get_buffer_sizes();

    /* Sample layout in Panthor depends on the available blocks, even disabled ones. */
    sample_layout layout = sample_layout(instance.get_hwcnt_block_extents(), buffer_sizes.sample_header_size,
                                         buffer_sizes.block_header_size);

    if (layout.sample_size() != buffer_sizes.sample_size) {
        ec = HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::invalid_sample, "Sample size mismatch: kernel=%zu calculated=%zu",
                                     buffer_sizes.sample_size, layout.sample_size());
        return std::make_pair(ec, std::move(result));
    }

    int panthor_fd = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
    if (panthor_fd < 0)
        return std::make_pair(std::error_code(errno, std::system_category()), std::move(result));

    int samples_fd = -1;
    filefd_guard<syscall_iface_type> samples_fd_guard;
    if (!period_ns) {
        samples_fd = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
        if (samples_fd < 0) {
            ec = HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::backend_fd_error, "Failed to create user-exposed fd (errno=%d)",
                                         errno);
            return std::make_pair(ec, std::move(result));
        }

        samples_fd_guard = filefd_guard<syscall_iface_type>{samples_fd, std::forward<syscall_iface_t>(iface)};
    }

    filefd_guard<syscall_iface_type> panthor_fd_guard{panthor_fd, std::forward<syscall_iface_t>(iface)};

    const size_t sample_size = buffer_sizes.sample_size;
    const size_t mapping_size = sample_size * result.slots;

    std::tie(ec, ringbuffer_guard, ringbuffer_mem) =
        detail::create_mapped_bo<memory_type>(instance.fd(), mapping_size, iface);
    if (ec)
        return std::make_pair(ec, std::move(result));

    std::tie(ec, control_guard, control_mem) = detail::create_mapped_bo<memory_type>(
        instance.fd(), sizeof(struct ioctl::panthor::perf_ringbuf_control), iface, PROT_READ | PROT_WRITE);
    if (ec)
        return std::make_pair(ec, std::move(result));

    ioctl::panthor::perf_cmd_setup setup{};
    std::tie(ec, setup) = convert(begin, end);
    if (ec)
        return std::make_pair(ec, std::move(result));

    setup.fd = static_cast<uint32_t>(panthor_fd);
    setup.sample_slots = backend_args_type::slots;
    setup.ringbuf_handle = ringbuffer_guard.get();
    setup.control_handle = control_guard.get();
    setup.control_offset = 0;
    setup.sample_freq_ns = period_ns;

    ioctl::panthor::perf_control setup_args{};
    setup_args.cmd = ioctl::panthor::perf_command::setup;
    setup_args.size = sizeof(ioctl::panthor::perf_cmd_setup);
    setup_args.pointer.u_perf_cmd_setup = &setup;

    int session_id{};
    std::tie(ec, session_id) = iface.ioctl(instance.fd(), ioctl::panthor::command::perf_control, &setup_args);
    if (ec)
        return std::make_pair(ec, std::move(result));

    /* In case of periodic sampler, use panthor_fd_guard for both user signaling and kernel-hwcpipe signaling */
    /* In case of manual sampler, use the extra samples_fd_guard for user signaling only */
    result.base_args.fd = period_ns ? std::move(panthor_fd_guard) : std::move(samples_fd_guard);
    result.base_args.period_ns = period_ns;
    result.base_args.features_v = std::move(instance.get_features());
    result.base_args.extents = std::move(extents);
    result.base_args.memory = std::move(ringbuffer_mem);
    result.sample_size = sample_size;
    result.ringbuffer_bo = std::move(ringbuffer_guard);
    result.control_idx_bo = std::move(control_guard);
    /* Instance fd, not equal to panthor_fd_guard */
    result.panthor_fd = std::move(filefd_guard<syscall_iface_type>(instance.fd()));
    result.sample_layout_v = std::move(layout);
    result.control_mem = std::move(control_mem);
    result.session_id = static_cast<uint32_t>(session_id);

    if (!period_ns)
        /* For manual samplers use panthor_fd_guard for kernel-hwcpipe signaling only */
        // NOLINTNEXTLINE(bugprone-use-after-move): moved above only when period_ns is true.
        result.manual_sync_fd = std::move(panthor_fd_guard);

    return std::make_pair(ec, std::move(result));
}

} // namespace panthor
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
