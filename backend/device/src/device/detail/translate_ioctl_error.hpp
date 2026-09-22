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
 * @file translate_ioctl_error.hpp
 *
 * Functions to handle the translation of ioctl std::error_code to a meaningful
 * @ref hwcpipe::device::hwcpipe_errc.
 */

#pragma once

#include <device/error.hpp>

namespace hwcpipe {
namespace device {
namespace detail {

/**
 * @brief Translate ioctl error codes to hwcpipe error codes.
 *
 * To remove ambiguity around system error codes coming out from the ioctl interface,
 * all ioctl errors are translated to meaningful @ref hwcpipe::device::hwcpipe_errc
 * error codes defined in @ref hwcpipe::device::hwcpipe_errc.
 *
 * @param[in] ec    std error code to translate.
 * @return          corresponding ioctl @ref hwcpipe::device::hwcpipe_errc
 */
inline std::error_code translate_ioctl_error(std::error_code ec) {
    if (ec == std::make_error_code(std::errc::operation_not_permitted))
        return std::make_error_code(hwcpipe_errc::ioctl_operation_not_permitted);
    else if (ec == std::make_error_code(std::errc::invalid_argument))
        return std::make_error_code(hwcpipe_errc::ioctl_invalid_argument);
    else if (ec == std::make_error_code(std::errc::bad_address))
        return std::make_error_code(hwcpipe_errc::ioctl_bad_address);
    else if (ec == std::make_error_code(std::errc::device_or_resource_busy))
        return std::make_error_code(hwcpipe_errc::ioctl_device_busy);
    else if (ec == std::make_error_code(std::errc::io_error))
        return std::make_error_code(hwcpipe_errc::ioctl_io_error);
    else if (ec == std::make_error_code(std::errc::function_not_supported))
        return std::make_error_code(hwcpipe_errc::ioctl_func_not_supported);
    else if (ec == std::make_error_code(std::errc::not_enough_memory))
        return std::make_error_code(hwcpipe_errc::ioctl_not_enough_memory);
    else if (ec == std::make_error_code(std::errc::bad_file_descriptor))
        return std::make_error_code(hwcpipe_errc::ioctl_bad_file_descriptor);

    // error is not handled, leave it as it is.
    return ec;
}

/**
 * @brief Overloading of @ref hwcpipe::device::detail::translate_ioctl_error to take std::errc.
 *
 * Takes an error value and passes it to @ref hwcpipe::device::detail::translate_ioctl_error with
 * a constructed std::error_code
 *
 * @param[in] ev    Enum error value.
 * @return          corresponding ioctl @ref hwcpipe::device::hwcpipe_errc
 */
inline std::error_code translate_ioctl_error(std::errc ev) { return translate_ioctl_error(std::make_error_code(ev)); }

} // namespace detail
} // namespace device
} // namespace hwcpipe
