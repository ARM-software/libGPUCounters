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

/** @file strided_array_iterator.hpp */

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace hwcpipe {
namespace device {
namespace ioctl {

/**
 * Strided array iterator.
 *
 * Allows to iterate over elements with a gap (stride) between them.
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
 * auto begin = strided_array_iterator<float>{&array[0].field1, sizeof(array[0])};
 * auto end = begin + array.size();
 *
 * // Will iterate over `field1` fields of the array elements.
 * for (auto it = begin; it != end; ++it)
 *     printf("field1 = %f\n", *it);
 * @endcode
 */
template <typename value_t>
class strided_array_iterator {
  public:
    /** Value type. */
    using value_type = value_t;
    /** Pointer type. */
    using pointer = std::add_pointer_t<value_t>;
    /** Reference type. */
    using reference = std::add_lvalue_reference_t<value_t>;
    /** Difference type. */
    using difference_type = ptrdiff_t;
    /** Iterator category. */
    using iterator_category = std::random_access_iterator_tag;

    /**
     * Construct from pointer and stride.
     *
     * @param[in] ptr       First element pointer.
     * @param[in] stride    Stride between elements.
     */
    template <typename stride_t>
    strided_array_iterator(pointer ptr, stride_t stride)
        : ptr_(ptr)
        , stride_(static_cast<difference_type>(stride)) {}
    /** Default copy constructor. */
    strided_array_iterator(const strided_array_iterator &) = default;
    /** Default assignment operator. */
    strided_array_iterator &operator=(const strided_array_iterator &) = default;

    strided_array_iterator &operator++() {
        ptr_ = advance(1);
        return *this;
    }

    strided_array_iterator operator++(int) {
        strided_array_iterator prev = *this;
        ++(*this);
        return prev;
    }

    strided_array_iterator &operator--() {
        ptr_ = advance(-1);
        return *this;
    }

    strided_array_iterator operator--(int) {
        strided_array_iterator prev = *this;
        --(*this);
        return prev;
    }

    reference operator[](difference_type idx) const { return *advance(idx); }
    strided_array_iterator operator+(difference_type rhs) { return {advance(rhs), stride_}; }
    strided_array_iterator operator-(difference_type rhs) { return {advance(-rhs), stride_}; }
    strided_array_iterator &operator+=(difference_type rhs) {
        ptr_ = advance(rhs);
        return *this;
    }
    strided_array_iterator &operator-=(difference_type rhs) {
        ptr_ = advance(-rhs);
        return *this;
    }

    difference_type operator-(const strided_array_iterator &rhs) {
        assert(rhs.stride_ == stride_);

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        char *lhs_addr = reinterpret_cast<char *>(ptr_);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        char *rhs_addr = reinterpret_cast<char *>(rhs.ptr_);

        difference_type diff = lhs_addr - rhs_addr;

        assert((diff % stride_) == 0);

        return diff / stride_;
    }

    reference operator*() const { return *ptr_; }
    pointer operator->() { return ptr_; }

  private:
    pointer advance(difference_type diff) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        auto addr = reinterpret_cast<difference_type>(ptr_);

        addr += diff * stride_;

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<pointer>(addr);
    }

    pointer ptr_;
    difference_type stride_;

    template <typename other_value_t>
    bool friend operator==(const strided_array_iterator<other_value_t> &lhs,
                           const strided_array_iterator<other_value_t> &rhs);

    template <typename other_value_t>
    bool friend operator!=(const strided_array_iterator<other_value_t> &lhs,
                           const strided_array_iterator<other_value_t> &rhs);
    template <typename other_value_t>
    bool friend operator<(const strided_array_iterator<other_value_t> &lhs,
                          const strided_array_iterator<other_value_t> &rhs);

    template <typename other_value_t>
    bool friend operator>(const strided_array_iterator<other_value_t> &lhs,
                          const strided_array_iterator<other_value_t> &rhs);
    template <typename other_value_t>
    bool friend operator<=(const strided_array_iterator<other_value_t> &lhs,
                           const strided_array_iterator<other_value_t> &rhs);

    template <typename other_value_t>
    bool friend operator>=(const strided_array_iterator<other_value_t> &lhs,
                           const strided_array_iterator<other_value_t> &rhs);
};

template <typename value_t>
inline bool operator==(const strided_array_iterator<value_t> &lhs, const strided_array_iterator<value_t> &rhs) {
    assert(lhs.stride_ == rhs.stride_);
    return lhs.ptr_ == rhs.ptr_;
}

template <typename value_t>
inline bool operator!=(const strided_array_iterator<value_t> &lhs, const strided_array_iterator<value_t> &rhs) {
    assert(lhs.stride_ == rhs.stride_);
    return lhs.ptr_ != rhs.ptr_;
}

template <typename value_t>
inline bool operator<(const strided_array_iterator<value_t> &lhs, const strided_array_iterator<value_t> &rhs) {
    assert(lhs.stride_ == rhs.stride_);
    return (lhs.ptr_ - rhs.ptr_) * lhs.stride_ < 0;
}

template <typename value_t>
inline bool operator>(const strided_array_iterator<value_t> &lhs, const strided_array_iterator<value_t> &rhs) {
    assert(lhs.stride_ == rhs.stride_);
    return (lhs.ptr_ - rhs.ptr_) * lhs.stride_ > 0;
}

template <typename value_t>
inline bool operator<=(const strided_array_iterator<value_t> &lhs, const strided_array_iterator<value_t> &rhs) {
    assert(lhs.stride_ == rhs.stride_);
    return (lhs.ptr_ - rhs.ptr_) * lhs.stride_ <= 0;
}

template <typename value_t>
inline bool operator>=(const strided_array_iterator<value_t> &lhs, const strided_array_iterator<value_t> &rhs) {
    assert(lhs.stride_ == rhs.stride_);
    return (lhs.ptr_ - rhs.ptr_) * lhs.stride_ >= 0;
}
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
