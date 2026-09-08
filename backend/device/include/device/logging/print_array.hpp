/*
 * Copyright (c) 2022-2025 Arm Limited.
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

/** @file print_array.hpp */

#pragma once

#include <ostream>

namespace hwcpipe {
namespace debug {

namespace detail {

/** Print array helper class. */
template <typename value_t>
class print_array {
  public:
    /**
     * View constructor.
     *
     * @param begin    Array begin.
     * @param size    Array size.
     */
    explicit print_array(const value_t *begin, size_t size)
        : begin_(begin)
        , size_(size) {}

    /** @return array begin iterator. */
    const value_t *begin() const { return begin_; }
    /** @return array end iterator. */
    const value_t *end() const { return begin_ + size_; }

  private:
    const value_t *begin_;
    const size_t size_;
};

template <typename value_t>
inline std::ostream &operator<<(std::ostream &os, const print_array<value_t> &array) {
    os << "{ ";
    for (const auto &element : array)
        os << element << ", ";

    return os << "}";
}
} // namespace detail

/**
 * Create array view that can be printed to ostream.
 *
 * @param array    Array to create view for.
 * @return printable array view.
 */
template <typename value_t, size_t size_v>
inline detail::print_array<value_t> print_array(value_t const (&array)[size_v]) {
    return detail::print_array<value_t>{array, size_v};
}

} // namespace debug
} // namespace hwcpipe
