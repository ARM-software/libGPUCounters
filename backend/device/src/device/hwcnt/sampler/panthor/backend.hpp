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

/**
 * @file backend.hpp
 *
 * Panthor hardware counters sample back-end implementation.
 */

#pragma once

#include "backend_args.hpp"

#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/reader.hpp>
#include <device/hwcnt/sample.hpp>
#include <device/hwcnt/sampler/base/backend.hpp>
#include <device/hwcnt/sampler/detail/backend.hpp>
#include <device/hwcnt/sampler/discard_impl.hpp>
#include <device/hwcnt/sampler/panthor/convert.hpp>
#include <device/hwcnt/sampler/poll.hpp>
#include <device/hwcnt/sampler/queue.hpp>
#include <device/hwcnt/sampler/timestamp.hpp>
#include <device/ioctl/offset_pointer.hpp>
#include <device/ioctl/panthor/commands.hpp>
#include <device/ioctl/panthor/types.hpp>
#include <device/ioctl/strided_array_iterator.hpp>
#include <device/ioctl/strided_array_reader.hpp>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <system_error>

#include <sys/eventfd.h>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace panthor {

/** HWC backend using the panthor kernel ioctls. */
template <typename syscall_iface_t, typename timestamp_iface_t = timestamp_iface>
class backend : public base::backend<syscall_iface_t>, private timestamp_iface_t {
  public:
    using args_type = backend_args<syscall_iface_t>;
    using sample_header_type = ioctl::panthor::sample_header;
    using block_header_type = ioctl::panthor::block_header;
    using sample_array_reader = ioctl::strided_array_reader<sample_header_type>;
    using block_array_reader = ioctl::strided_array_reader<block_header_type>;

    explicit backend(args_type &&args, const syscall_iface_t &syscall_iface = {},
                     const timestamp_iface_t &timestamp_iface = {})
        : base_type(std::move(args.base_args), syscall_iface)
        , timestamp_iface_t(timestamp_iface)
        , panthor_fd_(std::move(args.panthor_fd))
        , manual_sync_fd_(std::move(args.manual_sync_fd))
        , session_id_(args.session_id)
        , slots_(args.slots)
        , sample_size_(args.sample_size)
        , control_mem_(std::move(args.control_mem))
        , control_struct_(static_cast<ioctl::panthor::perf_ringbuf_control *>(control_mem_.data()))
        , sample_layout_(std::move(args.sample_layout_v))
        , samples_reader_(memory_.data(), sample_size_, sample_layout_.sample_header_size(), slots_) {}
    std::error_code start(uint64_t user_data) override {
        std::lock_guard<std::mutex> lock(access_);

        if (active_)
            return {};

        ioctl::panthor::perf_cmd_start start{};
        start.user_data = user_data;

        ioctl::panthor::perf_control start_cmd{};
        start_cmd.cmd = ioctl::panthor::perf_command::start;
        start_cmd.handle = session_id_;
        start_cmd.size = sizeof(start);
        start_cmd.pointer.u_perf_cmd_start = &start;

        std::error_code ec;
        std::tie(ec, std::ignore) =
            get_syscall_iface().ioctl(panthor_fd_.get(), ioctl::panthor::command::perf_control, &start_cmd);
        if (ec)
            return ec;

        active_ = true;
        return {};
    }

    std::error_code stop(uint64_t user_data) override {
        std::lock_guard<std::mutex> lock(access_);
        std::error_code ec;

        if (!active_)
            return {};

        ioctl::panthor::perf_cmd_stop stop{};
        stop.user_data = user_data;

        ioctl::panthor::perf_control stop_cmd{};
        stop_cmd.cmd = ioctl::panthor::perf_command::stop;
        stop_cmd.handle = session_id_;
        stop_cmd.size = sizeof(stop);
        stop_cmd.pointer.u_perf_cmd_stop = &stop;

        std::tie(ec, std::ignore) =
            get_syscall_iface().ioctl(panthor_fd_.get(), ioctl::panthor::command::perf_control, &stop_cmd);
        if (ec)
            return ec;

        /* Stop produce a final sample, wait for this sample in case of manual sampler */
        if (sampler_type() == base_type::sampler_type::manual) {
            ec = wait_for_manual_sample();
            if (ec)
                return ec;
        }
        active_ = false;
        return {};
    }

