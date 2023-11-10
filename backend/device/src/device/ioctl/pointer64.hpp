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

/** @file pointer64.hpp */

#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>
#include <type_traits>

namespace hwcpipe {
namespace device {
namespace ioctl {

/**
 * 64 bit pointer.
 *
 * Usually, for 32/64 bit compatibility, we use `uint64_t` type instead of
 * pointer types in our ioctl command argument structures. The issue with this
 * approach is that `uint64_t` is type-less, i.e. we do not know the type it
 * points to. This class is a type safe wrapper around `uint64_t` with
 * a pointer-like interface.
 *
 * @par Example
 * @code
 * struct my_ioctl_struct {
 *     pointer64<int> integers; // always 64 bit, even on 32 bit architectures.
 *     uint64_t       num_integers;
 * };
 * @endcode
 */
template <typename value_t>
class pointer64 {
  public:
    /** Value type. */
    using value_type = value_t;
    /** Pointer type. */
    using pointer_type = std::add_pointer_t<value_type>;
    /** Reference type. */
    using reference_type = std::add_lvalue_reference_t<value_type>;

    /** Default constructor. */
    pointer64() = default;
    /** Default copy constructor. */
    pointer64(const pointer64 &ptr) = default;

    /** Null pointer constructor. */
    pointer64(std::nullptr_t) {}
    /** Construct from pointer value. */
    pointer64(pointer_type ptr)
        : value_(ptr_to_uint64(ptr)) {}
    /** Construct from uint64_t. */
    explicit pointer64(uint64_t value)
        : value_(value) {}

    /** Copy constructor from other pointer64. */
    template <typename other_value_t>
    pointer64(const pointer64<other_value_t> &other_ptr)
        : pointer64(other_ptr.get()) {}

    /** Default assignment operator. */
    pointer64 &operator=(const pointer64 &) = default;

    /** @return Pointer to the object. */
    pointer_type get() const {
        assert(value_ % alignof(value_type) == 0);

        return uint64_to_ptr(value_);
    }

    /** uint64 value of the pointer. */
    uint64_t as_uint64() const { return value_; }

    /**
     * Set pointer value.
     *
     * @param[in] ptr New pointer value.
     */
    void reset(pointer_type ptr = {}) { value_ = ptr_to_uint64(ptr); }

    /**
     * Set pointer value from uint64_t.
     *
     * @param[in] ptr New pointer value.
     */
    void reset(uint64_t ptr) { value_ = ptr; }

    /** @return true if it is a non null pointer, false otherwise. */
    operator bool() const { return value_ != 0; }
    /** @return reference of the object stored. */
    reference_type operator*() const { return *get(); }
    /** @return pointer to the object stored. */
    pointer_type operator->() const { return get(); }

  private:
    /** @return Pointer to the object. Alignment is not checked. */
    pointer_type get_unsafe() const { return uint64_to_ptr(value_); }

    /** Convert uint64_t to pointer. */
    static pointer_type uint64_to_ptr(uint64_t value) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<pointer_type>(static_cast<uintptr_t>(value));
    }
    /** Convert pointer to uint64_t. */
    static uint64_t ptr_to_uint64(pointer_type ptr) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ptr));
    }

    /** Pointer value as uint64_t. */
    uint64_t value_ = 0;

    template <typename lhs_t, typename rhs_t>
    friend inline bool operator==(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs);

    template <typename other_value_t>
    friend inline bool operator==(pointer64<other_value_t> ptr, std::nullptr_t);

    template <typename other_value_t>
    friend inline bool operator==(std::nullptr_t, pointer64<other_value_t> ptr);

    template <typename lhs_t, typename rhs_t>
    friend inline bool operator!=(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs);

    template <typename other_value_t>
    friend inline bool operator!=(pointer64<other_value_t> ptr, std::nullptr_t);

    template <typename other_value_t>
    friend inline bool operator!=(std::nullptr_t, pointer64<other_value_t> ptr);

    template <typename lhs_t, typename rhs_t>
    friend inline bool operator<(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs);

    template <typename lhs_t, typename rhs_t>
    friend inline bool operator>(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs);

    template <typename lhs_t, typename rhs_t>
    friend inline bool operator<=(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs);

    template <typename lhs_t, typename rhs_t>
    friend inline bool operator>=(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs);

    template <typename rhs_t>
    friend inline std::ostream &operator<<(std::ostream &os, pointer64<rhs_t> rhs);
};

template <typename lhs_t, typename rhs_t>
inline bool operator==(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs) {
    return lhs.get_unsafe() == rhs.get_unsafe();
}

template <typename value_t>
inline bool operator==(pointer64<value_t> ptr, std::nullptr_t) {
    return ptr.get_unsafe() == nullptr;
}

template <typename value_t>
inline bool operator==(std::nullptr_t, pointer64<value_t> ptr) {
    return ptr.get_unsafe() == nullptr;
}

template <typename lhs_t, typename rhs_t>
inline bool operator!=(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs) {
    return lhs.get_unsafe() != rhs.get_unsafe();
}

template <typename value_t>
inline bool operator!=(pointer64<value_t> ptr, std::nullptr_t) {
    return ptr.get_unsafe() != nullptr;
}

template <typename value_t>
inline bool operator!=(std::nullptr_t, pointer64<value_t> ptr) {
    return ptr.get_unsafe() != nullptr;
}

template <typename lhs_t, typename rhs_t>
inline bool operator<(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs) {
    return lhs.get_unsafe() < rhs.get_unsafe();
}

template <typename lhs_t, typename rhs_t>
inline bool operator>(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs) {
    return lhs.get_unsafe() > rhs.get_unsafe();
}

template <typename lhs_t, typename rhs_t>
inline bool operator<=(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs) {
    return lhs.get_unsafe() <= rhs.get_unsafe();
}

template <typename lhs_t, typename rhs_t>
inline bool operator>=(pointer64<lhs_t> lhs, pointer64<rhs_t> rhs) {
    return lhs.get_unsafe() >= rhs.get_unsafe();
}

template <typename rhs_t>
inline std::ostream &operator<<(std::ostream &os, pointer64<rhs_t> rhs) {
    return os << rhs.get_unsafe();
}

static_assert(sizeof(pointer64<int>) == sizeof(uint64_t), "pointer64 size is not the same as uint64_t.");
static_assert(alignof(pointer64<int>) == alignof(uint64_t), "pointer64 alignment is not the same as uint64_t.");
static_assert(std::is_trivially_copyable<pointer64<int>>::value, "pointer64 is not trivially copyable.");

} // namespace ioctl
} // namespace device
} // namespace hwcpipe
