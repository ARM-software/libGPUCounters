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

/** @file setup.hpp @c kinstr_prfcnt::backend setup routine. */

#pragma once

#include "backend_args.hpp"
#include "convert.hpp"
#include "enum_info_parser.hpp"

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
 * @param[in] period_ns    Sampling period.
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
 * @param[in]  period_ns Sampling period (nanoseconds).
 * @param[in]  begin     Configuration begin.
 * @param[in]  end       Configuration end.
 * @param[out] out       Output iterator for the request items.
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
 * @param[in] device_fd    Device file descriptor.
 * @param[in] begin        Request items begin.
 * @param[in] end          Request items end.
 * @param[in] iface        System calls interface to use (unit tests only).
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
 * @param[in] ei           Enum info.
 * @param[in] kbase_ver    Current kbase version.
 * @return features structure initialized.
 */
inline auto init_features(const enum_info &ei, kbase_version kbase_ver) {
    static const kbase_version min_kbase_ver_block_state_jm(11, 41, ioctl_iface_type::jm_post_r21);
    static const kbase_version min_kbase_ver_block_state_csf(1, 23, ioctl_iface_type::csf);

    features result{};

    if (kbase_ver.type() == ioctl_iface_type::csf && kbase_ver >= min_kbase_ver_block_state_csf) {
        result.has_power_states = true;
        result.has_vm_states = false;
        result.has_protection_states = true;
    } else if (kbase_ver.type() == ioctl_iface_type::jm_post_r21 && kbase_ver >= min_kbase_ver_block_state_jm) {
        result.has_power_states = true;
        result.has_vm_states = true;
        result.has_protection_states = false;
    } else {
        assert(kbase_ver.type() != ioctl_iface_type::jm_pre_r21);
    }
    result.has_gpu_cycle = ei.has_cycles_top;
    result.has_stretched_flag = true;        // always true
    result.overflow_behavior_defined = true; // always true, on tODx under some circumstances false

    return result;
}
} // namespace detail

/**
 * Setup kisntr_prfcnt hardware counters.
 *
 * @param[in]     instance  Mali device instance.
 * @param[in]     period_ns Period in nanoseconds between samples taken. Zero for manual context.
 * @param[in]     begin     Counters configuration begin iterator.
 * @param[in]     end       Counters configuration end iterator.
 * @param[in,out] iface     System calls interface to use (unit tests only).
 *
 * @return A pair of error code and `backend_args` structure.
 */
template <typename instance_t, typename syscall_iface_t = syscall::iface>
auto setup(const instance_t &instance, uint64_t period_ns, const configuration *begin, const configuration *end,
           syscall_iface_t &&iface = {}) {
    using ioctl::kinstr_prfcnt::request_item;

    using syscall_iface_type = std::remove_reference_t<syscall_iface_t>;
    using args_type = backend_args<syscall_iface_type>;
    args_type result{};
    std::error_code ec{};

    block_extents extents{};
    std::tie(ec, extents) = filter_block_extents(instance.get_hwcnt_block_extents(), begin, end);
    if (ec)
        return std::make_pair(ec, std::move(result));

    const auto ei = instance.get_enum_info();

    // There can be at most 1 + `block_extents::num_block_types` + 1 request items.
    static constexpr size_t max_request_items = block_extents::num_block_types + 2;
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
    result.base_args.features_v = detail::init_features(ei, instance.kbase_version());
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
