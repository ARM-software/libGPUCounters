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
 * Functions to obtain a GPU product ID and query product information.
 */

#include <device/product_id.hpp>

#include <cassert>

namespace hwcpipe {
namespace device {

/**
 * Return true if the GPU_ID is 64 bit
 *
 * @param[in] gpu_id    Raw gpu id.
 * @return True if GPU_ID is 64 bit, False if it is 32 bit.
 */
static constexpr bool gpu_id_is_64bit(uint64_t gpu_id) {
    /* Compat used to identify 64 bit GPU ID */
    constexpr uint64_t compat{0xF};
    constexpr int compat_shift{28};

    return (((gpu_id >> compat_shift) & compat) == compat);
}

/**
 * Make a hash value using a product/arch major versions pair.
 *
 * @param[in] arch_major       Arch major version.
 * @param[in] product_major    Product major version.
 * @return Product id value.
 */
static constexpr uint32_t hash_from_versions(uint32_t arch_major, uint32_t product_major) {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    return (arch_major << 8) | product_major;
}

std::pair<std::error_code, product_id> product_id_from_raw_gpu_id(uint64_t raw_gpu_id) {
    const uint64_t old_product_id = raw_gpu_id >> 16;
    static constexpr uint16_t old_version_max_value{0x1000U};

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    if (old_product_id == 0x6956U || old_product_id < old_version_max_value) {
        assert(!gpu_id_is_64bit(raw_gpu_id));

        switch (old_product_id) {
        /* Midgard */
        case 0x6956U:
            return std::make_pair(std::error_code{}, product_id::t60x);
        case 0x0620U:
            return std::make_pair(std::error_code{}, product_id::t62x);
        case 0x0720U:
            return std::make_pair(std::error_code{}, product_id::t720);
        case 0x0750U:
            return std::make_pair(std::error_code{}, product_id::t760);
        case 0x0820U:
            return std::make_pair(std::error_code{}, product_id::t820);
        case 0x0830U:
            return std::make_pair(std::error_code{}, product_id::t830);
        case 0x0860U:
            return std::make_pair(std::error_code{}, product_id::t860);
        case 0x0880U:
            return std::make_pair(std::error_code{}, product_id::t880);
        default:
            return std::make_pair(std::make_error_code(std::errc::invalid_argument), product_id{});
        }
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
    } else {
        uint32_t arch_major = 0, product_major = 0;

        if (gpu_id_is_64bit(raw_gpu_id)) {
            // Taken from GPU_ID register
            // 8 bits per field
            constexpr int arch_major_offset = 56;
            constexpr int product_major_offset = 32;
            constexpr int full_byte = 0xFF;
            arch_major = static_cast<uint32_t>((raw_gpu_id >> arch_major_offset) & full_byte);
            product_major = static_cast<uint32_t>((raw_gpu_id >> product_major_offset) & full_byte);
        } else {
            // 4 bits per field
            constexpr int arch_major_offset = 28;
            constexpr int product_major_offset = 16;
            constexpr int nibble = 0xF;
            arch_major = (raw_gpu_id >> arch_major_offset) & nibble;
            product_major = (raw_gpu_id >> product_major_offset) & nibble;
        }

        switch (hash_from_versions(arch_major, product_major)) {
        // Values mapped from GPU_ID register from architecture spec
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
        /* Bifrost */
        case hash_from_versions(7, 3):
            return std::make_pair(std::error_code{}, product_id::g31);
        case hash_from_versions(7, 0):
            return std::make_pair(std::error_code{}, product_id::g51);
        case hash_from_versions(7, 2):
            return std::make_pair(std::error_code{}, product_id::g52);
        case hash_from_versions(6, 0):
            return std::make_pair(std::error_code{}, product_id::g71);
        case hash_from_versions(6, 1):
            return std::make_pair(std::error_code{}, product_id::g72);
        case hash_from_versions(7, 1):
            return std::make_pair(std::error_code{}, product_id::g76);
        /* Valhall */
        case hash_from_versions(9, 1):
            return std::make_pair(std::error_code{}, product_id::g57);
        case hash_from_versions(9, 3):
            return std::make_pair(std::error_code{}, product_id::g57_2);
        case hash_from_versions(9, 4):
            return std::make_pair(std::error_code{}, product_id::g68);
        case hash_from_versions(9, 0):
            return std::make_pair(std::error_code{}, product_id::g77);
        case hash_from_versions(9, 2):
            return std::make_pair(std::error_code{}, product_id::g78);
        case hash_from_versions(9, 5):
            return std::make_pair(std::error_code{}, product_id::g78ae);
        case hash_from_versions(10, 4):
            return std::make_pair(std::error_code{}, product_id::g310);
        case hash_from_versions(10, 3):
            return std::make_pair(std::error_code{}, product_id::g510);
        case hash_from_versions(10, 7):
            return std::make_pair(std::error_code{}, product_id::g610);
        case hash_from_versions(11, 3):
            return std::make_pair(std::error_code{}, product_id::g615);
        case hash_from_versions(10, 2):
            return std::make_pair(std::error_code{}, product_id::g710);
        case hash_from_versions(11, 2):
            return std::make_pair(std::error_code{}, product_id::g715);
        /* 5th Gen */
        case hash_from_versions(12, 0):
            return std::make_pair(std::error_code{}, product_id::g720);
        case hash_from_versions(12, 1):
            return std::make_pair(std::error_code{}, product_id::g620);
        case hash_from_versions(13, 0):
            return std::make_pair(std::error_code{}, product_id::g725);
        case hash_from_versions(13, 1):
            return std::make_pair(std::error_code{}, product_id::g625);

        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
        default:
            return std::make_pair(std::make_error_code(std::errc::invalid_argument), product_id{});
        }
    }
    __builtin_unreachable();
}

gpu_family get_gpu_family(const product_id pid) {
    switch (pid) {
    case product_id::t60x:
    case product_id::t62x:
    case product_id::t720:
    case product_id::t760:
    case product_id::t820:
    case product_id::t830:
    case product_id::t860:
    case product_id::t880:
        return gpu_family::midgard;

    case product_id::g31:
    case product_id::g51:
    case product_id::g52:
    case product_id::g71:
    case product_id::g72:
    case product_id::g76:
        return gpu_family::bifrost;

    /* Valhall */
    case product_id::g57:
    case product_id::g57_2:
    case product_id::g68:
    case product_id::g77:
    case product_id::g78:
    case product_id::g78ae:
    case product_id::g310:
    case product_id::g510:
    case product_id::g610:
    case product_id::g615:
    case product_id::g710:
    case product_id::g715:
        return gpu_family::valhall;

    case product_id::g720:
    case product_id::g620:
    case product_id::g725:
    case product_id::g625:
        return gpu_family::fifthgen;
    }
    __builtin_unreachable();
}

gpu_frontend get_gpu_frontend(const product_id pid) {
    switch (pid) {
    case product_id::t60x:
    case product_id::t62x:
    case product_id::t720:
    case product_id::t760:
    case product_id::t820:
    case product_id::t830:
    case product_id::t860:
    case product_id::t880:
    case product_id::g31:
    case product_id::g51:
    case product_id::g52:
    case product_id::g71:
    case product_id::g72:
    case product_id::g76:
    case product_id::g57:
    case product_id::g57_2:
    case product_id::g68:
    case product_id::g77:
    case product_id::g78:
    case product_id::g78ae:
        return gpu_frontend::jm;

    case product_id::g310:
    case product_id::g510:
    case product_id::g610:
    case product_id::g615:
    case product_id::g710:
    case product_id::g715:
    case product_id::g720:
    case product_id::g620:
    case product_id::g725:
    case product_id::g625:
        return gpu_frontend::csf;
    }
    __builtin_unreachable();
}

} // namespace device
} // namespace hwcpipe
