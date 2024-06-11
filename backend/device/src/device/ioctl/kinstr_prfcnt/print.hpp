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
namespace kinstr_prfcnt {

inline std::ostream &operator<<(std::ostream &os, block_type value);
inline std::ostream &operator<<(std::ostream &os, prfcnt_set value);
inline std::ostream &operator<<(std::ostream &os, enum_item::item_type value);
inline std::ostream &operator<<(std::ostream &os, const enum_item::header &value);
inline std::ostream &operator<<(std::ostream &os, const enum_item::enum_block_counter &value);
inline std::ostream &operator<<(std::ostream &os, enum_item::enum_request::request_type value);
inline std::ostream &operator<<(std::ostream &os, const enum_item::enum_request &value);
inline std::ostream &operator<<(std::ostream &os, const enum_item::enum_sample_info &value);
// no `operator<<` for union enum_item::enum_union
inline std::ostream &operator<<(std::ostream &os, const enum_item &value);
inline std::ostream &operator<<(std::ostream &os, metadata_item::item_type value);
inline std::ostream &operator<<(std::ostream &os, const metadata_item::header &value);
inline std::ostream &operator<<(std::ostream &os, metadata_item::block_metadata::block_state_type value);
inline std::ostream &operator<<(std::ostream &os, const metadata_item::block_metadata &value);
inline std::ostream &operator<<(std::ostream &os, const metadata_item::clock_metadata &value);
inline std::ostream &operator<<(std::ostream &os, metadata_item::sample_metadata::sample_flag value);
inline std::ostream &operator<<(std::ostream &os, const metadata_item::sample_metadata &value);
// no `operator<<` for union metadata_item::metadata_union
inline std::ostream &operator<<(std::ostream &os, const metadata_item &value);
inline std::ostream &operator<<(std::ostream &os, control_cmd::control_cmd_code value);
inline std::ostream &operator<<(std::ostream &os, const control_cmd &value);
inline std::ostream &operator<<(std::ostream &os, request_item::item_type value);
inline std::ostream &operator<<(std::ostream &os, const request_item::header &value);
inline std::ostream &operator<<(std::ostream &os, request_item::request_mode::sampling_mode value);
inline std::ostream &operator<<(std::ostream &os, const request_item::request_mode::periodic_type &value);
// no `operator<<` for union request_item::request_mode::mode_config_union
inline std::ostream &operator<<(std::ostream &os, const request_item::request_mode &value);
inline std::ostream &operator<<(std::ostream &os, const request_item::request_enable &value);
inline std::ostream &operator<<(std::ostream &os, request_item::request_scope::counters_scope value);
inline std::ostream &operator<<(std::ostream &os, const request_item::request_scope &value);
// no `operator<<` for union request_item::request_union
inline std::ostream &operator<<(std::ostream &os, const request_item &value);
inline std::ostream &operator<<(std::ostream &os, const sample_access &value);

inline std::ostream &operator<<(std::ostream &os, block_type value) {
    switch (value) {
    case block_type::fe:
        return os << "fe";
    case block_type::tiler:
        return os << "tiler";
    case block_type::memory:
        return os << "memory";
    case block_type::shader_core:
        return os << "shader_core";
    case block_type::firmware:
        return os << "firmware";
    case block_type::csg:
        return os << "csg";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, prfcnt_set value) {
    switch (value) {
    case prfcnt_set::primary:
        return os << "primary";
    case prfcnt_set::secondary:
        return os << "secondary";
    case prfcnt_set::tertiary:
        return os << "tertiary";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, enum_item::item_type value) {
    switch (value) {
    case enum_item::item_type::block:
        return os << "block";
    case enum_item::item_type::request:
        return os << "request";
    case enum_item::item_type::sample_info:
        return os << "sample_info";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint16_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, const enum_item::header &value) {
    return os << "header {\n" //
              << debug::indent_level::push //
              << debug::indent << ".type = " << value.type << ",\n" //
              << debug::indent << ".item_version = " << value.item_version << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const enum_item::enum_block_counter &value) {
    return os << "enum_block_counter {\n" //
              << debug::indent_level::push //
              << debug::indent << ".type = " << value.type << ",\n" //
              << debug::indent << ".set = " << value.set << ",\n" //
              << debug::indent << ".num_instances = " << value.num_instances << ",\n" //
              << debug::indent << ".num_values = " << value.num_values << ",\n" //
              << debug::indent << ".counter_mask = " << debug::print_array(value.counter_mask) << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, enum_item::enum_request::request_type value) {
    switch (value) {
    case enum_item::enum_request::request_type::mode:
        return os << "mode";
    case enum_item::enum_request::request_type::enable:
        return os << "enable";
    case enum_item::enum_request::request_type::scope:
        return os << "scope";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint16_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, const enum_item::enum_request &value) {
    return os << "enum_request {\n" //
              << debug::indent_level::push //
              << debug::indent << ".request_item_type = " << value.request_item_type << ",\n" //
              << debug::indent << ".versions_mask = " << value.versions_mask << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const enum_item::enum_sample_info &value) {
    return os << "enum_sample_info {\n" //
              << debug::indent_level::push //
              << debug::indent << ".num_clock_domains = " << value.num_clock_domains << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

// no `operator<<` for union enum_item::enum_union

inline std::ostream &operator<<(std::ostream &os, metadata_item::item_type value) {
    switch (value) {
    case metadata_item::item_type::none:
        return os << "none";
    case metadata_item::item_type::sample:
        return os << "sample";
    case metadata_item::item_type::clock:
        return os << "clock";
    case metadata_item::item_type::block:
        return os << "block";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint16_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, const metadata_item::header &value) {
    return os << "header {\n" //
              << debug::indent_level::push //
              << debug::indent << ".type = " << value.type << ",\n" //
              << debug::indent << ".item_version = " << value.item_version << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, metadata_item::block_metadata::block_state_type value) {
    if (!value)
        return os << 0;

    if (!!(value & metadata_item::block_metadata::block_state_type::on)) {
        value ^= metadata_item::block_metadata::block_state_type::on;
        os << "on";
    }

    if (!!value)
        os << " | ";

    if (!!(value & metadata_item::block_metadata::block_state_type::off)) {
        value ^= metadata_item::block_metadata::block_state_type::off;
        os << "off";
    }

    if (!!value)
        os << " | ";

    if (!!(value & metadata_item::block_metadata::block_state_type::available)) {
        value ^= metadata_item::block_metadata::block_state_type::available;
        os << "available";
    }

    if (!!value)
        os << " | ";

    if (!!(value & metadata_item::block_metadata::block_state_type::unavailable)) {
        value ^= metadata_item::block_metadata::block_state_type::unavailable;
        os << "unavailable";
    }

    if (!!value)
        os << " | ";

    if (!!(value & metadata_item::block_metadata::block_state_type::normal_mode)) {
        value ^= metadata_item::block_metadata::block_state_type::normal_mode;
        os << "normal_mode";
    }

    if (!!value)
        os << " | ";

    if (!!(value & metadata_item::block_metadata::block_state_type::protected_mode)) {
        value ^= metadata_item::block_metadata::block_state_type::protected_mode;
        os << "protected_mode";
    }

    if (!!value)
        os << " | ";

    if (!!value) {
        assert(!&"Unknown bits");
        os << std::hex << static_cast<uint32_t>(value);
    }

    return os;
}

inline std::ostream &operator<<(std::ostream &os, const metadata_item::block_metadata &value) {
    return os << "block_metadata {\n" //
              << debug::indent_level::push //
              << debug::indent << ".type = " << value.type << ",\n" //
              << debug::indent << ".block_idx = " << static_cast<uint32_t>(value.block_idx) << ",\n" //
              << debug::indent << ".set = " << value.set << ",\n" //
              << debug::indent << ".block_state = " << value.block_state << ",\n" //
              << debug::indent << ".values_offset = " << value.values_offset << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, const metadata_item::clock_metadata &value) {
    return os << "clock_metadata {\n" //
              << debug::indent_level::push //
              << debug::indent << ".num_domains = " << value.num_domains << ",\n" //
              << debug::indent << ".cycles = " << debug::print_array(value.cycles) << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, metadata_item::sample_metadata::sample_flag value) {
    if (!value)
        return os << 0;

    if (!!(value & metadata_item::sample_metadata::sample_flag::overflow)) {
        value ^= metadata_item::sample_metadata::sample_flag::overflow;
        os << "overflow";
    }

    if (!!value)
        os << " | ";

    if (!!(value & metadata_item::sample_metadata::sample_flag::error)) {
        value ^= metadata_item::sample_metadata::sample_flag::error;
        os << "error";
    }

    if (!!value)
        os << " | ";

    if (!!value) {
        assert(!&"Unknown bits");
        os << std::hex << static_cast<uint32_t>(value);
    }

    return os;
}

inline std::ostream &operator<<(std::ostream &os, const metadata_item::sample_metadata &value) {
    return os << "sample_metadata {\n" //
              << debug::indent_level::push //
              << debug::indent << ".timestamp_start = " << value.timestamp_start << ",\n" //
              << debug::indent << ".timestamp_stop = " << value.timestamp_stop << ",\n" //
              << debug::indent << ".seq = " << value.seq << ",\n" //
              << debug::indent << ".user_data = " << value.user_data << ",\n" //
              << debug::indent << ".flags = " << value.flags << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

// no `operator<<` for union metadata_item::metadata_union

inline std::ostream &operator<<(std::ostream &os, control_cmd::control_cmd_code value) {
    switch (value) {
    case control_cmd::control_cmd_code::start:
        return os << "start";
    case control_cmd::control_cmd_code::stop:
        return os << "stop";
    case control_cmd::control_cmd_code::sample_sync:
        return os << "sample_sync";
    case control_cmd::control_cmd_code::sample_async:
        return os << "sample_async";
    case control_cmd::control_cmd_code::discard:
        return os << "discard";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint16_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, const control_cmd &value) {
    return os << "control_cmd {\n" //
              << debug::indent_level::push //
              << debug::indent << ".cmd = " << value.cmd << ",\n" //
              << debug::indent << ".user_data = " << value.user_data << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, request_item::item_type value) {
    switch (value) {
    case request_item::item_type::none:
        return os << "none";
    case request_item::item_type::mode:
        return os << "mode";
    case request_item::item_type::enable:
        return os << "enable";
    case request_item::item_type::scope:
        return os << "scope";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint16_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, const request_item::header &value) {
    return os << "header {\n" //
              << debug::indent_level::push //
              << debug::indent << ".type = " << value.type << ",\n" //
              << debug::indent << ".item_version = " << value.item_version << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, request_item::request_mode::sampling_mode value) {
    switch (value) {
    case request_item::request_mode::sampling_mode::manual:
        return os << "manual";
    case request_item::request_mode::sampling_mode::periodic:
        return os << "periodic";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, const request_item::request_mode::periodic_type &value) {
    return os << "periodic_type {\n" //
              << debug::indent_level::push //
              << debug::indent << ".period_ns = " << value.period_ns << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

// no `operator<<` for union request_item::request_mode::mode_config_union

inline std::ostream &operator<<(std::ostream &os, const request_item::request_enable &value) {
    return os << "request_enable {\n" //
              << debug::indent_level::push //
              << debug::indent << ".type = " << value.type << ",\n" //
              << debug::indent << ".set = " << value.set << ",\n" //
              << debug::indent << ".enable_mask = " << debug::print_array(value.enable_mask) << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, request_item::request_scope::counters_scope value) {
    switch (value) {
    case request_item::request_scope::counters_scope::global:
        return os << "global";
    }

    assert(!&"Unknown enum value");
    return os << "<unknown = " << static_cast<uint32_t>(value) << ">";
}

inline std::ostream &operator<<(std::ostream &os, const request_item::request_scope &value) {
    return os << "request_scope {\n" //
              << debug::indent_level::push //
              << debug::indent << ".scope = " << value.scope << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

// no `operator<<` for union request_item::request_union

inline std::ostream &operator<<(std::ostream &os, const sample_access &value) {
    return os << "sample_access {\n" //
              << debug::indent_level::push //
              << debug::indent << ".sequence = " << value.sequence << ",\n" //
              << debug::indent << ".sample_metadata_ptr = " << value.sample_metadata_ptr << ",\n" //
              << debug::indent_level::pop //
              << debug::indent << "}"; //
}

inline std::ostream &operator<<(std::ostream &os, command::command_type cmd) {
    switch (cmd) {
    case command::issue_command:
        return os << "kinstr_prfcnt::command::issue_command";
    case command::get_sample:
        return os << "kinstr_prfcnt::command::get_sample";
    case command::put_sample:
        return os << "kinstr_prfcnt::command::put_sample";
    }

    assert(!&"Unknown command");
    return os << "::command::<unknown>";
}

} // namespace kinstr_prfcnt
} // namespace ioctl
} // namespace device
} // namespace hwcpipe

#include "print_manual.hpp"

// clang-format on
