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

/* Note, this file is generated, do NOT edit! */

#pragma once

// clang-format off

#include "commands.hpp"

#include <debug/ostream_indent.hpp>
#include <debug/print_array.hpp>

#include <cassert>
#include <ostream>

namespace hwcpipe {
namespace device {
namespace ioctl {
namespace vinstr {

inline std::ostream &operator<<(std::ostream &os, reader_event value);
inline std::ostream &operator<<(std::ostream &os, reader_features value);
inline std::ostream &operator<<(std::ostream &os, const reader_metadata_cycles &value);
inline std::ostream &operator<<(std::ostream &os, const reader_metadata &value);
inline std::ostream &operator<<(std::ostream &os, const reader_metadata_with_cycles &value);
inline std::ostream &operator<<(std::ostream &os, const reader_api_version &value);

inline std::ostream &operator<<(std::ostream &os, reader_event value) {
    switch (value) {
    case reader_event::manual:
        return os << "manual";
    case reader_event::periodic:
        return os << "periodic";
    case reader_event::prejob:
        return os << "prejob";
    case reader_event::postjob:
        return os << "postjob";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, reader_features value) {
    if (!value)
        return os << 0;

    if (!!(value & reader_features::cycles_top)) {
        value ^= reader_features::cycles_top;
        os << "cycles_top";
    }

    if (!!value)
        os << " | ";

    if (!!(value & reader_features::cycles_shader_core)) {
        value ^= reader_features::cycles_shader_core;
        os << "cycles_shader_core";
    }

    if (!!value)
        os << " | ";

    if (!!value) {
        assert(!&"Unknown bits");
        os << std::hex << static_cast<uint32_t>(value);
    }

    return os;
}

inline std::ostream &operator<<(std::ostream &os, const reader_metadata_cycles &value) {
    return os << "reader_metadata_cycles {\n" //
              << debug::indent_level::push //
              << debug::indent << ".top = " << value.top << ",\n" //
              << debug::indent << ".shader_cores = " << value.shader_cores << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const reader_metadata &value) {
    return os << "reader_metadata {\n" //
              << debug::indent_level::push //
              << debug::indent << ".timestamp = " << value.timestamp << ",\n" //
              << debug::indent << ".event_id = " << value.event_id << ",\n" //
              << debug::indent << ".buffer_idx = " << value.buffer_idx << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const reader_metadata_with_cycles &value) {
    return os << "reader_metadata_with_cycles {\n" //
              << debug::indent_level::push //
              << debug::indent << ".metadata = " << value.metadata << ",\n" //
              << debug::indent << ".cycles = " << value.cycles << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const reader_api_version &value) {
    return os << "reader_api_version {\n" //
              << debug::indent_level::push //
              << debug::indent << ".version = " << value.version << ",\n" //
              << debug::indent << ".features = " << value.features << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, command::command_type cmd) {
    switch (cmd) {
    case command::get_hwver:
        return os << "vinstr::command::get_hwver";
    case command::get_buffer_size:
        return os << "vinstr::command::get_buffer_size";
    case command::dump:
        return os << "vinstr::command::dump";
    case command::clear:
        return os << "vinstr::command::clear";
    case command::get_buffer:
        return os << "vinstr::command::get_buffer";
    case command::get_buffer_with_cycles:
        return os << "vinstr::command::get_buffer_with_cycles";
    case command::put_buffer:
        return os << "vinstr::command::put_buffer";
    case command::put_buffer_with_cycles:
        return os << "vinstr::command::put_buffer_with_cycles";
    case command::set_interval:
        return os << "vinstr::command::set_interval";
    case command::enable_event:
        return os << "vinstr::command::enable_event";
    case command::disable_event:
        return os << "vinstr::command::disable_event";
    case command::get_api_version:
        return os << "vinstr::command::get_api_version";
    case command::get_api_version_with_features:
        return os << "vinstr::command::get_api_version_with_features";
    }

    assert(!&"Unknown command");
    return os << "::command::<unknown>";
}

} // namespace vinstr
} // namespace ioctl
} // namespace device
} // namespace hwcpipe

#include "print_manual.hpp"

// clang-format on
