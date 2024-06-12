/*
 * Copyright (c) 2022-2024 Arm Limited.
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

#include <device/num_exec_engines.hpp>
#include <device/product_id.hpp>

#include <cassert>
#include <cstdint>
#include <system_error>

namespace hwcpipe {
namespace device {

static constexpr uint32_t max_registers_mask = 0xFFFF;
static constexpr uint64_t exec_engines_mask = 0xF;

template <product_id pid_v>
static constexpr uint32_t max_registers(uint32_t raw_thread_features) {
    static_assert(pid_v == product_id::g31 || pid_v == product_id::g51, "Unsupported product_id for max_registers()");
    return raw_thread_features & max_registers_mask;
}

static constexpr uint16_t g31_g51_max_registers_small_core{0x2000};

uint8_t get_num_exec_engines(get_num_exec_engines_args &&args, std::error_code &ec) {
    auto const known_pid = args.known_pid;
    auto const core_count = args.core_count;
    auto const core_features = args.core_features;
    auto const thread_features = args.thread_features;

    uint8_t core_variant{};
    uint8_t ee_count{};

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, bugprone-branch-clone)
    switch (known_pid) {
    case product_id::t60x:
    case product_id::t62x:
    case product_id::t720:
    case product_id::t760:
    case product_id::t820:
        return 1;
    case product_id::t830:
    case product_id::t860:
        return 2;
    case product_id::t880:
        return 3;
    case product_id::g31: {
        if (core_count == 1 && max_registers<product_id::g31>(thread_features) == g31_g51_max_registers_small_core)
            return 1;
        return 2;
    }
    case product_id::g51: {
        if (core_count == 1 && max_registers<product_id::g51>(thread_features) == g31_g51_max_registers_small_core)
            return 1;
        return 3;
    }
    case product_id::g52:
        ee_count = static_cast<uint8_t>(core_features & exec_engines_mask);
        return ee_count;
    case product_id::g57:
    case product_id::g57_2:
        return 1;
    case product_id::g68:
        return 1;
    case product_id::g71:
        return 3;
    case product_id::g72:
        return 3;
    case product_id::g76:
        return 3;
    case product_id::g77:
        return 1;
    case product_id::g78:
    case product_id::g78ae:
        return 1;
    case product_id::g310:
        core_variant = static_cast<uint8_t>(core_features & exec_engines_mask);
        switch (core_variant) {
        case 0:
        case 1:
        case 5:
        case 6:
            return 1;
        case 2:
        case 3:
        case 4:
            return 2;
        }
        ec = std::make_error_code(std::errc::not_supported);
        return 0;
    case product_id::g510:
        core_variant = static_cast<uint8_t>(core_features & exec_engines_mask);
        switch (core_variant) {
        case 0:
        case 1:
            return 1;
        case 2:
        case 3:
        case 4:
            return 2;
        }
        ec = std::make_error_code(std::errc::not_supported);
        return 0;
    case product_id::g610:
    case product_id::g710:
        return 2;
    case product_id::g615:
    case product_id::g715:
        core_variant = static_cast<uint8_t>(core_features & exec_engines_mask);
        switch (core_variant) {
        case 0:
        case 1:
            return 1;
        case 2:
        case 3:
        case 4:
            return 2;
        }
        ec = std::make_error_code(std::errc::not_supported);
        return 0;
    case product_id::g720:
    case product_id::g620:
    case product_id::g725:
    case product_id::g625:
        core_variant = static_cast<uint8_t>(core_features & exec_engines_mask);
        switch (core_variant) {
        case 1:
            return 1;
        case 4:
            return 2;
        }
        ec = std::make_error_code(std::errc::not_supported);
        return 0;
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, bugprone-branch-clone)

    ec = std::make_error_code(std::errc::not_supported);
    return 0;
}

} // namespace device
} // namespace hwcpipe
