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

/** @file kbase_version.hpp Kbase version class. */

#pragma once

#include <cassert>
#include <cstdint>
#include <utility>

/* Some old versions of <sys/types.h> header define major and minor macros.
 * Here we conveniently un-define them to avoid compilation errors.
 */

#if defined(major)
#undef major
#endif

#if defined(minor)
#undef minor
#endif

namespace hwcpipe {
namespace device {

/** Kbase ioctl interface type. */
enum class ioctl_iface_type {
    /** Pre R21 release Job manager kernel. */
    jm_pre_r21,
    /** Post R21 release Job manager kernel. */
    jm_post_r21,
    /** CSF kernel. */
    csf
};

/** Check version compatibility between kernel and userspace. */
class kbase_version {
  public:
    /**
     * Constructor
     *
     * @param[in] major Major kbase interface version.
     * @param[in] minor Minor kbase interface version.
     * @param[in] type  Kbase interface type.
     */
    constexpr kbase_version(uint16_t major, uint16_t minor, ioctl_iface_type type)
        : major_(major)
        , minor_(minor)
        , type_(type) {}

    kbase_version() = default;
    kbase_version(const kbase_version &) = default;
    kbase_version &operator=(const kbase_version &) = default;

    /** @return major version. */
    uint16_t major() const { return major_; }
    /** @return minor version. */
    uint16_t minor() const { return minor_; }
    /** @return ioctl interface type. */
    ioctl_iface_type type() const { return type_; }

  private:
    /** Major version number. */
    uint16_t major_{0};

    /** Minor version number. */
    uint16_t minor_{0};

    /** ioctl interface type info. */
    ioctl_iface_type type_{ioctl_iface_type::csf};
};

inline bool operator==(const kbase_version &lhs, const kbase_version &rhs) {
    return (lhs.major() == rhs.major())    //
           && (lhs.minor() == rhs.minor()) //
           && (lhs.type() == rhs.type());
}

inline bool operator!=(const kbase_version &lhs, const kbase_version &rhs) { return !(lhs == rhs); }

inline bool operator<(const kbase_version &lhs, const kbase_version &rhs) {
    assert(lhs.type() == rhs.type());
    return std::make_pair(lhs.major(), lhs.minor()) < std::make_pair(rhs.major(), rhs.minor());
}

inline bool operator<=(const kbase_version &lhs, const kbase_version &rhs) {
    assert(lhs.type() == rhs.type());
    return std::make_pair(lhs.major(), lhs.minor()) <= std::make_pair(rhs.major(), rhs.minor());
}

inline bool operator>(const kbase_version &lhs, const kbase_version &rhs) {
    assert(lhs.type() == rhs.type());
    return std::make_pair(lhs.major(), lhs.minor()) > std::make_pair(rhs.major(), rhs.minor());
}

inline bool operator>=(const kbase_version &lhs, const kbase_version &rhs) {
    assert(lhs.type() == rhs.type());
    return std::make_pair(lhs.major(), lhs.minor()) >= std::make_pair(rhs.major(), rhs.minor());
}

} // namespace device
} // namespace hwcpipe