    std::error_code request_sample(uint64_t user_data) override {
        if (sampler_type() != base_type::sampler_type::manual)
            return std::make_error_code(std::errc::invalid_argument);

        std::lock_guard<std::mutex> lock(access_);

        auto err = request_sample_no_lock(user_data);
        if (err)
            return err;
        /* Wait until the requested sample is ready */
        return wait_for_manual_sample();
    }

    std::error_code get_sample(sample_metadata &sm, sample_handle &sample_hndl) override {
        std::error_code ec = wait_for_sample(fd_, get_syscall_iface());
        if (ec)
            return ec;

        std::lock_guard<std::mutex> guard(access_);
        auto extract_idx = read_extract_idx();
        auto insert_idx = read_insert_idx();

        if (extract_idx == insert_idx)
            return std::make_error_code(std::errc::invalid_argument);

        const auto curr_idx = static_cast<int>(extract_idx % slots_);

        sample_header_type sample_header = samples_reader_.load(curr_idx);

        sm.user_data = sample_header.user_data;
        sm.flags = convert(sample_header.flags);
        sm.sample_nr = sample_nr_alloc_++;
        sm.timestamp_ns_begin = sample_header.timestamp_start_ns;
        sm.timestamp_ns_end = sample_header.timestamp_end_ns;
        /* These will be zero if the clocks are not supported. */
        sm.gpu_cycle = sample_header.toplevel_clock_cycles;
        sm.cg_cycle = sample_header.coregroup_clock_cycles;
        sm.sc_cycle = sample_header.shader_clock_cycles;
        sm.ne_cycle = sample_header.neural_clock_cycles;

        /*
         * Advance by the kernel-reported sample header size rather than
         * sizeof(sample_header_type) to remain compatible with header
         * size changes across kernel versions.
         */
        const auto *blocks_base = samples_reader_.field_end(curr_idx);
        block_array_reader blocks_reader{blocks_base, sample_layout_.block_size(), sample_layout_.block_header_size(),
                                         sample_layout_.size()};

        prfcnt_set global_set{};
        std::tie(ec, global_set) = convert(sample_header.block_set);
        if (ec)
            return ec;

        sample_iter it{std::move(blocks_reader), global_set, static_cast<int>(sample_layout_.size())};

        sample_hndl.get<sample_iter>() = std::move(it);

        return {};
    }

    bool next(sample_handle sample_hndl_raw, block_metadata &bm, block_handle &blk_hndl) const override {
        auto &iter = sample_hndl_raw.get<sample_iter>();
        auto &idx = blk_hndl.get<int>();

        while (idx < iter.block_num) {
            std::error_code ec;
            block_type temp_type{};
            block_header_type header = iter.it.load(idx);

            std::tie(ec, temp_type) = convert(header.block_type);
            if (ec)
                return false;

            /* Panthor sample contains all available blocks, even disabled ones.
             * Present only blocks that are enabled.
             */
            if (block_extents_.num_blocks_of_type(temp_type) == 0) {
                ++idx;
                continue;
            }
            bm.type = temp_type;
            bm.set = iter.set;
            bm.index = header.block_idx;
            bm.state = convert(header.block_states);
            bm.values = iter.it.field_end(idx);

            idx++;

            return true;
        }
        return false;
    }

    std::error_code put_sample(sample_handle /* sample_hndl_raw */) override {
        std::lock_guard<std::mutex> lock(access_);

        write_extract_idx(read_extract_idx() + 1);

        return read_eventfd(fd_);
    }

    std::error_code discard() override { return discard_impl(*this, get_syscall_iface(), get_ts_iface()); }

    ~backend() override {
        std::error_code ec;

        ioctl::panthor::perf_control teardown_cmd{};
        teardown_cmd.cmd = ioctl::panthor::perf_command::teardown;
        teardown_cmd.handle = session_id_;
        teardown_cmd.size = sizeof(sample);
        teardown_cmd.pointer.u_perf_cmd_teardown = nullptr;

        std::tie(std::ignore, std::ignore) =
            get_syscall_iface().ioctl(panthor_fd_.get(), ioctl::panthor::command::perf_control, &teardown_cmd);
    }

