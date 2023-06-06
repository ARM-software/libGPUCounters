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

#include <device/num_exec_engines.hpp>
#include <device/product_id.hpp>

#include <cassert>
#include <cstdint>
#include <system_error>

namespace hwcpipe {
namespace device {

static constexpr product_id product_g31{7, 3};
static constexpr product_id product_g51{7, 0};
static constexpr product_id product_g52{7, 2};
static constexpr product_id product_g57{9, 1};
static constexpr product_id product_g57_2{9, 3};
static constexpr product_id product_g68{9, 4};
static constexpr product_id product_g71{6, 0};
static constexpr product_id product_g72{6, 1};
static constexpr product_id product_g76{7, 1};
static constexpr product_id product_g77{9, 0};
static constexpr product_id product_g78{9, 2};
static constexpr product_id product_g78ae{9, 5};
static constexpr product_id product_g310{10, 4};
static constexpr product_id product_g510{10, 3};
static constexpr product_id product_g610{10, 7};
static constexpr product_id product_g710{10, 2};
static constexpr product_id product_g615{11, 3};
static constexpr product_id product_g715{11, 2};
static constexpr product_id product_g720{12, 0};
static constexpr product_id product_g720_2{12, 1};
static constexpr product_id product_g720_3{12, 2};

template <uint32_t pid_v>
static constexpr uint32_t max_registers(uint32_t raw_thread_features) {
    static_assert(pid_v == product_g31 || pid_v == product_g51, "Unsupported product_id for max_registers()");
    return raw_thread_features & 0xFFFF;
}

static constexpr uint16_t g31_g51_max_registers_small_core{0x2000};

uint8_t get_num_exec_engines(get_num_exec_engines_args &&args, std::error_code &ec) {
    auto const id = args.id;
    auto const core_count = args.core_count;
    auto const core_features = args.core_features;
    auto const thread_features = args.thread_features;

    uint8_t core_variant{};
    uint8_t ee_count{};

    switch (id) {
    case product_g31: {
        if (core_count == 1 &&
            max_registers<(uint32_t)product_g31>(thread_features) == g31_g51_max_registers_small_core)
            return 1;
        return 2;
    }
    case product_g51: {
        if (core_count == 1 &&
            max_registers<(uint32_t)product_g51>(thread_features) == g31_g51_max_registers_small_core)
            return 1;
        return 3;
    }
    case product_g52:
        ee_count = static_cast<uint8_t>(core_features & 0xF);
        return ee_count;
    // NOLINTNEXTLINE(bugprone-branch-clone)
    case product_g57:
    case product_g57_2:
        return 1;
    case product_g68:
        return 1;
    // NOLINTNEXTLINE(bugprone-branch-clone)
    case product_g71:
        return 3;
    // NOLINTNEXTLINE(bugprone-branch-clone)
    case product_g72:
        return 3;
    case product_g76:
        return 3;
    // NOLINTNEXTLINE(bugprone-branch-clone)
    case product_g77:
        return 1;
    case product_g78:
    case product_g78ae:
        return 1;
    case product_g310:
    case product_g510:
        core_variant = static_cast<uint8_t>(core_features & 0xF);
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
    case product_g610:
    case product_g710:
        return 2;
    case product_g615:
    case product_g715:
        core_variant = static_cast<uint8_t>(core_features & 0xF);
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
    case product_g720:
    case product_g720_2:
    case product_g720_3:
        core_variant = static_cast<uint8_t>(core_features & 0xF);
        switch (core_variant) {
        case 1:
            return 1;
        case 4:
            return 2;
        }
        ec = std::make_error_code(std::errc::not_supported);
        return 0;
    }

    if (id.get_gpu_family() == product_id::gpu_family::midgard)
        return 1;

    ec = std::make_error_code(std::errc::not_supported);
    return 0;
}

} // namespace device
} // namespace hwcpipe
