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
 * @file
 *
 * Sample handle detail private header.
 */

#pragma once

#include <array>
#include <cstdint>
#include <utility>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace detail {

/**
 * Handle detail class.
 *
 * Every @ref reader implementation uses handles to store some implementation
 * dependent data to represent a hardware counters sample or block being read. The rationale
 * behind this class is to know the handle size upfront so that the reader API
 * user can allocate it on stack.
 */
template <size_t size_v, size_t alignment_v>
class alignas(alignment_v) handle {
  public:
    /**
     * Get private sample handle.
     *
     * @tparam value_t Private sample handle type.
     * @return Private sample handle reference.
     */
    template <typename value_t>
    value_t &get() {
        check_alignment<value_t>();
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return *reinterpret_cast<value_t *>(data_.data());
    }

    /**
     * Get private sample handle.
     *
     * @tparam value_t Private sample handle type.
     * @return Private sample handle const reference.
     */
    template <typename value_t>
    const value_t &get() const {
        check_alignment<value_t>();
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return *reinterpret_cast<const value_t *>(data_.data());
    }

  private:
    template <typename value_t>
    void check_alignment() {
        // NOLINTNEXTLINE(bugprone-sizeof-expression)
        static_assert(sizeof(value_t) <= size_v, "handle cannot accommodate a value of this type.");
        static_assert(alignment_v > 0, "handle alignment must be > 0.");
        static_assert(alignment_v % alignof(value_t) == 0, "handle alignment disagrees with the value alignment.");
    }

    /** Sample handle memory. */
    std::array<uint8_t, size_v> data_{};

    template <size_t other_size_v, size_t other_alignment_v>
    friend inline bool operator==(const handle<other_size_v, other_alignment_v> &lhs,
                                  const handle<other_size_v, other_alignment_v> &rhs);
    template <size_t other_size_v, size_t other_alignment_v>
    friend inline bool operator!=(const handle<other_size_v, other_alignment_v> &lhs,
                                  const handle<other_size_v, other_alignment_v> &rhs);
};

/**
 * Equal compare sample handles.
 *
 * @param[in] lhs    Left hand side sample handle.
 * @param[in] rhs    Right hand side sample handle.
 * @return true if sample handles are equal, false otherwise.
 */
template <size_t size_v, size_t alignment_v>
inline bool operator==(const handle<size_v, alignment_v> &lhs, const handle<size_v, alignment_v> &rhs) {
    return lhs.data_ == rhs.data_;
}

/**
 * Not equal compare sample handles.
 *
 * @param[in] lhs    Left hand side sample handle.
 * @param[in] rhs    Right hand side sample handle.
 * @return true if sample handles are not equal, false otherwise.
 */

template <size_t size_v, size_t alignment_v>
inline bool operator!=(const handle<size_v, alignment_v> &lhs, const handle<size_v, alignment_v> &rhs) {
    return lhs.data_ != rhs.data_;
}
} // namespace detail
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
