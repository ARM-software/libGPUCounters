/*
 * Copyright (c) 2021-2023 Arm Limited.
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
 * @file
 *
 * Manual hadrdware counters sampler header.
 */

#pragma once

#include <device/hwcnt/sampler/detail/backend.hpp>

#include <cassert>

namespace hwcpipe {
namespace device {
namespace hwcnt {

class reader;

namespace sampler {

/** Manual hardware counters sampler. */
class manual {
  public:
    using backend_type = detail::backend;

    explicit manual(std::unique_ptr<backend_type> backend = nullptr)
        : backend_(std::move(backend)) {}

    /**
     * Constructor.
     *
     * Construct manual sampler and configure it with the parameters specified.
     *
     * @param[in] inst         Mali device instance.
     * @param[in] config       Which counters to enable on per-block basis.
     * @param[in] config_len   Len of @p config array.
     */
    manual(const instance &inst, const configuration *config, size_t config_len)
        : backend_(detail::backend::create(inst, 0, config, config_len)) {}

    /**
     * Initializer list constructor.
     *
     * @param[in] inst   Mali device instance.
     * @param[in] lst    Initializer list of configurations.
     */
    manual(const instance &inst, std::initializer_list<configuration> lst)
        : manual(inst, lst.begin(), lst.size()) {}

    /** Default move construct. */
    manual(manual &&) = default;

    /** Default move assign. */
    manual &operator=(manual &&) = default;

    /**
     * Check if the sampler's back-end was initialized successfully.
     *
     * Calling any methods when this function returns false is a programming error.
     *
     * @par Example
     * @code
     * sampler::manual m{config.data(), config.size()};
     * if (!m) {
     *     puts("Manual sampler initialization failed!");
     *     return false;
     * }
     * @endcode
     *
     * @return true if sampler initialization was successful, false otherwise.
     */
    operator bool() const { return !!backend_; }

    /**
     * Start hardware counters accumulation.
     *
     * Initially the sampler is created in disabled mode: counters are not counting
     * and no counter data is being accumulated.
     *
     * The function starts counter accumulation. The selected counters start
     * counting their corresponding events.
     *
     * If accumulation is already started, the function is no op.
     *
     * @return Error code.
     */
    std::error_code accumulation_start() {
        assert(backend_ && "Backend initialization failed!");

        return backend_->start(0);
    }

    /**
     * Stop counters accumulation.
     *
     * Stop counter accumulation, and store the accumulated counter
     * values since the most recent call to @ref sample or @ref accumulation_start in the sample
     * ring buffer.
     *
     * If accumulation was active at the destruction time, it will be stopped implicitly
     * by the destructor.
     *
     * If accumulation is already stopped or not started, the function is no op.
     *
     * @param[in] user_data    User data value to be stored in `sample_metadata::user_data`.
     * @return Error code.
     */
    std::error_code accumulation_stop(uint64_t user_data) {
        assert(backend_ && "Backend initialization failed!");

        return backend_->stop(user_data);
    }

    /**
     * Request sample counters.
     *
     * Store accumulated counter values since the most recent call to @ref sample
     * or @ref accumulation_start in the sample ring buffer.
     *
     * The accumulation must have been started with @ref accumulation_start,
     * otherwise the function will return an error.
     *
     * If counters sampled rarely, the values may overflow. If overflow behavior is
     * defined, @ref features::overflow_behavior_defined flag is set.
     *
     * @param[in] user_data    User data value to be stored in `sample_metadata::user_data`.
     * @return Error code.
     */
    std::error_code request_sample(uint64_t user_data) {
        assert(backend_ && "Backend initialization failed!");

        return backend_->request_sample(user_data);
    }

    /**
     * Get hardware counters reader.
     *
     * The same reader reference is always returned, so the value can be cached.
     * The reference must not outlive its sampler instance.
     *
     * @return Hardware counters reader.
     */
    reader &get_reader() {
        assert(backend_ && "Backend initialization failed!");
        return backend_->get_reader();
    }

  private:
    std::unique_ptr<backend_type> backend_;
};

} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
