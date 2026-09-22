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

#include "detail/get_warp_width.hpp"
#include "device/hwcnt/features.hpp"
#include "device/instance_backend_type.hpp"
#include "device/ioctl/panthor/commands.hpp"
#include "device/ioctl/panthor/types.hpp"

#include <device/constants.hpp>
#include <device/hwcnt/backend_type.hpp>
#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sampler/manual.hpp>
#include <device/hwcnt/sampler/panthor/construct_block_extents.hpp>
#include <device/instance.hpp>
#include <device/kbase_version.hpp>
#include <device/num_exec_engines.hpp>
#include <device/product_id.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <system_error>
#include <tuple>
#include <vector>

namespace hwcpipe {
namespace device {

using properties_type = std::vector<unsigned char>;

/** Mali device driver instance implementation. */
template <typename syscall_iface_t>
class instance_impl_panthor : public instance_backend_type, private syscall_iface_t {
    using driver_version_type = ::hwcpipe::device::kbase_version;

  public:
    instance_impl_panthor(int fd, std::error_code &ec, const syscall_iface_t &iface = {})
        : syscall_iface_t(iface)
        , fd_(fd) {
        ec = init();
        valid_ = !ec;

        if (valid_) {
            HWCPIPE_LOG_INFO("Instance initialized with data: %s", get_description().c_str());
            return;
        }

        HWCPIPE_LOG_ERROR("Instance failed to initialize with data: %s", get_description().c_str());
    }

    instance_impl_panthor(instance_impl_panthor<syscall_iface_t> const &o) = delete;
    instance_impl_panthor &operator=(instance_impl_panthor<syscall_iface_t> const &o) = delete;
    ~instance_impl_panthor() override = default;

    constants get_constants() const override {
        /* The constants must have been initialized. */
        assert(constants_.gpu_id != 0);
        return constants_;
    }

    hwcnt::block_extents get_hwcnt_block_extents() const override {
        /* The block extents must have been initialized. */
        assert(block_extents_.num_blocks());
        return block_extents_;
    }

    product_id get_product_id() const { return pid_; }

    /**
     * Check if instance_impl is valid.
     *
     * @return True if valid, false otherwise.
     */
    bool valid() const { return valid_; }

    /**
     * Get file descriptor.
     *
     * @return The file descriptor for this instance_impl.
     */
    int fd() const { return fd_; }

    struct buffer_sizes {
        size_t sample_header_size;
        size_t block_header_size;
        size_t sample_size;
    };

    const buffer_sizes &get_buffer_sizes() const { return buffer_sizes_; }

    const hwcnt::features &get_features() const { return features_; }

  private:
    /** @return Syscall iface reference. */
    syscall_iface_t &get_syscall_iface() { return *this; }

    /** Initialize constants_ field. */
    std::error_code init_constants() {
        std::error_code ec;

        ioctl::panthor::dev_query query{};

        query.type = ioctl::panthor::dev_query_type::gpu_info;
        std::tie(ec, std::ignore) = get_syscall_iface().ioctl(fd_, ioctl::panthor::command::dev_query, &query);
        if (ec)
            return ec;

        ioctl::panthor::gpu_info gpu_info{};
        query.size = std::min(static_cast<uint32_t>(sizeof(gpu_info)), query.size);
        query.pointer.u_gpu_info = &gpu_info;
        std::tie(ec, std::ignore) = get_syscall_iface().ioctl(fd_, ioctl::panthor::command::dev_query, &query);
        if (ec)
            return ec;

        constants_.gpu_id = ((uint64_t)gpu_info.gpu_id_hi << 32) | gpu_info.gpu_id;
        std::tie(ec, pid_) = product_id_from_raw_gpu_id(constants_.gpu_id);
        if (ec)
            return ec;

        constants_.warp_width = detail::get_warp_width(pid_, ec);
        if (ec)
            return ec;

        constants_.l2_slice_size = 1UL << ((gpu_info.l2_features >> 16) & 0xFF);
        constants_.num_l2_slices = 1UL + ((gpu_info.mem_features >> 8) & 0xF);
        constants_.axi_bus_width = 1UL << ((gpu_info.l2_features >> 24) & 0xFF);

        constants_.shader_core_mask = gpu_info.shader_present;
        constants_.num_shader_cores = static_cast<uint64_t>(__builtin_popcountll(constants_.shader_core_mask));

        constants_.tile_size = 16;

        constants_.fw_version = 1;

        get_num_exec_engines_args args{};
        args.known_pid = pid_;
        args.core_count = constants_.num_shader_cores;
        args.core_features = gpu_info.core_features;
        args.thread_features = gpu_info.thread_features;

        constants_.num_exec_engines = get_num_exec_engines(std::move(args), ec);

        return ec;
    }