  private:
    using base_type = base::backend<syscall_iface_t>;
    using base_type::block_extents_;
    using base_type::fd_;
    using base_type::get_syscall_iface;
    using base_type::memory_;
    using base_type::period_ns_;
    using base_type::sampler_type;
    using memory_type = typename base_type::memory_type;
    /**
     * Request manual sample dump.
     *
     * @pre `access_` must be locked.
     *
     * @param[in] user_data    User data.
     * @return Error code.
     */
    std::error_code request_sample_no_lock(uint64_t user_data) {
        assert(!period_ns_);

        if (!active_)
            return std::make_error_code(std::errc::invalid_argument);

        std::error_code ec;
        ioctl::panthor::perf_cmd_sample sample{};
        sample.user_data = user_data;

        ioctl::panthor::perf_control sample_cmd{};
        sample_cmd.cmd = ioctl::panthor::perf_command::sample;
        sample_cmd.handle = session_id_;
        sample_cmd.size = sizeof(sample);
        sample_cmd.pointer.u_perf_cmd_sample = &sample;

        std::tie(ec, std::ignore) =
            get_syscall_iface().ioctl(panthor_fd_.get(), ioctl::panthor::command::perf_control, &sample_cmd);

        return ec;
    }

    std::error_code read_eventfd(int fd) {
        eventfd_t tmp = 0;

        /* For a semaphore file descriptor, this decrements the semaphore value by one. */
        if (eventfd_read(fd, &tmp) == -1)
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::backend_fd_error, "Failed to read user-exposed fd (errno=%d)",
                                           errno);

        return {};
    }

    /**
     * For manual samplers, wait until the Panthor backend signals that a sample
     * is ready.
     *
     * This function blocks on the manual synchronization eventfd
     * (`manual_sync_fd_`) until the backend produces a sample. Once signaled,
     * the eventfd is consumed (read) to reset it for the next sampling request.
     *
     * After the sample is ready, the user-exposed eventfd (`fd_`) is signaled
     * to notify the client that a new sample can be consumed.
     * @return std::error_code
     */
    std::error_code wait_for_manual_sample() {

        if (sampler_type() != base_type::sampler_type::manual)
            return std::make_error_code(std::errc::invalid_argument);

        const int sync_fd = manual_sync_fd_.get();
        auto ec = wait_for_sample(sync_fd, get_syscall_iface());
        if (ec)
            return ec;

        ec = read_eventfd(sync_fd);
        if (ec)
            return ec;

        /* Notify users that a new sample can be consumed */
        if (eventfd_write(fd_, eventfd_t{POLLIN}) == -1) {
            return HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::backend_fd_error, "Failed to write user-exposed fd (errno=%d)",
                                           errno);
        }

        return {};
    }

    uint64_t read_extract_idx() {
        const uint64_t extract_idx = control_struct_->extract_idx;
        std::atomic_thread_fence(std::memory_order_acquire);
        return extract_idx;
    }

    uint64_t read_insert_idx() {
        const uint64_t insert_idx = control_struct_->insert_idx;
        std::atomic_thread_fence(std::memory_order_acquire);
        return insert_idx;
    }

    void write_extract_idx(uint64_t idx) {
        std::atomic_thread_fence(std::memory_order_release);
        control_struct_->extract_idx = idx;
    }

    /**
     * Clear hardware counters values.
     *
     * @return Error code.
     */
    std::error_code clear() {
        std::lock_guard<std::mutex> lock(access_);

        if (!active_)
            return {};

        write_extract_idx(read_insert_idx());

        return {};
    }

    struct sample_iter {
        block_array_reader it;
        prfcnt_set set;
        int block_num;
    };

    /** @return Timestamp iface reference. */
    timestamp_iface_t &get_ts_iface() { return *this; }

    /** File descriptor corresponding to the panthor instance. */
    filefd_guard<syscall_iface_t> panthor_fd_;
    /**  File descriptor to Sync manual samplers requests. */
    filefd_guard<syscall_iface_t> manual_sync_fd_;
    uint32_t session_id_;
    /** Hardware counters buffer size. */
    const uint32_t slots_;
    /** The size of a single sample with the sample header and n block headers. */
    size_t sample_size_{};
    /* Mmap'ed memory containing the control indices. */
    memory_type control_mem_;
    /* Pointer to the control indices as a structure. */
    ioctl::panthor::perf_ringbuf_control *control_struct_;
    /** Sample layout data structure. */
    sample_layout sample_layout_;
    /** Holds the ringbuffer data. */
    sample_array_reader samples_reader_;
    /** Mutex protecting access to the active flag. */
    std::mutex access_;
    /** Sampler state. */
    bool active_{};
    /** Counter to allocate values for sample_metadata::sample_nr. */
    uint64_t sample_nr_alloc_{};
};

} // namespace panthor
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
