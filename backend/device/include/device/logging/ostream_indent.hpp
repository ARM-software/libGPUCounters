/*
 * Copyright (c) 2022-2025 Arm Limited.
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
 * @file ostream_indent.hpp
 *
 * Indent stream manipulators and line indentation.
 *
 * Consider the following example:
 * @code
 * std::ostream &operator<<(std::ostream &os, const my_struct &instance) {
 *     return os << "my_struct {\n"
 *               << indent_level::push // Push indent level. Indent level is now 1.
 *               << indent << "a = " << instance.a << ",\n" // This line will be indented with four spaces.
 *               << indent << "b = " << instance.b << ",\n" // This line will be indented with four spaces.
 *               << indent_level::pop // Pop indent level. Indent level is now 0.
 *               << indent << "}\n"; // This line will be indented with zero spaces.
 * }
 * @endcode
 *
 * When printed, such structure will be indented correctly, even if it is
 * being printed as an inner structure of some other structure.
 */

#pragma once

#include <array>
#include <ostream>

namespace hwcpipe {
namespace debug {

namespace detail {

/** @return indent level index to be used with `std::ostream::iword`. */
inline int get_indent_level_index() {
    const static int index = std::ios_base::xalloc();
    return index;
}

/** @return indent level iword state. */
inline long &get_indent_level(std::ostream &os) { return os.iword(get_indent_level_index()); }
} // namespace detail

/** Indent tag class. */
class indent_type {};

/** I/O stream manipulator to indent a string. */
constexpr static indent_type indent{};

/** Indent level stream manipulation. */
class indent_level {
  public:
    /** Indent level push tag class. */
    class push_type {};
    /** Indent level pop tag class. */
    class pop_type {};

    /** I/O stream manipulator to push event level. */
    constexpr static push_type push{};
    /** I/O stream manipulator to pop event level. */
    constexpr static pop_type pop{};

    /**
     * Indent level guard.
     *
     * Pushes indent level at construction time and pops at destruction.
     */
    class guard {
      public:
        guard(std::ostream &os);
        ~guard();

      private:
        std::ostream &os_;
    };
};

/** Increase indent level by 1. */
inline std::ostream &operator<<(std::ostream &os, indent_level::push_type) {
    detail::get_indent_level(os)++;
    return os;
}

/** Decrease indent level by 1. */
inline std::ostream &operator<<(std::ostream &os, indent_level::pop_type) {
    auto &indent_level = detail::get_indent_level(os);

    if (!indent_level) {
        os.setstate(std::ios::badbit);
        return os;
    }

    detail::get_indent_level(os)--;
    return os;
}

/** Print whitespace for indentation. */
inline std::ostream &operator<<(std::ostream &os, indent_type) {
    auto level = detail::get_indent_level(os);
    for (long i = 0; i < level; ++i) {
        os << "    ";
    }

    return os;
}

inline indent_level::guard::guard(std::ostream &os)
    : os_(os) {
    os_ << indent_level::push;
}

inline indent_level::guard::~guard() { os_ << indent_level::pop; }

} // namespace debug
} // namespace hwcpipe
