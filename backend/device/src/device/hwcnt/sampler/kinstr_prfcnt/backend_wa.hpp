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

/** @file backend_wa.hpp Workaround kinstr_prfcnt back-end implementation. */

#pragma once

#include "backend.hpp"

#include <device/hwcnt/sampler/discard_impl.hpp>
#include <device/hwcnt/sampler/poll.hpp>
#include <device/hwcnt/sampler/queue.hpp>
#include <device/hwcnt/sampler/timestamp.hpp>

#include <algorithm>
#include <mutex>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {

/**
 * Workaround back-end implementation.
 *
 * There are three kbase defects that this back-end workarounds:
 *
 *  1. When HWCNT buffer is full and `backend::stop` is called, the implicit sample
 *     will not be taken and no error is returned.
 *  2. When `backend::stop` is called for a periodic back-end, the counters disabling
 *     routine is racing with the sampling thread, and an empty sample might be taken.
 *  3. `block_metadata::index` values are not contiguous for shader core blocks.
 *
 * For manual case, we figure out the HWCNT buffer size, and make sure that at least
 * one slot is always reserved for the stop command what fixes #1. And #2 is not
 * applicable.
 * For periodic case, this class implements a session tracking algorithm. It records
 * when a session is started/stopped and when its stop sample is consumed. If we detect
 * that a stop sample has been lost, we emit a fake one with error flag set and no
 * counter values (fixes #1). If we see an empty sample while waiting for the stop
 * sample, we ignore it (fixes #2).
 * The #3 is fixed by customizing the `block_index_remap_t` template argument.
 */
template <typename backend_base_t, typename timestamp_iface_t = timestamp_iface>
class backend_wa_impl : public backend_base_t, private timestamp_iface_t {
    using super = backend_base_t;
    using args_type = typename super::args_type;
    using syscall_iface_type = typename super::syscall_iface_type;

    using super::fd_;
    using super::get_syscall_iface;
    using super::sampler_type;

  public:
    /**
     * Constructor.
     *
     * @param[in] args            Back-end arguments.
     * @param[in] syscall_iface   Syscall interface.
     * @param[in] timestamp_iface Timestam interface.
     */
    backend_wa_impl(args_type &&args, const syscall_iface_type &syscall_iface = {},
                    const timestamp_iface_t &timestamp_iface = {})
        : super(std::move(args), &remap_, syscall_iface)
        , timestamp_iface_t(timestamp_iface)
        , remap_(args.sc_mask) {

        if (sampler_type() == super::sampler_type::manual) {
            num_buffers_max_ = compute_num_buffers_max();
            num_buffers_ = num_buffers_max_;
        }
    }

    std::error_code start(uint64_t user_data) override {
        std::lock_guard<std::recursive_mutex> lock(access_);

        if (active_)
            return {};

        std::error_code ec;

        if (sampler_type() == super::sampler_type::manual)
            ec = start_manual(user_data);
        else
            ec = start_periodic(user_data);

        if (ec)
            return ec;

        active_ = true;

        return {};
    }

    std::error_code stop(uint64_t user_data) override {
        std::lock_guard<std::recursive_mutex> lock(access_);

        if (!active_)
            return {};

        std::error_code ec;

        if (sampler_type() == super::sampler_type::manual)
            ec = stop_manual(user_data);
        else
            ec = stop_periodic(user_data);

        if (ec)
            return ec;

        active_ = false;

        return {};
    }

    std::error_code request_sample(uint64_t user_data) override {
        std::lock_guard<std::recursive_mutex> lock(access_);

        if (sampler_type() != super::sampler_type::manual)
            return std::make_error_code(std::errc::invalid_argument);

        if (num_buffers_ <= 1)
            return std::make_error_code(std::errc::operation_not_permitted);

        auto ec = super::request_sample(user_data);

        if (ec)
            return ec;

        num_buffers_--;

        return {};
    }

    std::error_code get_sample(sample_metadata &sm, sample_handle &sample_hndl_raw) override {
        std::lock_guard<std::recursive_mutex> lock(access_);

        std::error_code ec;

        if (sampler_type() == super::sampler_type::manual)
            ec = get_sample_manual(sm, sample_hndl_raw);
        else
            ec = get_sample_periodic(sm, sample_hndl_raw);

        if (ec)
            return ec;

        sm.sample_nr = sample_nr_++;

        return {};
    }

