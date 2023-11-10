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
 * Hardware counters reader interface header.
 */

#pragma once

#include <device/api.hpp>
#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/detail/handle.hpp>
#include <device/hwcnt/features.hpp>

#include <array>
#include <system_error>

namespace hwcpipe {
namespace device {
namespace hwcnt {

/**
 * Default block handle size.
 *
 * Must be set to the biggest `sizeof` for all implementation dependent block handles.
 */
static constexpr size_t block_handle_size = 8;
/**
 * Default block handle alignment.
 *
 * Must be set to LCM of all implementation dependent block handles `alignnof` s.
 */
static constexpr size_t block_handle_alignment = 8;

/**
 * Block handle detail class.
 */
class block_handle : public detail::handle<block_handle_size, block_handle_alignment> {};

/**
 * Default sample handle size.
 *
 * Must be set to the biggest `sizeof` for all implementation dependent sample handles.
 */
static constexpr size_t sample_handle_size = 16;
/**
 * Default sample handle alignment.
 *
 * Must be set to LCM of all implementation dependent sample handles `alignnof` s.
 */
static constexpr size_t sample_handle_alignment = 8;

/**
 * Sample handle detail class.
 */
class sample_handle : public detail::handle<sample_handle_size, sample_handle_alignment> {};

struct sample_metadata;
struct block_metadata;

/**
 *  Hardware counters reader interface.
 *
 * Hardware counters are collected in the kernel space and delivered to the
 * user space using a ring buffer. The ring buffer has constant unspecified
 * size.
 *
 * To get counter values, the user should call @ref reader::get_sample. The
 * function waits until the sample is taken, and then acquires its entry
 * in the ring buffer, so the kernel does not modify this memory. Only one
 * sample can be processed at a time.
 *
 * The counter values can be obtained from the hardware counter blocks, which
 * can be iterated over using @ref reader::next.
 *
 * When the user is done with the counters processing, they should hand
 * the ring buffer entry back to the kernel using @ref reader::put_sample.
 *
 * If the user does not empty the ring buffer fast enough it may overflow.
 * In this case the values are kept accumulating what may result into counter
 * values overflow. If overflow behavior is defined,
 * @ref features::overflow_behavior_defined is set.
 *
 * @par Example
 * @code
 * namespace hwcnt = hwcpipe::device::hwcnt;
 * hwcnt::sample_metadata sample_metadata;
 * hwcnt::block_metadata block_metadata;
 * hwcnt::sample_handle sh{};
 * hwcnt::block_handle bh{};
 *
 * // Obtain reader from a sampler
 * hwcnt::reader &r = sampler.get_reader();
 * // Wait for sample and read it.
 * r.get_sample(sample_metadata, sh);
 * // Iterate over hardware counters blocks and process them.
 * for (;r.next(sh, block_metadata, bh);)
 *     process_block(block_metadata);
 * // Return sample back to the ring buffer.
 * r.put_sample(sh);
 * @endcode
 */
class HWCPIPE_DEVICE_API reader {
  public:
    /** Destructor. */
    virtual ~reader();

    /**
     * Get file descriptor of the hardware counters interface.
     *
     * The file descriptor can be used with `poll()` to check
     * if a new sample is available.
     *
     * @warning
     * This file descriptor is different from the one passed to
     * the @ref handle::create. The hardware counters infrastructure
     * uses its own anonymous node.
     *
     * @par Example
     * @code
     * int fd = reader.get_fd();
     * struct pollfd pfds[1] = {};
     * pfds[0].fd = fd;
     * pfds[0].events = POLLIN;
     * bool data_ready = poll(pfds, 1, 0) == 1;
     * @endcode
     *
     * @return hardware counters interface descriptor.
     */
    int get_fd() const { return fd_; }

    /**
     * Get features supported by this back-end instance.
     *
     * @return Backend features structure.
     */
    const features &get_features() const { return features_; }

