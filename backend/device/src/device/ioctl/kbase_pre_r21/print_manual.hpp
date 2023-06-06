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

#include "print.hpp"

#include <debug/ostream_indent.hpp>

#include <cassert>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace kbase_pre_r21 {

inline std::ostream &operator<<(std::ostream &os, const version_check_args &value) {
    // clang-format off
    return os << "version_check_args {\n" //
              << debug::indent_level::push //
              << debug::indent << ".header.id = " << value.header.id << ",\n" //
              << debug::indent << ".major = " << value.major << ",\n" //
              << debug::indent << ".minor = " << value.minor << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
    // clang-format on
}

inline std::ostream &operator<<(std::ostream &os, const set_flags_args &value) {
    // clang-format off
    return os << "version_check_args {\n" //
              << debug::indent_level::push //
              << debug::indent << ".header.id = " << value.header.id << ",\n" //
              << debug::indent << ".create_flags = " << value.create_flags << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
    // clang-format on
}

inline std::ostream &operator<<(std::ostream &os, const uk_gpuprops &value) {
    // clang-format off
    return os << "uk_gpuprops {\n" //
              << debug::indent_level::push //
              << debug::indent << ".header.id = " << value.header.id << ",\n" //
              << debug::indent << ".props = " << value.props << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
    // clang-format on
}

inline std::ostream &operator<<(std::ostream &os, const uk_hwcnt_reader_setup &value) {
    // clang-format off
    return os << "uk_hwcnt_reader_setup {\n" //
              << debug::indent_level::push //
              << debug::indent << ".header.id = " << value.header.id << ",\n" //
              << debug::indent << ".buffer_count = " << value.buffer_count << ",\n" //
              << debug::indent << ".jm_bm = " << value.jm_bm << ",\n" //
              << debug::indent << ".shader_bm = " << value.shader_bm << ",\n" //
              << debug::indent << ".tiler_bm = " << value.tiler_bm << ",\n" //
              << debug::indent << ".mmu_l2_bm = " << value.mmu_l2_bm << ",\n" //
              << debug::indent << ".fd = " << value.fd << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
    // clang-format on
}

} // namespace kbase_pre_r21
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