    bool next(sample_handle sample_hndl_raw, block_metadata &bm, block_handle &block_hndl_raw) const override {
        std::lock_guard<std::recursive_mutex> lock(access_);

        if (empty_sample_)
            return false;

        return super::next(sample_hndl_raw, bm, block_hndl_raw);
    }

    std::error_code put_sample(sample_handle sample_hndl_raw) override {
        std::lock_guard<std::recursive_mutex> lock(access_);

        if (sampler_type() == super::sampler_type::manual)
            return put_sample_manual(sample_hndl_raw);
        else
            return put_sample_periodic(sample_hndl_raw);
    }

    std::error_code discard() override { return discard_impl(*this, get_syscall_iface(), get_ts_iface()); }

  private:
    /** @return Timestam iface reference. */
    timestamp_iface_t &get_ts_iface() { return *this; }

    /** @return HWCNT buffer size. */
    uint64_t compute_num_buffers_max() {
        uint64_t result{};
        static constexpr uint64_t result_max = 1024;

        nofail(super::start(0));

        for (; result <= result_max; ++result)
            if (super::request_sample(0))
                break;

        nofail(super::stop(0));
        nofail(super::discard());

        return result;
    }

    /**
     * Start manual session.
     *
     * @param[in] user_data    User data.
     */
    std::error_code start_manual(uint64_t user_data) {
        if (num_buffers_ == 0)
            return std::make_error_code(std::errc::operation_not_permitted);

        return super::start(user_data);
    }

    /**
     * Stop manual session.
     *
     * @param[in] user_data    User data.
     */
    std::error_code stop_manual(uint64_t user_data) {
        auto ec = super::stop(user_data);

        if (ec)
            return ec;

        assert(num_buffers_ >= 1);
        num_buffers_--;

        return {};
    }

    /**
     * Get sample (manual back-end).
     *
     * @param[out] sm                 Sample metadata.
     * @param[out] sample_hndl_raw    Sample handle.
     * @return Error code.
     */
    std::error_code get_sample_manual(sample_metadata &sm, sample_handle &sample_hndl_raw) {
        return super::get_sample(sm, sample_hndl_raw);
    }

    /**
     * Put sample (manual back-end).
     *
     * @param[in]     sample_hndl_raw    Sample handle.
     * @return Error code.
     */
    std::error_code put_sample_manual(sample_handle sample_hndl_raw) {
        auto ec = super::put_sample(sample_hndl_raw);

        if (ec)
            return ec;

        num_buffers_++;
        assert(num_buffers_ <= num_buffers_max_);

        return {};
    }

    /**
     * Start periodic session.
     *
     * @param[in] user_data    User data.
     * @return Error code.
     */
    std::error_code start_periodic(uint64_t user_data) {
        if (sessions_.full())
            return std::make_error_code(std::errc::operation_not_permitted);

        const auto begin_ts = this->clock_gettime();
        session_type session{session_nr(), user_data, begin_ts};

        auto ec = super::start(session.our_user_data_start());

        if (ec)
            return ec;

        sessions_.push(session);

        return {};
    }

    /**
     * Stop periodic session.
     *
     * @param[in] user_data    User data.
     * @return Error code.
     */
    std::error_code stop_periodic(uint64_t user_data) {
        assert(!sessions_.empty());

        auto &session = sessions_.back();

        const auto end_begin_ts = this->clock_gettime();
        auto ec = super::stop(session.our_user_data_stop());
        const auto end_end_ts = this->clock_gettime();

        if (ec)
            return ec;

        session.stop(user_data, end_begin_ts, end_end_ts);

        return {};
    }

