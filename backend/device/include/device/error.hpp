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
 * @file error.hpp
 *
 * Error handling of hwcpipe.
 */
#pragma once

#include <device/logging/logger.hpp>

#include <cassert>
#include <system_error>

namespace hwcpipe {
namespace device {

/**
 * Error codes of HWCPipe2.
 *
 * Each code corresponds to a point of failure where hwcpipe might fail and
 * we want to propagate this to the user.
 */
enum class hwcpipe_errc {
    success,

    /* Product ID */
    invalid_product_id, /**< Product id doesn't exist. */

    /* Instance Initialization */
    invalid_kernel_version,   /**< Kernel version not compatible with userspace. */
    warp_invalid_product_id,  /**< Missing product id in warp width calculation. */
    prop_reader_invalid_data, /**< Invalid data read from property reader. */
    failed_to_create_sampler, /**< Failed to create sampler. */

    /* Execution engines */
    invalid_num_exec_engines, /**< Failed to calculate number of execution engines. */

    /* Sampler and Backend */
    backend_invalid_type,              /**< Failed to find the backend type. */
    sampler_invalid_type,              /**< Invalid sampler type. */
    backend_start_invalid_buffer_size, /**< Invalid number of samples in hwc buffer. */
    invalid_buffer_size,               /**< Invalid number of samples in hwc buffer. */
    sampler_invalid_state,             /**< Can not request sample when another sample is in progress. */
    sampler_max_sessions,              /**< Reached maximum number of periodic sampler sessions. */
    invalid_sample,                    /**< Invalid sample received from the backend. */
    sampler_invalid_sc_remap,          /**< Invalid remapping of shader core indices. */
    sampler_timeout,                   /**< Sampler timed out waiting for sample. */
    backend_fd_error,                  /**< Backend failed to access an fd. */

    /* Block extents */
    extents_invalid_block_type,        /**< Invalid block type when parsing block extents. */
    extents_invalid_set,               /**< Invalid counter set when parsing block extents. */
    extents_invalid_num_values,        /**< Invalid number of values when parsing block extents. */
    extents_invalid_block,             /**< Invalid number of blocks when parsing block extents. */
    extents_invalid_num_instances,     /**< Invalid number of block instances when parsing block extents. */
    extents_version_not_supported,     /**< Prfcnt API version not supported. */
    extents_duplicate_request_type,    /**< Block extents don't allow for duplicate request types. */
    extents_duplicate_sample_info,     /**< Duplicate sample info when parsing block extents. */
    extents_invalid_num_clock_domains, /**< Invalid number of clock domains when parsing block extents. */

    /* Sample metadata */
    meta_sample_already_parsed, /**< Sample metadata already parsed. */
    meta_clock_already_parsed,  /**< Clock metadata already parsed. */
    meta_invalid_block_type,    /**< Invalid block type when parsing sample metadata. */
    meta_invalid_block_index,   /**< Invalid block index when parsing sample metadata. */
    meta_invalid_num_blocks,    /**< Invalid number of blocks when parsing sample metadata. */

    /* Parser */
    parser_invalid_order,    /**< Invalid order of parsing. */
    parser_not_done,         /**< Failed to finalize parsing data. */
    parser_invalid_data,     /**< Failed to parse data. */
    parser_invalid_argument, /**< Invalid argument when calling dispatch_on_item. */

    /* syscalls */
    ioctl_operation_not_permitted, /**< Ioctl return EPERM. */
    ioctl_invalid_argument,        /**< Ioctl return EINVAL. */
    ioctl_device_busy,             /**< Ioctl return EBUSY. */
    ioctl_bad_address,             /**< Ioctl failed to copy data EFAULT. */
    ioctl_io_error,                /**< Ioctl returned EIO. */
    ioctl_func_not_supported,      /**< Ioctl returned ENOSYS. */
    ioctl_not_enough_memory,       /**< Ioctl returned ENOMEM. */
    ioctl_bad_file_descriptor,     /**< Ioctl returned EBADF. */
};

/**
 * @brief Custom error category for hwcpipe.
 *
 * A custom error category to differentiate between hwcpipe
 * errors and std errors.
 */
class hwcpipe_error_category_t : public std::error_category {
  public:
    /** Return hwcpipe error category name. */
    const char *name() const noexcept override;

    /**
     * @brief Translate @ref hwcpipe::device::hwcpipe_errc to error message.
     *
     * @param[in] ev        Error value.
     * @return std::string  Error message.
     */
    std::string message(int ev) const override;

    /**
     * Return true if ec is of category @ref hwcpipe_error_category_t
     * and condition value matches condition.
     *
     * @param[in] ec        Error code.
     * @param[in] condition Error condition value to compare with.
     * @return bool         True if ec is of category @ref hwcpipe_error_category_t
     *                      and condition value matches condition.
     */
    bool equivalent(const std::error_code &ec, int condition) const noexcept override;
};

/**
 * @brief Return error message corresponding to the error code.
 *
 * @param[in] ec        Error code.
 * @return std::string  Error message.
 */
std::string get_error_message(std::error_code &ec);

} // namespace device
} // namespace hwcpipe

namespace std {
template <>
struct is_error_code_enum<hwcpipe::device::hwcpipe_errc> : true_type {};
/** Creates std error code value for hwcpipe error code. */
std::error_code make_error_code(hwcpipe::device::hwcpipe_errc e);
} // namespace std

/**
 * Macro printing the error code information.
 *
 * @param[in] func  Function name producing the error.
 * @param[in] fmt   Format of the extra error message with details about the error.
 * @param[in] ...   Variable argument list of the extra error message.
 */
#define HWCPIPE_LOG_ERROR_CODE(func, fmt, ...)                                                                         \
    HWCPIPE_LOG_ERROR("ERROR CODE: %s() returned with error code=[%i], error message=(%s) - error details: " fmt "%s", \
                      func, ec.value(), get_error_message(ec).c_str(), __VA_ARGS__)

/**
 * Create std error code value and log error message.
 *
 * The library takes extra information about the error message provided by the implementation.
 *
 * @param[in] _e    Error code of type @ref hwcpipe::device::hwcpipe_errc.
 * @param[in] ...   Variable argument list of the error message.
 * @return std::error_code.
 */
// NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-pro-type-vararg)
#define HWCPIPE_MAKE_ERROR_CODE(_e, ...)                                                                               \
    [&](hwcpipe::device::hwcpipe_errc e, __attribute__((unused)) auto &f) {                                            \
        std::error_code ec = std::make_error_code(e);                                                                  \
        if (__builtin_expect(!!(ec), 1)) {                                                                             \
            HWCPIPE_LOG_ERROR_CODE(f, __VA_ARGS__, "");                                                                \
        }                                                                                                              \
        return ec;                                                                                                     \
    }(_e, __FUNCTION__);
// NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-pro-type-vararg)
