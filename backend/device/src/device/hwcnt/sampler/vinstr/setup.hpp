/*
 * Copyright (c) 2022-2024 Arm Limited.
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

#include <device/hwcnt/backend_type.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sampler/filefd_guard.hpp>
#include <device/hwcnt/sampler/filter_block_extents.hpp>
#include <device/hwcnt/sampler/mapped_memory.hpp>
#include <device/hwcnt/sampler/vinstr/sample_layout.hpp>
#include <device/ioctl/kbase/commands.hpp>
#include <device/ioctl/kbase/types.hpp>
#include <device/ioctl/kbase_pre_r21/commands.hpp>
#include <device/ioctl/kbase_pre_r21/types.hpp>
#include <device/ioctl/vinstr/commands.hpp>
#include <device/syscall/iface.hpp>

#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace vinstr {

namespace detail {
inline auto init_features(ioctl::vinstr::reader_features reader_features) {
    features result{};

    result.has_gpu_cycle = !!reader_features;
    result.has_stretched_flag = false;
    result.overflow_behavior_defined = true;

    return result;
}

/**
 * Setup hardware counters reader handle.
 *
 * @param[in]     instance      Mali device instance.
 * @param[in]     setup_args    Ioctl setup arguments.
 * @param[in,out] iface         System calls interface to use (unit tests only).
 *
 * @return A pair of error code and the reader handle..
 */
template <typename instance_t, typename syscall_iface_t>
inline auto reader_setup(const instance_t &instance, ioctl::kbase::hwcnt_reader_setup &setup_args,
                         syscall_iface_t &&iface) {
    const auto version = instance.kbase_version();

    if (version.type() != ioctl_iface_type::jm_pre_r21) {
        return iface.ioctl(instance.fd(), ioctl::kbase::command::hwcnt_reader_setup, &setup_args);
    }

    ioctl::kbase_pre_r21::uk_header header = {
        ioctl::kbase_pre_r21::header_id::hwcnt_reader_setup, //
    };

    ioctl::kbase_pre_r21::uk_hwcnt_reader_setup setup_args_pre_r21 = {
        header,                  //
        setup_args.buffer_count, //
        setup_args.fe_bm,        //
        setup_args.shader_bm,    //
        setup_args.tiler_bm,     //
        setup_args.mmu_l2_bm,    //
        -1,
    };

    std::error_code ec;

    std::tie(ec, std::ignore) =
        iface.ioctl(instance.fd(), ioctl::kbase_pre_r21::command::hwcnt_reader_setup, &setup_args_pre_r21);

    return std::make_pair(ec, setup_args_pre_r21.fd);
}

} // namespace detail

/**
 * Setup vinstr hardware counters.
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
    backend_args_type result{};

    block_extents extents{};
    std::tie(ec, extents) = filter_block_extents(instance.get_hwcnt_block_extents(), begin, end);
    if (ec)
        return std::make_pair(ec, std::move(result));

    ioctl::kbase::hwcnt_reader_setup setup_args{};

    std::tie(ec, setup_args) = convert(begin, end);

    if (ec)
        return std::make_pair(ec, std::move(result));

    setup_args.buffer_count = backend_args_type::max_buffer_count;

    int vinstr_fd = -1;

    /* Try to initialize vinstr reader with `max_buffer_count` buffers or fewer. */
    for (; setup_args.buffer_count > 1; setup_args.buffer_count >>= 1) {
        std::tie(ec, vinstr_fd) = detail::reader_setup(instance, setup_args, iface);

        if (ec != std::errc::not_enough_memory)
            break;
    }

    if (ec)
        return std::make_pair(ec, std::move(result));

    filefd_guard<syscall_iface_type> vinstr_fd_guard{vinstr_fd, iface};

    ioctl::vinstr::reader_api_version api_version{};

    std::tie(ec, std::ignore) =
        iface.ioctl(vinstr_fd, ioctl::vinstr::command::get_api_version_with_features, &api_version);

    if (ec) {
        api_version = {};
        std::tie(ec, std::ignore) =
            iface.ioctl(vinstr_fd, ioctl::vinstr::command::get_api_version, &api_version.version);
    }

    if (ec)
        return std::make_pair(ec, std::move(result));

    uint32_t buffer_size = 0;
    std::tie(ec, std::ignore) = iface.ioctl(vinstr_fd, ioctl::vinstr::command::get_buffer_size, &buffer_size);

    if (ec)
        return std::make_pair(ec, std::move(result));

    const size_t mapping_size = buffer_size * static_cast<size_t>(setup_args.buffer_count);
    typename backend_args_type::memory_type memory{vinstr_fd, mapping_size, ec, iface};

    if (ec)
        return std::make_pair(ec, std::move(result));

    const auto consts = instance.get_constants();

    product_id pid = instance.get_product_id();
    const auto sample_layout_type = (is_v4_layout(pid)) ? sample_layout_type::v4 : sample_layout_type::non_v4;

    result.sample_layout_v = sample_layout(extents, consts.num_l2_slices, consts.shader_core_mask, sample_layout_type);
    result.base_args.fd = std::move(vinstr_fd_guard);
    result.base_args.period_ns = period_ns;
    result.base_args.features_v = detail::init_features(api_version.features);
    result.base_args.extents = extents;
    result.base_args.memory = std::move(memory);

    result.features = api_version.features;
    result.buffer_size = buffer_size;

    return std::make_pair(std::error_code{}, std::move(result));
}

} // namespace vinstr
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
