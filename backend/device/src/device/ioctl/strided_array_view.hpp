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

#include "strided_array_iterator.hpp"

namespace hwcpipe {
namespace device {
namespace ioctl {

/** Strided array view. */
template <typename value_t>
class strided_array_view {
  public:
    /** Iterator type. */
    using iterator_type = strided_array_iterator<value_t>;
    /** Difference type. */
    using difference_type = typename iterator_type::difference_type;
    /** Pointer type. */
    using pointer = typename iterator_type::pointer;

    /**
     * Construct from pointer, stride and number of elements.
     *
     * @param[in] ptr             First element pointer.
     * @param[in] stride          Stride between the elements.
     * @param[in] num_elements    Number of elements in the array.
     */
    strided_array_view(pointer ptr, difference_type stride, size_t num_elements)
        : ptr_(ptr)
        , stride_(stride)
        , num_elements_(num_elements) {}

    /** Default copy constructor. */
    strided_array_view(const strided_array_view &) = default;
    /** Default assignment operator. */
    strided_array_view &operator=(const strided_array_view &) = default;

    iterator_type begin() { return iterator_type{ptr_, stride_}; }
    iterator_type end() { return begin() + static_cast<difference_type>(num_elements_); }

  private:
    pointer ptr_;
    difference_type stride_;
    size_t num_elements_;
};

namespace detail {
/** Arguments for constructing a strided array view. */
struct strided_array_view_args {
    ptrdiff_t stride;
    size_t num_elements;
};

template <typename value_t>
strided_array_view<value_t> operator|(value_t *ptr, const strided_array_view_args &args) {
    return strided_array_view<value_t>{ptr, args.stride, args.num_elements};
}
} // namespace detail

/**
 * Construct strided array view.
 *
 * @par Example
 * @code
 * struct my_struct {
 *     int field0;
 *     float field1;
 * };
 *
 *
 * std::array<my_struct, 128> array = get_array();
 *
 * // Will iterate over `field1` fields of the array elements.
 * for (auto element: &array[0].field1 | strided_array(sizeof(array[0]), array.size()))
 *     printf("field1 = %f\n", element);
 * @endcode
 *
 * @param stride Offset in bytes between successive elements.
 * @param[in] num_elements    Number of elements in the array.
 */
inline detail::strided_array_view_args strided_array(ptrdiff_t stride, size_t num_elements) {
    return detail::strided_array_view_args{stride, num_elements};
}

} // namespace ioctl
} // namespace device
} // namespace hwcpipe
