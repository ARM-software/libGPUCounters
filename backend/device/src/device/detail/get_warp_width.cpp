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

#include "get_warp_width.hpp"

#include <device/error.hpp>

namespace hwcpipe {
namespace device {
namespace detail {

uint64_t get_warp_width(product_id known_pid, std::error_code &ec) {
    switch (known_pid) {
    /* Midgard, no warp hardware */
    case product_id::t60x:
    case product_id::t62x:
    case product_id::t720:
    case product_id::t760:
    case product_id::t820:
    case product_id::t830:
    case product_id::t860:
    case product_id::t880:
        return 1;
    /* Bifrost - Valhall */
    case product_id::g31:
    case product_id::g51:
    case product_id::g71:
    case product_id::g72:
        return 4;
    case product_id::g52:
    case product_id::g76:
        return 8;
    case product_id::g68:
    case product_id::g57:
    case product_id::g57_2:
    case product_id::g77:
    case product_id::g78:
    case product_id::g78ae:
    case product_id::g310:
    case product_id::g510:
    case product_id::g610:
    case product_id::g710:
    case product_id::g615:
    case product_id::g715:
    /* 5th Gen */
    case product_id::g720:
    case product_id::g620:
    case product_id::g725:
    case product_id::g625:
    case product_id::g1_ultra:
    case product_id::g1_premium:
    case product_id::g1_pro:
    case product_id::g2_ultra:
    case product_id::g2_premium:
    case product_id::g2_pro:
        /* 6th Gen */
        return 16;
    }

    ec = HWCPIPE_MAKE_ERROR_CODE(hwcpipe_errc::warp_invalid_product_id, "Product: %s",
                                 product_id_name(known_pid).c_str());
    return 0;
}

} // namespace detail
} // namespace device
} // namespace hwcpipe
