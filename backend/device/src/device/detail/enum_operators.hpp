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

/** @file enum_operators.hpp */

#pragma once

#include <type_traits>

namespace hwcpipe {
namespace device {
namespace detail {
namespace enum_operators {

template <typename enum_t>
auto to_underlying(enum_t value) {
    using underlying_type = typename std::underlying_type<enum_t>::type;
    return static_cast<underlying_type>(value);
}

template <typename enum_t>
inline typename std::enable_if<std::is_enum<enum_t>::value, enum_t &>::type operator++(enum_t &value) {
    auto raw_value = to_underlying(value);
    ++raw_value;
    value = static_cast<enum_t>(raw_value);
    return value;
}

template <typename enum_t>
inline typename std::enable_if<std::is_enum<enum_t>::value, enum_t &>::type operator--(enum_t &value) {
    auto raw_value = to_underlying(value);
    --raw_value;
    value = static_cast<enum_t>(raw_value);
    return value;
}

template <typename enum_t>
inline typename std::enable_if<std::is_enum<enum_t>::value, enum_t>::type operator++(enum_t &value, int) {
    const auto prev = value;
    ++value;
    return prev;
}

template <typename enum_t>
inline typename std::enable_if<std::is_enum<enum_t>::value, enum_t>::type operator--(enum_t &value, int) {
    const auto prev = value;
    --value;
    return prev;
}

} // namespace enum_operators
} // namespace detail
} // namespace device
} // namespace hwcpipe
