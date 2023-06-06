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

/** @file setup.hpp `kinstr_prfcnt::backend` setup routine. */

#pragma once

#include "backend_args.hpp"
#include "convert.hpp"
#include "enum_info.hpp"

#include <device/hwcnt/backend_type.hpp>
#include <device/hwcnt/sampler/filefd_guard.hpp>
#include <device/hwcnt/sampler/filter_block_extents.hpp>
#include <device/hwcnt/sampler/mapped_memory.hpp>
#include <device/ioctl/kbase/commands.hpp>
#include <device/ioctl/kinstr_prfcnt/types.hpp>
#include <device/syscall/iface.hpp>

#include <iterator>
#include <numeric>
#include <system_error>
#include <vector>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {
namespace detail {

/**
 * Construct request mode structure.
 *
 * @param period_ns[in]    Sampling period.
 * @return request mode structure constructed.
 */
inline auto request_mode(uint64_t period_ns) {
    using ioctl::kinstr_prfcnt::request_item;

    request_item result{};

    result.hdr.type = request_item::item_type::mode;
    result.hdr.item_version = ioctl::kinstr_prfcnt::api_version;

    if (period_ns == 0) {
        result.u.req_mode.mode = request_item::request_mode::sampling_mode::manual;
    } else {
        result.u.req_mode.mode = request_item::request_mode::sampling_mode::periodic;
        result.u.req_mode.mode_config.periodic.period_ns = period_ns;
    }

    return result;
}

/**
 * Fill request array for `ioctl::kbase::command::kinstr_prfcnt_setup` ioctl.
 *
 * @param period_ns[in] Sampling period (nanoseconds).
 * @param begin[in]     Configuration begin.
 * @param end[in]       Configuration end.
 * @param out[out]      Output iterator for the request items.
 * @return End iterator for the output request items.
 */
template <typename input_iterator_t, typename output_iterator_t>
inline auto fill_request(uint64_t period_ns, input_iterator_t begin, input_iterator_t end, output_iterator_t out) {
    using ioctl::kinstr_prfcnt::request_item;

    *(out++) = request_mode(period_ns);

    for (auto it = begin; it != end; ++it)
        *(out++) = convert(*it);

    *(out++) = request_item{};

    return out;
}

/**
 * Call `ioctl::kbase::command::kinstr_prfcnt_setup` ioctl.
 *
 * @param device_fd[in]    Device file descriptor.
 * @param begin[in]        Request items begin.
 * @param end[in]          Request items end.
 * @return Tuple of error code, kinstr_prfcnt file descriptor, metadata item size and mapping size.
 */
template <typename syscall_iface_t>
inline auto invoke_request(int device_fd, ioctl::kinstr_prfcnt::request_item *begin,
                           ioctl::kinstr_prfcnt::request_item *end, syscall_iface_t &&iface = {}) {
    assert(std::distance(begin, end) > 0);

    ioctl::kbase::kinstr_prfcnt_setup setup_arg{};
    setup_arg.in.request_item_count = static_cast<uint32_t>(std::distance(begin, end));
    setup_arg.in.request_item_size = sizeof(*begin);
    setup_arg.in.requests_ptr.reset(begin);

    std::error_code ec;
    int kinstr_prfcnt_fd = -1;
    std::tie(ec, kinstr_prfcnt_fd) = iface.ioctl(device_fd, ioctl::kbase::command::kinstr_prfcnt_setup, &setup_arg);

    return std::make_tuple(ec, kinstr_prfcnt_fd, setup_arg.out.prfcnt_metadata_item_size,
                           setup_arg.out.prfcnt_mmap_size_bytes);
}

/**
 * Init features structure.
 *
 * @param ei[in]    Enum info.
 * @return features structure initialized.
 */
inline auto init_features(const enum_info &ei) {
    features result{};

    result.has_gpu_cycle = ei.has_cycles_top;
    result.has_block_state = false;          // always false
    result.has_stretched_flag = true;        // always true
    result.overflow_behavior_defined = true; // always true, on tODx under some circumstances false

    return result;
}

class setup_helper : public block_extents_filter, public enum_info_parser {};
} // namespace detail

/**
 * Setup kisntr_prfcnt hardware counters.
 *
 * @param[in] inst         Mali device instance.
 * @param[in] period_ns    Period in nanoseconds between samples taken. Zero for manual context.
 * @param begin[in]        Counters configuration begin iterator.
 * @param end[in]          Counters configuration end iterator.
 * @param iface[in,out]    System calls interface to use (unit tests only).
 * @param helper[in,out]   Setup helper (unit tests only).
 *
 * @return A pair of error code and `backend_args` structure.
 */
template <typename instance_t, typename syscall_iface_t = syscall::iface,
          typename setup_helper_t = detail::setup_helper>
auto setup(const instance_t &instance, uint64_t period_ns, const configuration *begin, const configuration *end,
           syscall_iface_t &&iface = {}, setup_helper_t &&helper = {}) {
    using ioctl::kinstr_prfcnt::request_item;

    using syscall_iface_type = std::remove_reference_t<syscall_iface_t>;
    using args_type = backend_args<syscall_iface_type>;
    args_type result{};
    std::error_code ec{};

    block_extents extents{};
    std::tie(ec, extents) = helper.filter_block_extents(instance, begin, end);
    if (ec)
        return std::make_pair(ec, std::move(result));

    enum_info ei{instance.get_enum_info()};

    // There can be at most 1 + 4 + 1 request items.
    static constexpr size_t max_request_items = 6;
    std::array<request_item, max_request_items> request_items{};
    const auto request_items_end = detail::fill_request(period_ns, begin, end, request_items.begin());

    int kinstr_prfcnt_fd = -1;
    uint32_t metadata_size = 0;
    uint32_t mmap_size = 0;

    std::tie(ec, kinstr_prfcnt_fd, metadata_size, mmap_size) =
        detail::invoke_request(instance.fd(), request_items.begin(), request_items_end, iface);

    if (ec)
        return std::make_pair(ec, std::move(result));

    filefd_guard<syscall_iface_type> guard_kinstr_prfcnt_fd(kinstr_prfcnt_fd, iface);

    using memory_type = typename args_type::memory_type;
    memory_type memory{kinstr_prfcnt_fd, mmap_size, ec, iface};
    if (ec)
        return std::make_pair(ec, std::move(result));

    result.base_args.fd = std::move(guard_kinstr_prfcnt_fd);
    result.base_args.period_ns = period_ns;
    result.base_args.features_v = detail::init_features(ei);
    result.base_args.extents = extents;
    result.base_args.memory = std::move(memory);

    result.sc_mask = instance.get_constants().shader_core_mask;
    result.metadata_item_size = metadata_size;

    return std::make_pair(ec, std::move(result));
}
} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