    /**
     * Get sample (periodic back-end).
     *
     * @param[out] sm                 Sample metadata.
     * @param[out] sample_hndl_raw    Sample handle.
     * @return Error code.
     */
    std::error_code get_sample_periodic(sample_metadata &sm, sample_handle &sample_hndl_raw) {
        assert(!empty_sample_);

        for (;;) {
            std::error_code ec;
            sm = {};

            if (stash_.has_value()) {
                stash_.release(sm, sample_hndl_raw);
            } else {
                ec = super::get_sample(sm, sample_hndl_raw);
            }

            if (ec) {
                /* Check if the error was caused by the stop() race, and if so, try again. */
                if (detect_stop_race(sm))
                    continue;

                return ec;
            }

            auto &session = sessions_.front();

            if (session.our_user_data_start() == sm.user_data) {
                /* Got a periodic sample for a session. */
                sm.user_data = session.their_user_data_start();
                session.update_ts(sm.timestamp_ns_end);
                break;
            } else if (session.our_user_data_stop() == sm.user_data) {
                /* Got a stop sample for a session. */

                if (session.had_error()) {
                    /* This session had a stop() race. The invalid data is discarded. */
                    nofail(put_sample(sample_hndl_raw));

                    get_emtpy_sample(session, sm, sample_hndl_raw);
                } else {
                    sm.user_data = session.their_user_data_stop();
                }

                sessions_.pop();
                break;
            } else if (session.our_user_data_keep() == sm.user_data) {
                /* Got a periodic sample from the keep alive session, ignore it. */
                nofail(super::put_sample(sample_hndl_raw));
                continue;
            } else if (session.our_user_data_keep_stop() == sm.user_data) {
                /* Got a stop sample from the keep alive session. Stop the session
                 * and emit an empty stop sample.
                 */
                nofail(super::put_sample(sample_hndl_raw));

                get_emtpy_sample(session, sm, sample_hndl_raw);
                sessions_.pop();
                break;
            } else {
                /* Got a sample from the next session. The stop sample for the current
                 * session has been lost. Stash the sample we have just obtained and
                 * emit and empty stop sample.
                 */
                stash_.save(sm, sample_hndl_raw);

                assert(session.session_nr_diff(sm.user_data) < max_sessions);

                get_emtpy_sample(session, sm, sample_hndl_raw);
                sessions_.pop();
                break;
            }
        }

        return {};
    }

    /**
     * Put sample (periodic back-end).
     *
     * @param[in]     sample_hndl_raw    Sample handle.
     * @return Error code.
     */
    std::error_code put_sample_periodic(sample_handle sample_hndl_raw) {
        std::error_code ec;
        if (empty_sample_) {
            put_emtpy_sample(sample_hndl_raw);
        } else {
            ec = super::put_sample(sample_hndl_raw);
        }

        if (ec)
            return ec;

        if (sessions_.empty())
            return {};

        auto &session = sessions_.back();
        if (!session.is_stopped())
            return {};

        {
            /* Check if we have more samples to consume. Since stop() is synchronous,
             * we expect HWCNT buffer to be non empty. If it is empty, the stop sample
             * has been lost.
             */
            std::error_code ec;
            bool ready = false;

            std::tie(ec, ready) = check_ready_read(this->fd_, this->get_syscall_iface());
            nofail(ec);

            if (ready)
                return {};
        }

        {
            /* Do extra start/stop. The stop must trigger an implicit sample. This will keep
             * the file descriptor active (`poll` will return `1`). Later this sample will be
             * replaced with an empty stop sample. Here we assume that the stop sample will not
             * be lost. The assumption is safe because at start() time the HWCNT buffer is empty
             * and start/stop are issued back to back, so we unlikely to overflow the buffer.
             */
            nofail(super::start(session.our_user_data_keep()));

            const auto end_begin_ts = this->clock_gettime();
            nofail(super::stop(session.our_user_data_keep_stop()));
            const auto end_end_ts = this->clock_gettime();

            session.keep_stop(end_begin_ts, end_end_ts);
        }

        return {};
    }

    /**
     * Detect stop() / sampling thread race.
     *
     * @param[in] sm    Sample metadata obtained from failed get_sample().
     * @return True if stop() race is detected, false otherwise.
     */
    bool detect_stop_race(const sample_metadata &sm) {
        size_t session_idx = 0;
        for (; session_idx < sessions_.size(); ++session_idx) {
            if (sessions_[session_idx].on_error(sm))
                return true;
        }

        return false;
    }

    /** @return Current session number. */
    uint64_t session_nr() const { return sessions_.push_count(); }

    /**
     * Assert no errors.
     *
     * @param[in] ec    Error code to check.
     */
    static void nofail(std::error_code ec) {
        assert(!ec);
        static_cast<void>(ec);
    }

    /** Session state (periodic back-end only). */
    class session_type {
      public:
        /** Default constructor. */
        session_type() = default;

        /**
         * Constructor.
         *
         * @param[in] session_nr      Session number.
         * @param[in] user_data       User data specified at session start time.
         * @param[in] timestamp_ns    Session start timestamp (ns).
         */
        session_type(uint64_t session_nr, uint64_t user_data, uint64_t timestamp_ns)
            : session_nr_(session_nr & session_nr_mask)
            , user_data_start_(user_data)
            , last_ts_(timestamp_ns) {}

