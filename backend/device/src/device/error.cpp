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
 * @file error.cpp
 *
 * Error handling of hwcpipe.
 */
#include <device/error.hpp>

namespace hwcpipe {
namespace device {

const hwcpipe_error_category_t hwcpipe_error_category{};

const char *hwcpipe_error_category_t::name() const noexcept { return "hwcpipe2"; }

std::string get_error_message(std::error_code &ec) { return ec.category().message(ec.value()); }

bool hwcpipe_error_category_t::equivalent(const std::error_code &ec, int condition) const noexcept {
    return *this == ec.category() && static_cast<int>(default_error_condition(ec.value()).value()) == condition;
}

std::string hwcpipe_error_category_t::message(int ev) const {
    switch (static_cast<hwcpipe_errc>(ev)) {
    /* Success */
    case hwcpipe_errc::success:
        return "Success";

    /* Product ID errors */
    case hwcpipe_errc::invalid_product_id:
        return "[Product ID] Invalid product id";

    /* Instance initialization errors */
    case hwcpipe_errc::invalid_kernel_version:
        return "[Instance] Invalid kernel version";
    case hwcpipe_errc::warp_invalid_product_id:
        return "[Instance] Missing product id in warp width calculation";
    case hwcpipe_errc::prop_reader_invalid_data:
        return "[Instance] Invalid data size from Property reader";
    case hwcpipe_errc::failed_to_create_sampler:
        return "[Instance] Failed to create sampler";
    case hwcpipe_errc::invalid_num_exec_engines:
        return "[EXEC_ENGINES] Failed to calculate number of execution engines";

    /* Backend errors */
    case hwcpipe_errc::backend_start_invalid_buffer_size:
        return "[Backend] Failed to start backend: Invalid number of samples in hwc buffer";
    case hwcpipe_errc::backend_fd_error:
        return "[Backend] Failed to access/create an fd";

    /* Sampler errors */
    case hwcpipe_errc::backend_invalid_type:
        return "[Sampler] Failed to get backend type";
    case hwcpipe_errc::sampler_invalid_type:
        return "[Sampler] Invalid sampler type";
    case hwcpipe_errc::invalid_buffer_size:
        return "[Sampler] Invalid number of samples in hwc buffer";
    case hwcpipe_errc::sampler_invalid_state:
        return "[Sampler] Invalid sampler state, sampler is either not started or in progress";
    case hwcpipe_errc::sampler_max_sessions:
        return "[Sampler] Reached maximum number of periodic sampler sessions";
    case hwcpipe_errc::invalid_sample:
        return "[Sampler] Invalid sample received";
    case hwcpipe_errc::sampler_invalid_sc_remap:
        return "[Sampler] Invalid remapping of shader core indices";
    case hwcpipe_errc::sampler_timeout:
        return "[Sampler] Sampler timed out waiting for sample";

    /* Block extents */
    case hwcpipe_errc::extents_invalid_block_type:
        return "[Block Extents] Invalid block type when parsing block extents";
    case hwcpipe_errc::extents_invalid_set:
        return "[Block Extents] Invalid counter set when parsing block extents";
    case hwcpipe_errc::extents_invalid_num_values:
        return "[Block Extents] Invalid number of sample values when parsing block extents";
    case hwcpipe_errc::extents_invalid_block:
        return "[Block Extents] Invalid number of blocks when parsing block extents";
    case hwcpipe_errc::extents_invalid_num_instances:
        return "[Block Extents] Invalid number of block instances when parsing block extents";
    case hwcpipe_errc::extents_version_not_supported:
        return "[Block Extents] prfcnt api version not supported";
    case hwcpipe_errc::extents_duplicate_request_type:
        return "[Block Extents] Duplicate request type when parsing block extents";
    case hwcpipe_errc::extents_duplicate_sample_info:
        return "[Block Extents] Duplicate sample info when parsing block extents";
    case hwcpipe_errc::extents_invalid_num_clock_domains:
        return "[Block Extents] Invalid number of clock domains when parsing block extents";

    /* Sample metadata */
    case hwcpipe_errc::meta_sample_already_parsed:
        return "[Sampler Metadata] Sample metadata already parsed";
    case hwcpipe_errc::meta_clock_already_parsed:
        return "[Sampler Metadata] Clock metadata already parsed";
    case hwcpipe_errc::meta_invalid_block_type:
        return "[Sampler Metadata] Invalid block type when parsing sample metadata";
    case hwcpipe_errc::meta_invalid_block_index:
        return "[Sampler Metadata] Invalid block index when parsing sample metadata";
    case hwcpipe_errc::meta_invalid_num_blocks:
        return "[Sampler Metadata] Invalid number of blocks when parsing sample metadata";

    /* Parser errors */
    case hwcpipe_errc::parser_not_done:
        return "[Parser] Failed to finalize parsing data";
    case hwcpipe_errc::parser_invalid_order:
        return "[Parser] Invalid order of parsing data (extents/sample metadata)";
    case hwcpipe_errc::parser_invalid_data:
        return "[Parser] Invalid data";
    case hwcpipe_errc::parser_invalid_argument:
        return "[Parser] Invalid argument when calling dispatch_on_item";

    /* syscalls */
    case hwcpipe_errc::ioctl_operation_not_permitted:
        return "[IOCTL] Ioctl device returned (EPERM)";
    case hwcpipe_errc::ioctl_invalid_argument:
        return "[IOCTL] Ioctl device returned (EINVAL)";
    case hwcpipe_errc::ioctl_device_busy:
        return "[IOCTL] Ioctl device returned (EBUSY)";
    case hwcpipe_errc::ioctl_bad_address:
        return "[IOCTL] Ioctl device returned (EFAULT)";
    case hwcpipe_errc::ioctl_io_error:
        return "[IOCTL] Ioctl device returned (EIO)";
    case hwcpipe_errc::ioctl_func_not_supported:
        return "[IOCTL] Ioctl device returned (ENOSYS)";
    case hwcpipe_errc::ioctl_not_enough_memory:
        return "[IOCTL] Ioctl device returned (ENOMEM)";
    case hwcpipe_errc::ioctl_bad_file_descriptor:
        return "[IOCTL] Ioctl device returned (EBADF)";
    default:
        return "[HWCPIPE2] (unrecognized error)";
    }
}

} // namespace device
} // namespace hwcpipe

namespace std {
std::error_code make_error_code(hwcpipe::device::hwcpipe_errc e) {
    return {static_cast<int>(e), hwcpipe::device::hwcpipe_error_category};
}

} // namespace std
