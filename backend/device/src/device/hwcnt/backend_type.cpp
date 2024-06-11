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

#include <device/hwcnt/backend_type.hpp>
#include <device/kbase_version.hpp>
#include <device/product_id.hpp>

#include <cstdint>
#include <cstring>

namespace hwcpipe {
namespace device {
namespace hwcnt {

static bool is_gtux_or_later(product_id pid) {
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
    case product_id::g57:
    case product_id::g57_2:
    case product_id::g68:
    case product_id::g71:
    case product_id::g72:
    case product_id::g76:
    case product_id::g77:
    case product_id::g78:
    case product_id::g78ae:
    case product_id::g310:
    case product_id::g510:
    case product_id::g610:
    case product_id::g710:
        return false;
    default:
        return true;
    }
    __builtin_unreachable();
}

static bool is_vinstr_available(const kbase_version &version, product_id pid) {
    if (is_gtux_or_later(pid))
        return false;

    static constexpr kbase_version jm_max_version{11, 40, ioctl_iface_type::jm_post_r21};
    static constexpr kbase_version csf_max_version{1, 21, ioctl_iface_type::csf};

    switch (version.type()) {
    case ioctl_iface_type::jm_pre_r21:
        return true;
    case ioctl_iface_type::jm_post_r21:
        return version < jm_max_version;
    case ioctl_iface_type::csf:
        return version < csf_max_version;
    }

    __builtin_unreachable();
}

static bool is_kinstr_prfcnt_available(const kbase_version &version) {
    static constexpr kbase_version jm_min_version{11, 37, ioctl_iface_type::jm_post_r21};
    static constexpr kbase_version csf_min_version{1, 17, ioctl_iface_type::csf};

    const auto type = version.type();

    /* kinstr_prfcnt becomes available after these versions. */
    if ((type == ioctl_iface_type::jm_post_r21 && version >= jm_min_version) ||
        (type == ioctl_iface_type::csf && version >= csf_min_version))
        return true;

    return false;
}

static bool is_kinstr_prfcnt_bad_available(const kbase_version &version) {
    if (is_kinstr_prfcnt_available(version))
        return false;

    static constexpr kbase_version jm_min_version{11, 34, ioctl_iface_type::jm_post_r21};
    static constexpr kbase_version csf_min_version{1, 10, ioctl_iface_type::csf};

    const auto type = version.type();

    /* kinstr_prfcnt_bad becomes available after these versions. */
    if ((type == ioctl_iface_type::jm_post_r21 && version >= jm_min_version) ||
        (type == ioctl_iface_type::csf && version >= csf_min_version))
        return true;

    return false;
}

std::pair<std::error_code, backend_type> backend_type_from_str(const char *str) {
    assert(str);

    if (!strcmp(str, "vinstr"))
        return std::make_pair(std::error_code{}, backend_type::vinstr);
    if (!strcmp(str, "vinstr_pre_r21"))
        return std::make_pair(std::error_code{}, backend_type::vinstr_pre_r21);
    if (!strcmp(str, "kinstr_prfcnt"))
        return std::make_pair(std::error_code{}, backend_type::kinstr_prfcnt);
    if (!strcmp(str, "kinstr_prfcnt_wa"))
        return std::make_pair(std::error_code{}, backend_type::kinstr_prfcnt_wa);
    if (!strcmp(str, "kinstr_prfcnt_bad"))
        return std::make_pair(std::error_code{}, backend_type::kinstr_prfcnt_bad);

    return std::make_pair(std::make_error_code(std::errc::invalid_argument), backend_type{});
}

backend_types_set backend_type_discover(const kbase_version &version, product_id pid) {
    backend_types_set result{};

    if (version.type() == ioctl_iface_type::jm_pre_r21) {
        result.set(static_cast<size_t>(backend_type::vinstr_pre_r21));
        return result;
    }

    /* We disallow vinstr for gtux and later since vinstr does not
     * support 128 counters per block.
     */
    if (is_vinstr_available(version, pid))
        result.set(static_cast<size_t>(backend_type::vinstr));

    if (is_kinstr_prfcnt_available(version)) {
        result.set(static_cast<size_t>(backend_type::kinstr_prfcnt));
    }

    if (is_kinstr_prfcnt_bad_available(version)) {
        result.set(static_cast<size_t>(backend_type::kinstr_prfcnt_bad));
        result.set(static_cast<size_t>(backend_type::kinstr_prfcnt_wa));
    }

    return result;
}

} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
