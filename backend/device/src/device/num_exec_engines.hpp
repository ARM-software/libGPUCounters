/*
 * Copyright (c) 2022-2024 Arm Limited.
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

#pragma once

#include <device/product_id.hpp>

#include <cstdint>
#include <system_error>

namespace hwcpipe {
namespace device {

/** Argument pack for \c get_num_exec_engines. */
struct get_num_exec_engines_args {
    product_id known_pid{};     //!< The GPU product to query.
    uint64_t core_count{};      //!< The number of cores of the GPU.
    uint64_t core_features{};   //!< The raw value of the CORE_FEATURES register.
    uint32_t thread_features{}; //!< The raw value of the THREAD_FEATURES register.
};

/** Decode the number of execution engines for a particular GPU.
 *
 * @param[in] args Argument pack, see \c get_num_exec_engines_args.
 * @param[in] ec   Error code.
 * @return The maximum number of execution engines per core.
 */
uint8_t get_num_exec_engines(get_num_exec_engines_args &&args, std::error_code &ec);

} // namespace device
} // namespace hwcpipe
