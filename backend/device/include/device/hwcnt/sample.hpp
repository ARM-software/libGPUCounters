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
 * Hardware counters sample header.
 */

#pragma once

#include <device/hwcnt/blocks_view.hpp>

#include <cassert>
#include <cstdint>
#include <system_error>

namespace hwcpipe {
namespace device {

/**
 * Hardware counters namespace.
 *
 * Hardware counters can be sampled from a GPU either manually or periodically:
 *  - @ref sampler::manual for manual sampling.
 *  - @ref sampler::periodic for periodic sampling.
 *
 * The sampled counter values can be read using @ref reader interface. A reader
 * instance can be obtained from a sampler by calling @ref sampler::manual::get_reader
 * or @ref sampler::periodic::get_reader.
 *
 * A counters sample is managed by @ref sample class. The counters are collected on
 * per hardware block basis and hence are stored as an array of @ref block_metadata.
 */
namespace hwcnt {

class reader;

/** Sample flags. */
struct sample_flags {
    /**
     * The counters sample period was not met because of the counters ring buffer
     * overflow. The sample period is stretched for this sample. The value is
     * undefined if @ref features::has_stretched_flag is false.
     */
    uint32_t stretched : 1;

    /** This sample has had an error condition for sample duration. */
    uint32_t error : 1;
};

/** Hardware counters sample meta-data. */
struct sample_metadata {
    /** User data. */
    uint64_t user_data;

    /** Sample flags. */
    sample_flags flags;

    /** Sample number. */
    uint64_t sample_nr;

    /** Earliest timestamp that values in this sample represent. */
    uint64_t timestamp_ns_begin;

    /** Latest timestamp that values in this sample represent. */
    uint64_t timestamp_ns_end;

    /**
     * GPU cycles elapsed since the last sample was taken.
     * The value is undefined if @ref features::has_gpu_cycle is false.
     */
    uint64_t gpu_cycle;

    /**
     * Shader cores cycles elapsed since the last sample was taken.
     * The value is undefined if @ref features::has_gpu_cycle is false.
     */
    uint64_t sc_cycle;
};

/**
 * Hardware counters sample.
 *
 * Represents an entry in the hardware counters @ref reader ring buffer.
 * After a sample was got from a reader using @ref reader::get_sample,
 * the user may extract @ref sample_metadata using @ref get_metadata method.
 * To extract the counters values, one should iterate over blocks using
 * @ref blocks.
 */
class sample {
  public:
    /**
     * Sample constructor.
     *
     * @warning
     * This function calls @ref reader::get_sample which may fail. To check
     * if the failure occurred, use @p ec or @ref operator bool(). It is a
     * programming error to call any other methods when construction failed.
     *
     * @param[in]     reader    Reader to read this sample from.
     * @param[in,out] ec        Reference where to store reader errors, if any.
     */
    sample(reader &reader, std::error_code &ec)
        : reader_(reader)
        , ec_(ec) {
        ec_ = reader_.get_sample(metadata_, sample_hndl_);
    }

    /**
     * Sample destructor.
     *
     * @warning
     * This function calls @ref reader::put_sample which may fail. To check
     * if the failure occurred, use the error code passed to this sample at
     * construction time.
     *
     * @par Example
     * @code
     * std::error_code ec;
     * {
     *     sample s{reader, ec};
     * }
     * if (!ec)
     *     puts("The sample was created and destroyed successfully.");
     * @endcode
     */
    ~sample() {
        if (!ec_)
            ec_ = reader_.put_sample(sample_hndl_);
    }

    /** @return true if the object was successfully constructed, false otherwise. */
    operator bool() const { return !ec_; }

    /**
     * Get hardware counters sample metadata.
     *
     * The method can only be called after obtaining a sample from @ref reader::get_sample.
     *
     * @return Sample metadata structure.
     */
    const sample_metadata &get_metadata() const {
        assert(!ec_ && "reader::get_sample failed, but metadata was read.");
        return metadata_;
    }

    /**
     * @return Hardware counters @ref blocks_view for this sample.
     */
    blocks_view blocks() const {
        assert(!ec_ && "reader::get_sample failed, but blocks were read.");
        return blocks_view(reader_, sample_hndl_);
    }

  private:
    /** Reader that was used to read this sample. */
    reader &reader_;

    /** Error code to store errors to. */
    std::error_code &ec_;

    /** Sample meta-data. */
    sample_metadata metadata_{};

    /** Point in the counters sample buffer where this sample was decoded from. */
    sample_handle sample_hndl_{};
};

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