    /**
     * Get hardware counters blocks extents info.
     *
     * The method differs from `instance::get_block_extents`. It returns
     * only those blocks that were requested at the sampler creation time.
     *
     * @see sampler::configuration
     *
     * @return Block layout structure.
     */
    const block_extents &get_block_extents() const { return block_extents_; }

    /**
     * Wait and get a new hardware counters sample.
     *
     * Block until a new sample is ready and decode its meta-data.
     * The counter values could be obtained from @ref block_metadata using @ref next.
     *
     * Only one sample can be processed at a time. Calling this function twice without
     * putting the first sample back with @ref put_sample will result into an error.
     *
     * @par Example
     * @code
     * sample_metadata metadata;
     * sample_handle hndl;
     * std::error_code ec = reader.get_sample(metadata, hdnl);
     * if (!ec)
     *     print_metadata(metadata);
     * @endcode
     *
     * @note It is a low level function, it is more convenient to use @ref sample class.
     *
     * @param[out] sm             Reference where decoded sample meta-data will be stored.
     * @param[out] sample_hndl    Reference where opaque sample handle will be stored.
     * @return Error code.
     */
    virtual std::error_code get_sample(sample_metadata &sm, sample_handle &sample_hndl) = 0;

    /**
     * Iterate over sample's hardware counters blocks.
     *
     * Given sample_handle @p sample_hndl iterate over hardware counters blocks of that sample.
     * @p block_hndl is an opaque block handle that represents an iteration state. A call to
     * this function with @p block_hndl set to @c nullptr  gives the handle of the first block.
     * Further calls will give next block handles allowing to iterate over all the blocks.
     * Two independent iterations are allowed.
     *
     * @warning
     * The iteration order is neither defined nor guaranteed:
     * the block's indices and types may appear out of order.
     *
     * Example:
     * @code
     * block_metadata metadata;
     * block_handle block_hndl = nullptr;
     * for (; backend.next(sample_hndl, metadata, block_hndl); )
     *     print_metadata(metadata);
     * @endcode
     *
     * @note It is a low level function, it is more convenient to use @ref sample class.
     *
     * @param[in]     sample_hndl    Sample handle previously obtained from @ref get_sample.
     * @param[out]    bm             Reference where decoded block meta-data will be stored.
     * @param[in,out] block_hndl     Block handle reference. If null, the first block handle
     *                               is stored in this variable. If non-null, the next block
     *                               handle is stored.
     *
     * @return False if there are no blocks to decode, True otherwise.
     */
    virtual bool next(sample_handle sample_hndl, block_metadata &bm, block_handle &block_hndl) const = 0;

    /**
     * Put hardware counters sample back to the kernel.
     *
     * Notify the kernel that the hardware counters sample is fully consumed
     * by the user space. Its entry in the shared memory ring-buffer can
     * be used again to store new samples.
     *
     * Once sample is released, it must not be used with any other functions.
     * The memory under @ref block_metadata::values for blocks of this sample is
     * no longer valid.
     *
     * @note It is a low level function, it is more convenient to use @ref sample class.
     *
     * @param[in]     sample_hndl    Sample handle previously obtained from @ref get_sample
     *                               to put back.
     * @return Error code.
     */
    virtual std::error_code put_sample(sample_handle sample_hndl) = 0;

    /**
     * Discard the contents of the ring buffer.
     *
     * @warning This function may race with periodically collected samples.
     * Make sure that periodic collection is stopped with @ref sampler::periodic::sampling_stop
     * before using this method.
     *
     * @return Error code.
     */
    virtual std::error_code discard() = 0;

  protected:
    /**
     * Constructor.
     *
     * @param[in] fd            Counters file descriptor.
     * @param[in] features_v    Reader features.
     * @param[in] extents       Hardware counters block extents.
     */
    reader(int fd, features features_v, block_extents extents)
        : fd_(fd)
        , features_(features_v)
        , block_extents_(extents) {}

    /** Hardware counters file descriptor. */
    int fd_;

    /** Features. */
    features features_;

    /** Block extents. */
    block_extents block_extents_;
};

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
