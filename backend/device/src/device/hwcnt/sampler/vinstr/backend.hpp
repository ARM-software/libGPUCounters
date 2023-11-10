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
 * @file backend.hpp
 *
 * VINSTR hardware counters sample back-end implementation.
 */

#pragma once

#include "backend_args.hpp"
#include "sample_layout.hpp"
#include "session.hpp"

#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/reader.hpp>
#include <device/hwcnt/sample.hpp>
#include <device/hwcnt/sampler/base/backend.hpp>
#include <device/hwcnt/sampler/detail/backend.hpp>
#include <device/hwcnt/sampler/discard_impl.hpp>
#include <device/hwcnt/sampler/poll.hpp>
#include <device/hwcnt/sampler/queue.hpp>
#include <device/hwcnt/sampler/timestamp.hpp>
#include <device/ioctl/offset_pointer.hpp>
#include <device/ioctl/vinstr/commands.hpp>
#include <device/ioctl/vinstr/types.hpp>

#include <cstring>
#include <mutex>
#include <tuple>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace vinstr {

/** HWC backend using the vinstr kernel ioctls. */
template <typename syscall_iface_t, typename timestamp_iface_t = timestamp_iface>
class backend : public base::backend<syscall_iface_t>, private timestamp_iface_t {
  public:
    using args_type = backend_args<syscall_iface_t>;

    backend(args_type &&args, const syscall_iface_t &syscall_iface = {}, const timestamp_iface_t &timestamp_iface = {})
        : base_type(std::move(args.base_args), syscall_iface)
        , timestamp_iface_t(timestamp_iface)
        , features_(args.features)
        , buffer_size_(args.buffer_size)
        , sample_layout_(args.sample_layout_v) {}

    std::error_code start(uint64_t user_data) override {
        std::lock_guard<std::mutex> lock(access_);

        if (active_)
            return {};

        std::error_code ec = clear();

        if (ec)
            return ec;

        const auto now = get_ts_iface().clock_gettime();

        if (sampler_type() == base_type::sampler_type::periodic) {
            std::tie(ec, std::ignore) =
                get_syscall_iface().ioctl(fd_, ioctl::vinstr::command::set_interval, period_ns_);

            if (ec)
                return ec;

            sampling_ = true;
        }

        sessions_.push({now, user_data});

        active_ = true;
        return {};
    }

    std::error_code stop(uint64_t user_data) override {
        std::lock_guard<std::mutex> lock(access_);

        if (!active_)
            return {};

        std::error_code ec;

        if (this->sampler_type() == base_type::sampler_type::periodic && sampling_) {
            std::tie(ec, std::ignore) = get_syscall_iface().ioctl(fd_, ioctl::vinstr::command::set_interval, 0);

            if (ec)
                return ec;

            sampling_ = false;
        }

        ec = request_sample_no_lock(user_data);
        if (ec)
            return ec;

        const auto stop_sample_nr = user_data_manual_.push_count();
        sessions_.back().stop(stop_sample_nr);

        active_ = false;
        return {};
    }

    std::error_code request_sample(uint64_t user_data) override {
        if (sampler_type() != base_type::sampler_type::manual)
            return std::make_error_code(std::errc::invalid_argument);

        std::lock_guard<std::mutex> lock(access_);

        return request_sample_no_lock(user_data);
    }

    std::error_code get_sample(sample_metadata &sm, sample_handle &sample_hndl) override {
        std::error_code ec = wait_for_sample(fd_, get_syscall_iface());

        if (ec)
            return ec;

        ioctl::vinstr::reader_metadata_with_cycles metadata{};

        if (!!features_) {
            std::tie(ec, std::ignore) =
                get_syscall_iface().ioctl(fd_, ioctl::vinstr::command::get_buffer_with_cycles, &metadata);
        } else {
            std::tie(ec, std::ignore) =
                get_syscall_iface().ioctl(fd_, ioctl::vinstr::command::get_buffer, &metadata.metadata);
        }

        if (ec)
            return ec;

        {
            const auto is_manual_sample = metadata.metadata.event_id == ioctl::vinstr::reader_event::manual;

            std::lock_guard<std::mutex> guard(access_);

            auto &session = sessions_.front();

            if (is_manual_sample)
                sm.user_data = user_data_manual_.pop();
            else
                sm.user_data = session.user_data_periodic();

            sm.flags = sample_flags{};

            sm.sample_nr = sample_nr_alloc_++;

            sm.timestamp_ns_begin = session.update_ts(metadata.metadata.timestamp);

            if (is_manual_sample) {
                const auto manual_sample_nr = user_data_manual_.pop_count();

                if (session.can_erase(manual_sample_nr))
                    sessions_.pop();
            }
        }

        sm.timestamp_ns_end = metadata.metadata.timestamp;

        if (!!(features_ & reader_features_type::cycles_top))
            sm.gpu_cycle = metadata.cycles.top;

        if (!!(features_ & reader_features_type::cycles_shader_core))
            sm.sc_cycle = metadata.cycles.shader_cores;

        if (!sm.sc_cycle && sm.gpu_cycle)
            sm.sc_cycle = sm.gpu_cycle;

        sample_hndl.get<sample_handle_type>() = metadata.metadata;

        return {};
    }

