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
 * Hardware counters blocks view header.
 */

#pragma once

#include <device/hwcnt/block_iterator.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {

/**
 *  Blocks view.
 *
 * Allows to iterate over blocks using range-for loop.
 *
 * Example:
 * @code
 * for (const auto &block: sample.blocks())
 *     handle_block(block);
 * @endcode
 */
class blocks_view {
  public:
    /** Iterator type. */
    using iterator_type = block_iterator;

    /**
     *  Constructor.
     *
     * @param[in] r             Hardware counters reader.
     * @param[in] sample_hndl   Sample handle to construct `blocks_view` for.
     */
    blocks_view(const reader &r, sample_handle sample_hndl)
        : reader_(r)
        , sample_hndl_(sample_hndl) {}

    /** @return Iterator pointing to the first block of a sample */
    iterator_type begin() { return iterator_type(reader_, sample_hndl_); }

    /** @return End iterator. */
    iterator_type end() { return iterator_type(); }

  private:
    const reader &reader_;
    sample_handle sample_hndl_;
};

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
