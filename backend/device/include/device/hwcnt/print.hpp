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
 * @file print.hpp
 *
 * Helper function for logging hwcnt block structures
 */

#pragma once

#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/features.hpp>

#include <iostream>

namespace hwcpipe {
namespace device {
namespace hwcnt {

inline std::ostream &operator<<(std::ostream &os, const block_type value) {
    switch (value) {
    case block_type::fe:
        return os << "FE";
    case block_type::geometry:
        return os << "Geometry";
    case block_type::memory:
        return os << "Memory";
    case block_type::core:
        return os << "Shader Core";
    case block_type::firmware:
        return os << "Firmware";
    case block_type::csg:
        return os << "CSG";
    case block_type::neural_accelerator:
        return os << "Neural Accelerator";
    default:
        return os << "Unknown";
    }

    __builtin_unreachable();
}

inline std::ostream &operator<<(std::ostream &os, const prfcnt_set set) {
    switch (set) {
    case prfcnt_set::primary:
        return os << "Primary";
    case prfcnt_set::secondary:
        return os << "Secondary";
    case prfcnt_set::tertiary:
        return os << "Tertiary";
    default:
        return os << "Unknown";
    }

    __builtin_unreachable();
}

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