        /**
         * Track session stop.
         *
         * @param[in] user_data       User data specified at session stop time.
         * @param[in] end_begin_ts    Timestamp before `super::stop` call.
         * @param[in] end_end_ts      Timestamp after `super::stop` call.
         */
        void stop(uint64_t user_data, uint64_t end_begin_ts, uint64_t end_end_ts) {
            assert(end_begin_ts < end_end_ts);

            end_begin_ts_ = end_begin_ts;
            end_end_ts_ = end_end_ts;
            user_data_stop_ = user_data;

            assert(is_stopped());
        }

        /**
         * Track session keep-stop.
         *
         * @param[in] end_begin_ts    Timestamp before `super::stop` call.
         * @param[in] end_end_ts      Timestamp after `super::stop` call.
         */
        void keep_stop(uint64_t end_begin_ts, uint64_t end_end_ts) {
            assert(end_begin_ts < end_end_ts);

            keep_end_begin_ts_ = end_begin_ts;
            keep_end_end_ts_ = end_end_ts;

            assert(is_keep_stopped());
        }

        /**
         * Track sampling/stopping race errors at get_sample() failure time.
         *
         * @param[in] sm    Sample metadata passed to the get_sample() failed.
         * @return True if a stop() race is detected for this session.
         */
        bool on_error(const sample_metadata &sm) {
            if (!is_stopped())
                return false;

            const auto ts = sm.timestamp_ns_end;
            const auto user_data = sm.user_data;

            /* Check if it's a periodic sample taken at stop time. */
            if (user_data == our_user_data_start() && (ts >= end_begin_ts_ && ts <= end_end_ts_)) {
                had_error_ = true;
                return true;
            }

            /* Check if it's a periodic sample taken at keep-stop time. */
            if (user_data == our_user_data_keep() && (ts >= keep_end_begin_ts_ && ts <= keep_end_end_ts_)) {
                had_error_ = true;
                return true;
            }

            return false;
        }

        /**
         * Update last timestamp (ns).
         *
         * @param[in] timestamp_ns    New timestamp value.
         */
        void update_ts(uint64_t timestamp_ns) {
            assert(last_ts_ < timestamp_ns);
            last_ts_ = timestamp_ns;
        }

        /** @return Original user user data for start command. */
        uint64_t their_user_data_start() const { return user_data_start_; }
        /** @return Original user user data for stop command. */
        uint64_t their_user_data_stop() const { return user_data_stop_; }

        /** @return Our user data for start command. */
        uint64_t our_user_data_start() const { return session_nr_; }
        /** @return Our user data for stop command. */
        uint64_t our_user_data_stop() const { return session_nr_ | stop_bit; }
        /** @return Our user data for keep-alive start command. */
        uint64_t our_user_data_keep() const { return session_nr_ | keep_bit; }
        /** @return Our user data for keep-alive stop command. */
        uint64_t our_user_data_keep_stop() const { return session_nr_ | keep_bit | stop_bit; }

        /** @return True, if the session was stopped, false otherwise. */
        bool is_stopped() const { return end_end_ts_ != 0; }

        /** @return True, if the session was keep-stopped, false otherwise. */
        bool is_keep_stopped() const { return keep_end_end_ts_ != 0; }

        /** @return True if this session had an error. */
        bool had_error() const { return had_error_; }

        /** @return Empty sample metadata to replace the lost stop-sample. */
        sample_metadata empty_sample_metadata() const {
            sample_metadata result{};

            result.user_data = user_data_stop_;
            result.flags.error = 1;
            result.timestamp_ns_begin = last_ts_;
            result.timestamp_ns_end = end_end_ts_;

            return result;
        }

        /**
         * Return difference between this session session_nr and a future session_nr.
         *
         * @param[in] our_user_data    Our user data from a future session.
         * @return Sample numbers difference.
         */
        uint64_t session_nr_diff(uint64_t our_user_data) {
            const auto session_nr = our_user_data & session_nr_mask;
            return session_nr - session_nr_;
        }

      private:
        /** User data bit indicating stop samples. */
        static constexpr uint64_t stop_bit = uint64_t{1} << uint64_t{63};
        /** User data bit indicating keep-alive samples. */
        static constexpr uint64_t keep_bit = uint64_t{1} << uint64_t{62};
        /** Session number mask. */
        static constexpr uint64_t session_nr_mask = ~(stop_bit | keep_bit);