    /** Detect backend interface type and initialize backend_type_ field. */
    std::error_code backend_type_probe() {
        std::error_code ec;
        auto available_types = hwcnt::backend_type_discover(panthor_version_, pid_);

        std::tie(ec, this->backend_type_) = hwcnt::backend_type_select(available_types);
        return ec;
    }

    hwcnt::features parse_features(const ioctl::panthor::perf_info &info) {
        using feat_flags_type = ioctl::panthor::perf_info::perf_feat_flags;
        using features = hwcnt::features;

        features result{};

        result.has_gpu_cycle = true;
        result.has_stretched_flag = true;

        if (!!(info.flags & feat_flags_type::block_states_support)) {
            result.has_vm_states = true;
            result.has_power_states = true;
            result.has_protection_states = true;
        }

        if (pid_ == product_id::g310 || pid_ == product_id::g510 || pid_ == product_id::g710)
            result.overflow_behavior_defined = false;
        else
            result.overflow_behavior_defined = true;

        return result;
    }

    buffer_sizes parse_header_sizes(const ioctl::panthor::perf_info &info) {
        buffer_sizes result{};

        result.block_header_size = info.block_header_size;
        result.sample_header_size = info.sample_header_size;
        result.sample_size = info.sample_size;

        return result;
    }

    hwcnt::clock_extents get_hwcnt_clock_extents() const override { return clock_extents_; }

    /**
     * Initialize `block_extents_` field.
     *
     * @param[in] iface Syscall iface.
     * @return Error code.
     */
    std::error_code init_info() {
        std::error_code ec;
        ioctl::panthor::dev_query query{};

        assert(this->backend_type_ == hwcnt::backend_type::panthor);

        query.type = ioctl::panthor::dev_query_type::perf_info;
        std::tie(ec, std::ignore) = get_syscall_iface().ioctl(fd_, ioctl::panthor::command::dev_query, &query);
        if (ec)
            return ec;

        ioctl::panthor::perf_info perf_info{};
        query.size = std::min(static_cast<uint32_t>(sizeof(perf_info)), query.size);
        query.pointer.u_perf_info = &perf_info;
        std::tie(ec, std::ignore) = get_syscall_iface().ioctl(fd_, ioctl::panthor::command::dev_query, &query);
        if (ec)
            return ec;

        this->block_extents_ = hwcnt::sampler::panthor::construct_block_extents(perf_info);
        this->clock_extents_ = hwcnt::sampler::panthor::construct_clock_extents(perf_info);
        this->features_ = parse_features(perf_info);
        this->buffer_sizes_ = parse_header_sizes(perf_info);

        return ec;
    }

    /** Initialise device info. */
    std::error_code init() {
        std::error_code ec;

        panthor_version_ = driver_version_type(0, 0, ioctl_iface_type::panthor);

        ec = init_constants();
        if (ec)
            return ec;

        ec = backend_type_probe();
        if (ec)
            return ec;

        ec = init_info();
        if (ec)
            return ec;

        return {};
    }

    std::string get_description() const {
        return "Product:{" + product_id_name(pid_) +
               "}\nBackend:{Panthor}\nConstants:" + hwcpipe::device::get_constants_str(constants_) +
               "\nBlock extents: (" + std::string(block_extents_) + ")\nClock extents: (" +
               std::string(clock_extents_) + ")";
    }

    buffer_sizes buffer_sizes_{};
    constants constants_{};
    hwcnt::block_extents block_extents_{};
    hwcnt::clock_extents clock_extents_{};
    driver_version_type panthor_version_{};
    product_id pid_{};
    hwcnt::features features_{};

    bool valid_{true};
    int fd_;
};

} // namespace device
} // namespace hwcpipe
