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
 * Private hardware counters sampler interface.
 */

#pragma once

#include <device/api.hpp>

#include <cstdint>
#include <memory>
#include <system_error>

namespace hwcpipe {
namespace device {

class instance;

namespace hwcnt {

class reader;

/**
 * Hardware counters sampler namespace.
 *
 * The namespace contains two hardware counter sampler classes and their
 * configuration structure.
 *  - @ref manual for manual sampling.
 *  - @ref periodic for periodic sampling.
 *  - @ref configuration to configure which counters to sample for a given
 *    block type.
 *
 * Also contains the low level @c detail::backend interface,
 * which is used for samplers implementation. The interface is implemented
 * for each hardware counters ioctl API version.
 */
namespace sampler {

struct configuration;

/** Private implementation namespace. */
namespace detail {

/** Private hardware counters sampler interface. */
class HWCPIPE_DEVICE_API backend {
  public:
    /** Destructor. */
    virtual ~backend();

    /**
     * Start counters sampling or accumulation.
     *
     * If the backend was configured as periodic, the method starts counters sampling.
     * If the backend was configured as manual, the method starts counters accumulation.
     *
     * If the backend was active at the destruction time, it will be stopped implicitly
     * by the destructor.
     *
     * @param[in] user_data    User data value to be stored in `sample_metadata::user_data`.
     * @return Error code.
     */
    virtual std::error_code start(uint64_t user_data) = 0;

    /**
     * Stop counters sampling or accumulation.
     *
     * If the backend was configured as periodic, the method stops counters sampling.
     * If the backend was configured as manual, the method stops counters accumulation.
     * Before stopping, one last sample is done synchronously.
     *
     * @param[in] user_data    User data value to be stored in `sample_metadata::user_data`.
     * @return Error code.
     */
    virtual std::error_code stop(uint64_t user_data) = 0;

    /**
     * Request manual sample.
     *
     * The backend must have been created as manual, otherwise this function fails.
     *
     * @param[in] user_data    User data value to be stored in `sample_metadata::user_data`.
     * @return Error code.
     */
    virtual std::error_code request_sample(uint64_t user_data) = 0;

    /**
     * Get hardware counters reader.
     *
     * @return Hardware counters reader. @see reader.
     */
    virtual reader &get_reader() = 0;

    /**
     * Create hardware counters reader instance.
     *
     * @param[in] inst         Mali device instance.
     * @param[in] period_ns    Period in nanoseconds between samples taken. Zero for manual context.
     * @param[in] config       Which counters to enable on per-block basis.
     * @param[in] config_len   Len of @p config array.
     * @return Backend instance, nullptr if failed.
     */
    static std::unique_ptr<backend> create(const instance &inst, uint64_t period_ns, const configuration *config,
                                           size_t config_len);
};

} // namespace detail
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
