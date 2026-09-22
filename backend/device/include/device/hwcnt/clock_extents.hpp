/*
 * Copyright (c) 2024-2026 Arm Limited.
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
#include <string>
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
enum class clock_type {
    gpu_cycle = 0,
    cg_cycle = 1,
    sc_cycle = 2,
    ne_cycle = 3,
    num_clock_types = 4,
};

class clock_extents {
  public:
    /**
     * Construct clock extents.
     *
     * @param[in] has_gpu_cycle  does GPU support GPU clock
     * @param[in] has_cg_cycle  does GPU support Core Group clock (MMU, L2 cache, Tile, etc.)
     * @param[in] has_sc_cycle  does GPU support Shader Core clock
     * @param[in] has_ne_cycle  does GPU support NE clock
     */
    clock_extents(bool has_gpu_cycle, bool has_cg_cycle, bool has_sc_cycle, bool has_ne_cycle)
        : clocks_{has_gpu_cycle, has_cg_cycle, has_sc_cycle, has_ne_cycle}
        , was_set(true) {}

    /** Default ctor. */
    clock_extents()
        : clocks_{}
        , was_set(false) {}

    /** Default copy ctor. */
    clock_extents(const clock_extents &) = default;
    /** Default assign. */
    clock_extents &operator=(const clock_extents &) = default;

    /** @return number of clocks. */
    uint16_t num_of_enabled_clocks() const {
        return static_cast<uint16_t>(static_cast<uint16_t>(clocks_[0]) + static_cast<uint16_t>(clocks_[1]) +
                                     static_cast<uint16_t>(clocks_[2]) + static_cast<uint16_t>(clocks_[3]));
    }

    /** @return is clock enabled. */
    bool has_clock(clock_type type) const { return clocks_[static_cast<size_t>(type)]; }

    /** @return is GPU cycle clock enabled. */
    bool has_gpu_cycle() const { return clocks_[static_cast<size_t>(clock_type::gpu_cycle)]; }

    /** @return is Core Group cycle clock enabled. */
    bool has_cg_cycle() const { return clocks_[static_cast<size_t>(clock_type::cg_cycle)]; }

    /** @return is Shader cycle clock enabled. */
    bool has_sc_cycle() const { return clocks_[static_cast<size_t>(clock_type::sc_cycle)]; }

    /** @return is Neural accelerator cycle clock enabled. */
    bool has_ne_cycle() const { return clocks_[static_cast<size_t>(clock_type::ne_cycle)]; }

    bool was_clock_extent_set() const { return was_set; }

    /** @return clock names in domain order */
    const std::vector<const char *> get_active_clock_strings() const {
        std::vector<const char *> ret;
        for (size_t i = 0; i < static_cast<size_t>(clock_type::num_clock_types); ++i) {
            if (clocks_[i])
                ret.push_back(get_clock_type_string(i));
        }
        return ret;
    }

    /** @return string representation of clock extents. */
    explicit operator std::string() const {
        std::string result = "Clocks=" + std::to_string(num_of_enabled_clocks());
        if (num_of_enabled_clocks() > 0)
            result += ":";
        for (size_t i = 0; i < static_cast<size_t>(clock_type::num_clock_types); ++i) {
            if (clocks_[i])
                result += " '" + std::string(get_clock_type_string(i)) + "'";
        }
        return result;
    }

  private:
    static const char *get_clock_type_string(size_t index) {
        switch (static_cast<clock_type>(index)) {
        case clock_type::gpu_cycle:
            return "Top cycle";
        case clock_type::cg_cycle:
            return "Core group";
        case clock_type::sc_cycle:
            return "Shader cores";
        case clock_type::ne_cycle:
            return "Neural accelerator";
        case clock_type::num_clock_types:
        default:
            return "Unknown";
        }

        __builtin_unreachable();
    }

    std::array<bool, static_cast<size_t>(clock_type::num_clock_types)> clocks_;
    bool was_set;
};

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