        /** Session number of this session. */
        uint64_t session_nr_{};
        /** User data specified at session start time. */
        uint64_t user_data_start_{};
        /** User data specified at session stop time. */
        uint64_t user_data_stop_{};
        /** Either start timestamp or the timestamp for the last sample (ns). */
        uint64_t last_ts_{};
        /** Timestamp before `super::stop` call. */
        uint64_t end_begin_ts_{};
        /** Timestamp after `super::stop` call. */
        uint64_t end_end_ts_{};
        /** Timestamp before `super::stop` call at session keep-stop time. */
        uint64_t keep_end_begin_ts_{};
        /** Timestamp after `super::stop` call at session keep-stop time. */
        uint64_t keep_end_end_ts_{};
        /** True if this session had an error. */
        bool had_error_{};
    };

    void get_emtpy_sample(const session_type &session, sample_metadata &sm, sample_handle &handle) {
        assert(!empty_sample_);

        sm = session.empty_sample_metadata();

        auto &empty_sample = handle.get<empty_sample_type>();
        std::fill(empty_sample.begin(), empty_sample.end(), empty_sample_pattern);

        empty_sample_ = true;
    }

    void put_emtpy_sample(sample_handle handle) {
        assert(empty_sample_);

        const auto &empty_sample = handle.get<empty_sample_type>();
        assert(std::all_of(empty_sample.begin(), empty_sample.end(),
                           [](auto val) { return val == empty_sample_pattern; }));

        /* Use empty_sample if the assertion is compiled out. */
        static_cast<void>(empty_sample);

        empty_sample_ = false;
    }

    /** Sample handle stash storage. */
    class sample_stash {
      public:
        /**
         * Save sample metadata and handle values.
         *
         * @param[in] sm        Sample metadata to save.
         * @param[in] handle    Sample handle to save.
         */
        void save(const sample_metadata &sm, sample_handle handle) {
            assert(!has_value_);

            metadata_ = sm;
            handle_ = handle;
            has_value_ = true;
        }

        /**
         * Get sample metadata and handle values and empty the stash.
         *
         * @param[in] sm        Sample metadata got.
         * @param[in] handle    Sample handle got.
         */
        void release(sample_metadata &sm, sample_handle &handle) {
            assert(has_value_);

            sm = std::exchange(metadata_, sample_metadata{});
            handle = std::exchange(handle_, sample_handle{});
            has_value_ = false;
        }

        /** @return True, if the stash has value, false otherwise. */
        bool has_value() const { return has_value_; }

      private:
        /** Has value flag. */
        bool has_value_{false};
        /** Sample metadata stored. */
        sample_metadata metadata_{};
        /** Sample handle stored. */
        sample_handle handle_{};
    };

    /** Mutex to protect access to this class. */
    mutable std::recursive_mutex access_;

    /** True if there is an active sampling session. */
    bool active_{false};
    /** Counters used for sample numbers allocations. */
    uint64_t sample_nr_{0};

    /** Maximum buffers in HWCNT ring buffer (manual back-end only). */
    uint64_t num_buffers_max_{0};
    /** Current number of buffers in HWCNT ring buffer (manual back-end only). */
    uint64_t num_buffers_{0};

    /** Sample stash storage. */
    sample_stash stash_;
    /** True if the user got an empty sample, false otherwise. */
    bool empty_sample_{false};
    /** Empty sample handle type. */
    using empty_sample_type = std::array<uint8_t, sample_handle_size>;
    /** Empty sample bit pattern for empty sample handles. */
    static constexpr uint8_t empty_sample_pattern{0xFE};
    /** Maximum number of sessions being tracked at a time. */
    static constexpr size_t max_sessions = 32;
    /** Queue of sessions being tracked (periodic back-end only). */
    queue<session_type, max_sessions> sessions_;
    /** Block index remap. */
    block_index_remap remap_;
};

template <typename backend_base_t, typename timestamp_iface_t>
constexpr uint8_t backend_wa_impl<backend_base_t, timestamp_iface_t>::empty_sample_pattern;

/** Workaround back-end type. */
template <typename syscall_iface_t>
using backend_wa = backend_wa_impl<backend<syscall_iface_t>>;

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
