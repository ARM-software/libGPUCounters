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

/** @file offset_pointer.hpp */

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <type_traits>

namespace hwcpipe {
namespace device {
namespace ioctl {

/**
 * Offset pointer.
 *
 * Offset pointer is a type-safe object's offset class. It stores an object's offset
 * in some memory region. The pointer value can later be resolved when base address
 * is known.
 *
 * The class is intended for use in ioctl structures to represent objects stored in
 * device memory. Usually, base address is the address returned from `mmap` function
 * (assuming the last argument was set to zero).
 *
 * @par Example:
 * @code
 * struct my_struct {
 *     int filed0;
 *     float field1;
 * };
 *
 * // An instance of `my_struct` is stored in some memory region at offset `0x1000`
 * offset_pointer<my_struct> offset_ptr(0x1000);
 * // Get address of the memory region.
 * void *base_addr = get_addr();
 * // Read the pointer value:
 * my_struct *ptr = offset_ptr.get(base_addr);
 * printf("my_struct { field0 =  %d, field1 = %f}", ptr->field0, ptr->field1);
 * @endcode
 */
template <typename value_t, typename representation_t>
class offset_pointer {
  public:
    /** Value type. */
    using value_type = value_t;
    /** Pointer type. */
    using pointer_type = std::add_pointer_t<value_type>;
    /** Reference type. */
    using reference_type = std::add_lvalue_reference_t<value_type>;
    /** Representation type. */
    using representation_type = representation_t;
    /** Type of the base pointer. */
    using base_pointer_type = std::conditional_t<std::is_const<value_type>::value, const void *, void *>;

    /** Default constructor. */
    offset_pointer() = default;
    /** Default copy constructor. */
    offset_pointer(const offset_pointer &) = default;
    /** Default assignment operator. */
    offset_pointer &operator=(const offset_pointer &) = default;

    /**
     * Construct from offset.
     *
     * @param[in] offset    Offset value.
     */
    offset_pointer(representation_type offset)
        : offset_(offset) {}
    /**
     * Construct from pointer and base address.
     *
     * @param[in] ptr  Pointer value.
     * @param[in] base Base address of the memory region.
     */
    offset_pointer(pointer_type ptr, base_pointer_type base) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        const ptrdiff_t diff = (reinterpret_cast<ptrdiff_t>(ptr) - reinterpret_cast<ptrdiff_t>(base));
        offset_ = static_cast<representation_type>(diff);
    }

    /**
     * Get pointer value.
     *
     * @param[in] base Base address.
     * @return Pointer value.
     */
    pointer_type get(base_pointer_type base) const {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        const auto addr = reinterpret_cast<uintptr_t>(base) + offset_;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<pointer_type>(addr);
    }

    /** @return the offset value. */
    representation_type offset() const { return offset_; }

    /**
     * Reset the offset value.
     *
     * @param[in] offset New offset value.
     */
    void reset(representation_type offset) { *this = offset_pointer(offset); }

    /**
     * Reset the offset value.
     *
     * @param[in] ptr  Pointer value.
     * @param[in] base Base address of the memory region.
     */
    void reset(pointer_type ptr, base_pointer_type base) { *this = offset_pointer(ptr, base); }

  private:
    /** The offset value. */
    representation_type offset_{};

    template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
    friend bool operator==(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                           offset_pointer<rhs_value_t, rhs_representation_t> rhs);

    template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
    friend bool operator!=(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                           offset_pointer<rhs_value_t, rhs_representation_t> rhs);

    template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
    friend bool operator<(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                          offset_pointer<rhs_value_t, rhs_representation_t> rhs);

    template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
    friend bool operator>(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                          offset_pointer<rhs_value_t, rhs_representation_t> rhs);

    template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
    friend bool operator<=(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                           offset_pointer<rhs_value_t, rhs_representation_t> rhs);

    template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
    friend bool operator>=(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                           offset_pointer<rhs_value_t, rhs_representation_t> rhs);

    template <typename rhs_value_t, typename rhs_representation_t>
    friend std::ostream &operator<<(std::ostream &os, offset_pointer<rhs_value_t, rhs_representation_t> rhs);
};

template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
inline bool operator==(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                       offset_pointer<rhs_value_t, rhs_representation_t> rhs) {
    return lhs.offset_ == rhs.offset_;
}

template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
inline bool operator!=(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                       offset_pointer<rhs_value_t, rhs_representation_t> rhs) {
    return lhs.offset_ != rhs.offset_;
}

template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
inline bool operator<(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                      offset_pointer<rhs_value_t, rhs_representation_t> rhs) {
    return lhs.offset_ < rhs.offset_;
}

template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
inline bool operator>(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                      offset_pointer<rhs_value_t, rhs_representation_t> rhs) {
    return lhs.offset_ > rhs.offset_;
}

template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
inline bool operator<=(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                       offset_pointer<rhs_value_t, rhs_representation_t> rhs) {
    return lhs.offset_ <= rhs.offset_;
}

template <typename lhs_value_t, typename lhs_representation_t, typename rhs_value_t, typename rhs_representation_t>
inline bool operator>=(offset_pointer<lhs_value_t, lhs_representation_t> lhs,
                       offset_pointer<rhs_value_t, rhs_representation_t> rhs) {
    return lhs.offset_ >= rhs.offset_;
}

template <typename rhs_value_t, typename rhs_representation_t>
inline std::ostream &operator<<(std::ostream &os, offset_pointer<rhs_value_t, rhs_representation_t> rhs) {
    return os << rhs.offset_;
}

static_assert(sizeof(offset_pointer<uint64_t, uint32_t>) == sizeof(uint32_t),
              "offset_pointer size is not the same as its representation type.");
static_assert(alignof(offset_pointer<uint64_t, uint32_t>) == alignof(uint32_t),
              "offset_pointer alignment is not the same as its representation type.");
static_assert(std::is_trivially_copyable<offset_pointer<uint64_t, uint32_t>>::value,
              "offset_pointer is not trivially copyable.");

} // namespace ioctl
} // namespace device
} // namespace hwcpipe
