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

#pragma once

#include "compare.hpp"

#include <cassert>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace kbase_pre_r21 {

bool operator==(const version_check_args &lhs, const version_check_args &rhs) {
    return (lhs.header.id == rhs.header.id) //
           && (lhs.major == rhs.major)      //
           && (lhs.minor == rhs.minor);     //
}

bool operator==(const set_flags_args &lhs, const set_flags_args &rhs) {
    return (lhs.header.id == rhs.header.id)           //
           && (lhs.create_flags == rhs.create_flags); //
}

bool operator==(const uk_gpuprops &lhs, const uk_gpuprops &rhs) {
    return (lhs.header.id == rhs.header.id) //
           && (lhs.props == rhs.props);     //
}

bool operator==(const uk_hwcnt_reader_setup &lhs, const uk_hwcnt_reader_setup &rhs) {
    return (lhs.header.id == rhs.header.id)          //
           && (lhs.buffer_count == rhs.buffer_count) //
           && (lhs.jm_bm == rhs.jm_bm)               //
           && (lhs.shader_bm == rhs.shader_bm)       //
           && (lhs.tiler_bm == rhs.tiler_bm)         //
           && (lhs.mmu_l2_bm == rhs.mmu_l2_bm)       //
           && (lhs.fd == rhs.fd);                    //
}

} // namespace kbase_pre_r21
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
