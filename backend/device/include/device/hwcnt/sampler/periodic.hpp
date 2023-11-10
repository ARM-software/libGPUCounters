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
 * Period hadrdware counters sampler header.
 */

#pragma once

#include <device/hwcnt/sampler/detail/backend.hpp>

#include <memory>

namespace hwcpipe {
namespace device {
namespace hwcnt {

class reader;

namespace sampler {

/** Periodic hardware counters sampler. */
class periodic {
  public:
    using backend_type = detail::backend;

    explicit periodic(std::unique_ptr<backend_type> backend = nullptr)
        : backend_(std::move(backend)) {}

    /**
     * Constructor.
     *
     * Construct periodic sampler and configure it with the parameters specified. The actual
     * sample period may be different if the requested sample period can't be met, for example
     * if the period is too small then the underlying implementation may not be able to keep up.
     *
     * @param[in] inst         Mali device instance.
     * @param[in] period_ns    Period in nanoseconds between samples taken.
     * @param[in] config       Which counters to enable on per-block basis.
     * @param[in] config_len   Len of @p config array.
     */
    periodic(const instance &inst, uint64_t period_ns, const configuration *config, size_t config_len) {
        if (period_ns == 0)
            return;

        backend_ = detail::backend::create(inst, period_ns, config, config_len);
    }

    /**
     * Initializer list constructor.
     *
     * @param[in] inst         Mali device instance.
     * @param[in] period_ns    Period in nanoseconds between samples taken.
     * @param[in] lst          Initializer list of configurations.
     */
    periodic(const instance &inst, uint64_t period_ns, std::initializer_list<const configuration> lst)
        : periodic(inst, period_ns, lst.begin(), lst.size()) {}

    /** Default move construct. */
    periodic(periodic &&) = default;

    /** Default move assign. */
    periodic &operator=(periodic &&) = default;

    /**
     * Check if the sampler's back-end was initialized successfully.
     *
     * Calling any methods when this function returns false is a programming error.
     *
     * @par Example
     * @code
     * sampler::periodic m{1000000, config.data(), config.size()};
     * if (!m) {
     *     puts("Periodic sampler initialization failed!");
     *     return false;
     * }
     * @endcode
     *
     * @return true if sampler initialization was successful, false otherwise.
     */
    operator bool() const { return !!backend_; }

    /**
     * Start counters sampling.
     *
     * Initially the sampler is created in disabled mode: counters are not counting,
     * no counter data is being accumulated and no periodic sampling is scheduled.
     *
     * The function starts counter accumulation and sampling. The selected counters start
     * counting their corresponding events. The counter values are sampled periodically
     * with the interval specified at the construction time.
     *
     * If sampling is already started, the function is no op.
     *
     * @param[in] user_data    User data value to be stored in `sample_metadata::user_data`.
     * @return Error code.
     */
    std::error_code sampling_start(uint64_t user_data) {
        assert(backend_ && "Backend initialization failed!");
        return backend_->start(user_data);
    }

    /**
     * Stop counters sampling.
     *
     * Stop periodic counters sampling. Store the accumulated counter values since
     * the most recent periodic sample or a call to @ref sampling_start in the sample
     * ring buffer. Stop counters accumulation and counting.
     *
     * If sampling was active at the destruction time, it will be stopped implicitly
     * by the destructor.
     *
     * If sampling is already stopped or not started, the function is no op.
     *
     * @param[in] user_data    User data value to be stored in `sample_metadata::user_data`.
     * @return Error code.
     */
    std::error_code sampling_stop(uint64_t user_data) {
        assert(backend_ && "Backend initialization failed!");
        return backend_->stop(user_data);
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
