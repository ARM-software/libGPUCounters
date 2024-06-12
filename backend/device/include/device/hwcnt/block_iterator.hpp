/*
 * Copyright (c) 2021-2023 Arm Limited.
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
 * Hardware counters block iterator header.
 */

#pragma once

#include <device/hwcnt/reader.hpp>

#include <cassert>

namespace hwcpipe {
namespace device {
namespace hwcnt {

class reader;
class sample;

/**
 * Block iterator.
 *
 * Helper class allowing to iterate over hardware counters blocks
 * of a given sample.
 *
 * @code
 *
 * block_iterator begin{reader, sample_hndl};
 * block_iterator end{};
 * for (auto it = begin; it != end; ++it)
 *     handle_block(*it);
 *
 * @endcode
 */
class block_iterator {
  public:
    /** Value type. */
    using value_type = block_metadata;

    /** Pointer. */
    using pointer = const block_metadata *;

    /** Reference. */
    using reference = const block_metadata &;

    /** Default constructor. */
    block_iterator() = default;

    /**
     * Construct iterator pointing to the first block of a sample.
     *
     * @param[in] r             Hardware counters reader.
     * @param[in] sample_hndl   Sample handle to construct blocks begin iterator for.
     */
    block_iterator(const reader &r, sample_handle sample_hndl)
        : reader_(&r)
        , sample_hndl_(sample_hndl) {
        reader_ = reader_->next(sample_hndl_, metadata_, block_hndl_) ? reader_ : nullptr;
    }

    /** @return Hardware counters block reference. */
    reference operator*() const {
        assert(reader_);
        return metadata_;
    }

    /** @return Hardware counters block pointer. */
    pointer operator->() const {
        assert(reader_);
        return &metadata_;
    }

    /** @return Iterator pointing to the next hardware counters block. */
    class block_iterator &operator++() {
        assert(reader_);
        // NOLINTNEXTLINE
        reader_ = reader_->next(sample_hndl_, metadata_, block_hndl_) ? reader_ : nullptr;
        return *this;
    }

    /**  Advance the iterator and return the previous iterator value. */
    class block_iterator operator++(int) {
        block_iterator prev = *this;
        ++(*this);
        return prev;
    }

  private:
    friend bool operator==(const block_iterator &lhs, const block_iterator &rhs);
    friend bool operator!=(const block_iterator &lhs, const block_iterator &rhs);

    const reader *reader_{nullptr};
    sample_handle sample_hndl_{};
    block_handle block_hndl_{};
    block_metadata metadata_{};
};

/**
 * Equal compare two block iterators.
 *
 * @param[in] lhs    Left hand side iterator.
 * @param[in] rhs    Right hand side iterator.
 * @return true if the iterators are equal, false otherwise.
 */
inline bool operator==(const block_iterator &lhs, const block_iterator &rhs) {
    // When reader_ is nullptr, we reached the end.
    if (lhs.reader_ == nullptr && rhs.reader_ == nullptr)
        return true;

    return lhs.reader_ == rhs.reader_              //
           && lhs.sample_hndl_ == rhs.sample_hndl_ //
           && lhs.block_hndl_ == rhs.block_hndl_;
}

/**
 * Not-equal compare two block iterators.
 *
 * @param[in] lhs    Left hand side iterator.
 * @param[in] rhs    Right hand side iterator.
 * @return true if the iterators are not equal, false otherwise.
 */
inline bool operator!=(const block_iterator &lhs, const block_iterator &rhs) { return !(lhs == rhs); }

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
