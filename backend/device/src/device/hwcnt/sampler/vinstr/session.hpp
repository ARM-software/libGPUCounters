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
 * @file session.hpp
 *
 * Vinstr profiling session state.
 */

#pragma once

#include <cassert>
#include <cstdint>
#include <utility>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace vinstr {

/** Vinstr profiling session state class. */
class session {
  public:
    /** Default constructor. */
    session() = default;

    /**
     * Session constructor.
     *
     * @param[in] start_ts_ns           Session start timestamp (nanoseconds).
     * @param[in] user_data_periodic    User data for periodic samples.
     */
    session(uint64_t start_ts_ns, uint64_t user_data_periodic)
        : last_ts_ns_(start_ts_ns)
        , user_data_periodic_(user_data_periodic) {}

    /** Default copy constructor. */
    session(const session &) = default;
    /** Default assignment operator. */
    session &operator=(const session &) = default;

    /**
     * Update last timestamp.
     *
     * @param[in] ts New timestamp value to store.
     * @return Last timestamp (nanoseconds) before the update.
     */
    uint64_t update_ts(uint64_t ts) {
        assert(last_ts_ns_ < ts);

        return std::exchange(last_ts_ns_, ts);
    }

    /** @return user data for periodic samples. */
    uint64_t user_data_periodic() const { return user_data_periodic_; }

    /**
     * Track session stop.
     *
     * @param[in] stop_sample_nr    Number of the manual sample that corresponds to
     *                              this session stop.
     */
    void stop(uint64_t stop_sample_nr) {
        pending_stop_ = true;
        stop_sample_nr_ = stop_sample_nr;
    }

    /**
     * Check if this session state can be erased.
     *
     * @param[in] manual_sample_nr    Number of the manual sample being parsed.
     * @return if this session can be removed.
     */
    bool can_erase(uint64_t manual_sample_nr) const {
        if (!pending_stop_)
            return false;

        return manual_sample_nr == stop_sample_nr_;
    }

  private:
    /** Either session start timestamp or last sample's timestamp. */
    uint64_t last_ts_ns_{};
    /** User data for periodic samples of this session. */
    uint64_t user_data_periodic_{};
    /** True stop() was called for this session, but some samples
     * might not have been parsed yet.
     */
    bool pending_stop_{};
    /** Number of the manual sample that was taken when this
     * session was stopped.
     */
    uint64_t stop_sample_nr_{};
};

} // namespace vinstr
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
