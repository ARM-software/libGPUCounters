/*
 * Copyright (c) 2026 Arm Limited.
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

/** @file strided_array_reader.hpp */

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <type_traits>

namespace hwcpipe {
namespace device {
namespace ioctl {

/**
 * Strided array reader.
 *
 * Allows to read strided elements from a byte buffer with an element size that
 * can differ from the userspace type size. This supports backward/forward
 * compatibility when the kernel ABI changes element sizes.
 */
template <typename value_t>
class strided_array_reader {
  public:
    /**
     * Construct from pointer, stride, element size and number of elements.
     *
     * @param[in] ptr            Base pointer to the first element.
     * @param[in] stride         Stride between elements in bytes.
     * @param[in] element_size   Element size in bytes as reported by the kernel.
     * @param[in] num_elements   Number of elements in the array.
     */
    strided_array_reader(const void *ptr, std::size_t stride, std::size_t element_size, std::size_t num_elements)
        : ptr_(static_cast<const unsigned char *>(ptr))
        , stride_(stride)
        , element_size_(element_size)
        , num_elements_(num_elements) {
        static_assert(std::is_trivially_copyable<value_t>::value,
                      "strided_array_reader requires trivially copyable value types");
        assert(element_size_ <= stride_);
    }

    /** Read an element by value, zero-padding any missing fields. */
    value_t load(std::ptrdiff_t idx) const {
        value_t value{};
        const auto *addr = element_base(idx);
        const auto copy_size = std::min(sizeof(value), element_size_);
        std::memcpy(&value, addr, copy_size);
        return value;
    }

    /** @return Pointer to the end of an element using kernel-reported size. */
    const unsigned char *field_end(std::ptrdiff_t idx) const { return element_base(idx) + element_size_; }

    /** @return Kernel-reported element size in bytes. */
    std::size_t element_size() const { return element_size_; }

    /** @return Stride in bytes. */
    std::size_t stride() const { return stride_; }

    /** @return Number of elements. */
    std::size_t size() const { return num_elements_; }

  private:
    /** @return Pointer to the start of an element. */
    const unsigned char *element_base(std::ptrdiff_t idx) const {
        assert(idx >= 0);
        assert(static_cast<std::size_t>(idx) < num_elements_);
        return ptr_ + static_cast<std::size_t>(idx) * stride_;
    }

    const unsigned char *ptr_;
    std::size_t stride_;
    std::size_t element_size_;
    std::size_t num_elements_;
};

} // namespace ioctl
} // namespace device
} // namespace hwcpipe
