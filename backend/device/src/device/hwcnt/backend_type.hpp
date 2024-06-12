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

/** @file backend_type.hpp */

#pragma once

#include <device/kbase_version.hpp>
#include <device/product_id.hpp>

#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>

namespace hwcpipe {
namespace device {
namespace hwcnt {

namespace detail {
class getenv_iface {
  public:
    static const char *getenv(const char *name) { return ::getenv(name); }
};
} // namespace detail

/**
 * HWCNT back-end types bit-mask.
 *
 * The back-end are listed in a priority order (highest to lowest).
 */
enum class backend_type : uint8_t {
    /** vinstr available. */
    vinstr,
    /** vinstr pre r21 available. */
    vinstr_pre_r21,
    /** kinstr_prfcnt available. */
    kinstr_prfcnt,
    /** kinstr_prfcnt workaround available. */
    kinstr_prfcnt_wa,
    /** kinstr_prfcnt bad available. */
    kinstr_prfcnt_bad,
    /** Sentinel. */
    last = kinstr_prfcnt_bad,
};

/** Supported back-end types set. */
using backend_types_set = std::bitset<static_cast<size_t>(backend_type::last) + 1>;

/**
 * Parse back-end type from string.
 *
 * @param[in] str String to parse, must not be NULL.
 * @return A pair of error code and back-end type parsed.
 */
std::pair<std::error_code, backend_type> backend_type_from_str(const char *str);

/**
 * Discover which HWCNT back-ends are available for a given kernel version / GPU product id.
 *
 * @param[in] version    Kbase version.
 * @param[in] pid        GPU product id.
 * @return HWCNT back-ends available.
 */
backend_types_set backend_type_discover(const kbase_version &version, product_id pid);

/**
 * Select HWCNT back-end to use.
 *
 * @param[in] available_types Available HWCNT back-end types.
 * @param[in] iface           Getenv iface (testing only).
 * @return A pair of error code and back-end selected.
 */
template <typename evnvar_iface_t = detail::getenv_iface>
inline std::pair<std::error_code, backend_type> backend_type_select(backend_types_set available_types,
                                                                    evnvar_iface_t &&iface = {}) {
    const char *iface_str = iface.getenv("HWCPIPE_BACKEND_INTERFACE");

    if (iface_str != nullptr) {
        backend_type desired_type{};
        std::error_code ec;

        std::tie(ec, desired_type) = backend_type_from_str(iface_str);

        if (ec)
            return std::make_pair(ec, backend_type{});

        backend_types_set desired_types_set{};
        desired_types_set.set(static_cast<size_t>(desired_type));

        available_types &= desired_types_set;
    }

    for (size_t i = 0; i < available_types.size(); ++i) {
        if (available_types.test(i))
            return std::make_pair(std::error_code{}, static_cast<backend_type>(i));
    }

    return std::make_pair(std::make_error_code(std::errc::function_not_supported), backend_type{});
}

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
