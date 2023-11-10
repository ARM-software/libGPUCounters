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

/**
 * @file is_empty_class.hpp is_empty traits header.
 */

#pragma once

namespace hwcpipe {
namespace device {
namespace detail {

/**
 * Checks if a class is an empty class.
 *
 * @note Classes with alignas attributes are not supported.
 *
 * @par Example
 * @code
 * class empty {};
 * static_assert(is_empty_class<empty>::value, "empty must be an empty class!");
 * @endcode
 */
template <typename value_t>
class is_empty_class {
    struct ebo : value_t {
        int var;
    };

  public:
    /** True if the class is empty, false otherwise. */
    static constexpr bool value{sizeof(ebo) == sizeof(int)};
};

/** True if value_t is an empty class, false otherwise. */
template <typename value_t>
constexpr bool is_empty_class_v{is_empty_class<value_t>::value};

} // namespace detail
} // namespace device
} // namespace hwcpipe
