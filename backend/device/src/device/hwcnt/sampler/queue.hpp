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
 * @file queue.hpp
 *
 * Fixed size ring buffer queue class.
 */

#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {

/**
 * Fixed size ring buffer queue.
 *
 * @warning `size_v` must be a power of two.
 */
template <typename value_t, size_t size_v>
class queue {
  public:
    /** Value type. */
    using value_type = value_t;

    /** Maximum size this queue can store. */
    static constexpr size_t max_size_value = size_v;

    /** @return element popped from the queue. */
    value_type pop() {
        assert(!empty());

        const auto result = front();

        ++pop_idx_;

        return result;
    }

    /**
     * Push value to the queue.
     *
     * @param[in] value Value to push to the queue.
     */
    void push(const value_type &value) {
        assert(!full());

        ++push_idx_;

        back() = value;
    }

    /** @return Front element reference. */
    value_type &front() { return front_impl(*this); }

    /** @return Front element const reference. */
    const value_type &front() const { return front_impl(*this); }

    /** @return Back element reference. */
    value_type &back() { return back_impl(*this); }

    /** @return Back element const reference. */
    const value_type &back() const { return back_impl(*this); }

    /** @return queue size. */
    uint32_t size() const { return push_idx_ - pop_idx_; }

    /** @return true if queue is empty, false otherwise. */
    bool empty() const { return size() == 0; }

    /** @return true if queue is full, false otherwise. */
    bool full() const { return size() == max_size_value; }

    /** @return Number of times push() was called. */
    uint64_t push_count() const { return push_idx_; }

    /** @return Number of times pop() was called. */
    uint64_t pop_count() const { return pop_idx_; }

    /**
     * Get n'th value in the queue from the front.
     *
     * @param[in] index    Value index.
     * @return Value at index.
     */
    value_type &operator[](size_t index) { return lookup_impl(*this, index); }

    /**
     * Get n'th value in the constant queue from the front.
     *
     * @param[in] index    Value index.
     * @return Value at index.
     */
    const value_type &operator[](size_t index) const { return lookup_impl(*this, index); }

  private:
    template <typename queue_t>
    static auto &front_impl(queue_t &q) {
        assert(!q.empty());

        const auto pop_idx = q.pop_idx_ % queue_t::max_size_value;
        return q.elements_[pop_idx];
    }

    template <typename queue_t>
    static auto &back_impl(queue_t &q) {
        assert(!q.empty());

        const auto push_idx = (q.push_idx_ - 1) % queue_t::max_size_value;
        return q.elements_[push_idx];
    }

    template <typename queue_t>
    static auto &lookup_impl(queue_t &q, size_t index) {
        assert(index < q.size());

        const auto elements_index = (q.pop_idx_ + index) % queue_t::max_size_value;
        return q.elements_[elements_index];
    }

    /** Push index. */
    uint32_t push_idx_{};
    /** Pop index. */
    uint32_t pop_idx_{};
    /** Queue elements. */
    std::array<value_t, max_size_value> elements_{};
};

} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
