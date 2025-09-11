/*
 * Copyright (c) 2024 Arm Limited.
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
 * Hardware counters clock extents header.
 */

#pragma once

#include <device/hwcnt/features.hpp>

#include <array>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <map>
#include <type_traits>
#include <vector>

namespace hwcpipe {
namespace device {
namespace hwcnt {

/**
 * Clocks extents class.
 *
 * Stores information about clock numbers and clock names
 */
class clock_extents {
  public:
    /** Number of clock types. GPU Cycle Clock and SC Cycle Clock */
    static const constexpr size_t num_clock_types = 2;

    using num_clock_types_strings_type = std::array<const char *, num_clock_types>;

    /**
     * Construct clock extents.
     *
     * @param[in] has_gpu_cycle  does GPU support GPU clock
     * @param[in] has_sc_cycle  does GPU support SC clock
     */
    clock_extents(bool has_gpu_cycle, bool has_sc_cycle) {
        was_set = true;
        has_gpu_cycle_ = false;
        has_sc_cycle_ = false;

        if (has_gpu_cycle) {
            has_gpu_cycle_ = true;
        }
        if (has_sc_cycle) {
            has_sc_cycle_ = true;
        }
    }

    /** Default ctor. */
    clock_extents()
        : has_gpu_cycle_(false)
        , has_sc_cycle_(false)
        , was_set(false){};
    /** Default copy ctor. */
    clock_extents(const clock_extents &) = default;
    /** Default assign. */
    clock_extents &operator=(const clock_extents &) = default;

    /** @return number of clocks. */
    uint16_t num_of_enabled_clocks() const { return static_cast<uint16_t>(has_gpu_cycle_ + has_sc_cycle_); }

    /** @return is GPU cycle clock enabled. */
    bool has_gpu_cycle() const { return has_gpu_cycle_; }

    /** @return is Shader cycle clock enabled. */
    bool has_sc_cycle() const { return has_sc_cycle_; }

    bool was_clock_extent_set() const { return was_set; }

    /** @return clock names in domain order */
    const std::vector<const char *> get_active_clock_strings() const {
        std::vector<const char *> ret;
        if (has_gpu_cycle_)
            ret.push_back(clock_types_strings[gpu_cycle_idx]);
        if (has_sc_cycle_)
            ret.push_back(clock_types_strings[sc_cycle_idx]);
        return ret;
    };

  private:
    bool has_gpu_cycle_;
    bool has_sc_cycle_;
    bool was_set;
    static constexpr size_t gpu_cycle_idx = 0;
    static constexpr size_t sc_cycle_idx = 1;

    static const constexpr num_clock_types_strings_type clock_types_strings = {
        "Top cycle",
        "Shader cores",
    };
};

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