    bool next(sample_handle sample_hndl_raw, block_metadata &bm, block_handle &block_hndl_raw) const override {
        auto &sample_hndl = sample_hndl_raw.get<sample_handle_type>();
        auto &block_index = block_hndl_raw.get<size_t>();
        static_cast<void>(sample_hndl);

        if (block_index == sample_layout_.size())
            return false;

        const auto &layout_entry = sample_layout_[block_index];

        bm.type = layout_entry.type;
        bm.index = layout_entry.index;
        bm.set = prfcnt_set::primary;
        bm.state = {};

        bm.values = static_cast<const uint8_t *>(memory_.data()) //
                    + buffer_size_ * sample_hndl.buffer_idx      //
                    + layout_entry.offset;

        ++block_index;

        return true;
    }

    std::error_code put_sample(sample_handle sample_hndl_raw) override {
        std::error_code ec;

        auto &sample_hndl = sample_hndl_raw.get<sample_handle_type>();

        std::tie(ec, std::ignore) = get_syscall_iface().ioctl(fd_, ioctl::vinstr::command::put_buffer, &sample_hndl);

        return ec;
    }

    std::error_code discard() override { return discard_impl(*this, get_syscall_iface(), get_ts_iface()); }

  private:
    using base_type = base::backend<syscall_iface_t>;
    using base_type::block_extents_;
    using base_type::fd_;
    using base_type::get_syscall_iface;
    using base_type::memory_;
    using base_type::period_ns_;
    using base_type::sampler_type;

    /**
     * Request manual sample dump.
     *
     * @pre `access_` must be locked.
     *
     * @param[in] user_data    User data.
     * @return Error code.
     */
    std::error_code request_sample_no_lock(uint64_t user_data) {
        if (!active_)
            return std::make_error_code(std::errc::invalid_argument);

        std::error_code ec;
        std::tie(ec, std::ignore) = get_syscall_iface().ioctl(fd_, ioctl::vinstr::command::dump, 0);

        if (ec)
            return ec;

        user_data_manual_.push(user_data);

        return ec;
    }

    /**
     * Clear hardware counters values.
     *
     * @return Error code.
     */
    std::error_code clear() {
        std::error_code ec;

        std::tie(ec, std::ignore) = get_syscall_iface().ioctl(fd_, ioctl::vinstr::command::clear, 0);

        return ec;
    }

    /** @return Timestam iface reference. */
    timestamp_iface_t &get_ts_iface() { return *this; }

    using sample_handle_type = ioctl::vinstr::reader_metadata;

    using reader_features_type = ioctl::vinstr::reader_features;
    /** Vinstr reader features. */
    const reader_features_type features_;
    /** Hardware counters buffer size. */
    const size_t buffer_size_;
    /** Mutex protecting access to the active flag. */
    std::mutex access_;
    /** Sampler state. */
    bool active_{};
    /** True if sampling thread is running. */
    bool sampling_{};
    /** User data for manual samples. */
    queue<uint64_t, args_type::max_buffer_count> user_data_manual_{};
    /** Maximum profiling sessions being tracked at a time.
     *
     * Every session stop() results into a manual sample. There could be at most
     * `args_type::buffer_count` in the ring buffer. Therefore, we can
     * have `args_type::buffer_count` start/stop pairs and one extra `start`
     * (because it does not require a sample). Since a queue size must be a power of two,
     * we take the next power of two.
     */
    static constexpr size_t max_sessions = args_type::max_buffer_count * 2;
    /** Profiling session states. */
    queue<session, max_sessions> sessions_;
    /** Counter to allocate values for sample_metadata::sample_nr. */
    uint64_t sample_nr_alloc_{};
    /** Sample layout data structure. */
    sample_layout sample_layout_;
};

} // namespace vinstr
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
