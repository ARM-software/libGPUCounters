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

#include <device/ioctl/offset_pointer.hpp>
#include <device/ioctl/pointer64.hpp>

#include <cstddef>
#include <cstdint>

namespace hwcpipe {
namespace device {
namespace ioctl {

/** Kinstr performance counters ioctl interface. */
namespace kinstr_prfcnt {
namespace detail {

/** Flex list type. */
enum class flex_list_type : uint8_t {
    /** Enumeration type. */
    enumeration,
    /** Request type. */
    request,
    /** Metadata type. */
    metadata,
};

/** @return Flex list item type. */
inline constexpr uint16_t flex_list_item_type(flex_list_type type, uint16_t subtype) {

    constexpr uint16_t type_shift = 12;
    constexpr uint16_t subtype_mask = 0xfff;

    return static_cast<uint16_t>((static_cast<uint8_t>(type) << type_shift)) | (subtype_mask & subtype);
}
} // namespace detail

/** Kinstr performance counters ioctl interface version. */
static constexpr const uint16_t api_version = 0;

/** Type of performance counter block. */
enum class block_type : uint8_t {
    /** Front end. */
    fe,
    /** Tiler. */
    tiler,
    /** Memory system. */
    memory,
    /** Shader core. */
    shader_core,
    /** CSF Firmware. */
    firmware,
    /** Firmware command stream group. */
    csg,
};

/** Type of performance counter block set. */
enum class prfcnt_set : uint8_t {
    /** Primary. */
    primary,
    /** Secondary. */
    secondary,
    /** Tertiary. */
    tertiary,
};

/** Performance counter enumeration item. */
struct enum_item {
    /** Enumeration item type. */
    enum class item_type : uint16_t {
        /** Sentinel value. */
        none = 0,
        /** Enumeration item type block. */
        block = detail::flex_list_item_type(detail::flex_list_type::enumeration, 0),
        /** Enumeration item type request. */
        request = detail::flex_list_item_type(detail::flex_list_type::enumeration, 1),
        /** Enumeration item type sample info. */
        sample_info = detail::flex_list_item_type(detail::flex_list_type::enumeration, 2),
    };

    /** Enum header structure. */
    struct header {
        /** Type of the item. */
        item_type type;
        /** Protocol version. */
        uint16_t item_version;
    };

    /** Performance counter block descriptor. */
    struct enum_block_counter {
        /** Type of performance counter block. */
        block_type type;
        /** Which SET this represents: primary, secondary or tertiary. */
        prfcnt_set set;
        /** Padding bytes. */
        uint8_t pad[2];
        /** How many instances of this block type exist in the hardware. */
        uint16_t num_instances;
        /** How many entries in the values array there are for samples from this block. */
        uint16_t num_values;
        /**
         * Bitmask that indicates counter availability in this block. A '0' indicates that
         * a counter is not available at that index and will always return zeroes if
         * requested.
         */
        uint64_t counter_mask[2];
    };

    /** Request descriptor. */
    struct enum_request {
        /** Request type. */
        enum class request_type : uint16_t {
            /** Request mode. */
            mode,
            /** Request enable. */
            enable,
            /** Request scope. */
            scope,
        };

        /** Type of request. */
        request_type request_item_type;
        /** Padding bytes. */
        uint16_t pad;
        /** Bitmask of versions that support this request. */
        uint32_t versions_mask;
    };

    /** Sample information descriptor. */
    struct enum_sample_info {
        /** Number of clock domains of the GPU. */
        uint32_t num_clock_domains;
        /** Padding bytes. */
        uint32_t pad;
    };

    /** Enum union. */
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    union enum_union {
        /** Performance counter block descriptor. */
        enum_block_counter block_counter;
        /** Request descriptor. */
        enum_request request;
        /** Sample info descriptor. */
        enum_sample_info sample_info;
    };

    /** Header describing the type of item in the list. */
    header hdr;
    /** Padding bytes. */
    uint8_t padding[4];
    /** Structure containing descriptor for enumeration item type. */
    enum_union u;
};

/** Performance counters metadata item. */
struct metadata_item {
    /** Metadata item type. */
    enum class item_type : uint16_t {
        /** Sentinel value. */
        none = 0,
        /** Metadata item type sample. */
        sample = detail::flex_list_item_type(detail::flex_list_type::metadata, 0),
        /** Metadata item type clock. */
        clock = detail::flex_list_item_type(detail::flex_list_type::metadata, 1),
        /** Metadata item type block. */
        block = detail::flex_list_item_type(detail::flex_list_type::metadata, 2),
    };

    /** Metadata header structure. */
    struct header {
        /** Type of the item. */
        item_type type;
        /** Protocol version. */
        uint16_t item_version;
    };

    /** Metadata for counter block. */
    struct block_metadata {
        /** Block state bits. */
        enum class block_state_type : uint32_t {
            /** This block was powered on for at least some portion of the sample. */
            on = (uint32_t{1} << 0),
            /** This block was powered off for at least some portion of the sample. */
            off = (uint32_t{1} << 1),
            /** This block was available to this VM for at least some portion of the sample. */
            available = (uint32_t{1} << 2),
            /**
             * This block was not available to this VM for at least some portion of the sample
             * Note that no data is collected when the block is not available to the VM.
             */
            unavailable = (uint32_t{1} << 3),
            /**
             * This block was operating in "normal" (non-protected) mode for at least some
             * portion of the sample.
             */
            normal_mode = (uint32_t{1} << 4),
            /**
             * This block was operating in "protected" mode for at least some portion of the
             * sample. Note that no data is collected when the block is in protected mode.
             */
            protected_mode = (uint32_t{1} << 5),
        };

        /** Type of performance counter block. */
        block_type type;
        /** Index of performance counter block. */
        uint8_t block_idx;
        /** Set of performance counter block. */
        prfcnt_set set;
        /** Padding bytes. */
        uint8_t pad_u8;
        /**
         * Bits set indicate the states which the block is known to have operated in during
         * this sample.
         */
        block_state_type block_state;
        /**
         * Offset from the start of the mmapped region, to the values for this block. The
         * values themselves are an array of uint64_t.
         */
        uint32_t values_offset;
        /** Padding bytes. */
        uint32_t pad_u32;
    };

    /** Metadata for clock cycles. */
    struct clock_metadata {
        /** Maximum number of clock domains. */
        static constexpr const size_t max_reported_domains = 4;

        /** Number of domains this metadata refers to. */
        uint32_t num_domains;
        /** Padding bytes. */
        uint32_t pad;
        /**
         * Number of cycles elapsed in each counter domain between timestamp_start and
         * timestamp_end. Valid only for the first num_domains.
         */
        uint64_t cycles[max_reported_domains];
    };

    /** Metadata for counter sample data. */
    struct sample_metadata {
        /** Sample flags. */
        enum class sample_flag : uint32_t {
            /**
             * This sample contains overflows from dump duration stretch because the sample
             * buffer was full.
             */
            overflow = (uint32_t{1} << 0),
            /** This sample has had an error condition for sample duration. */
            error = (uint32_t{1} << 30),
        };

        /** Earliest timestamp that values in this sample represent. */
        uint64_t timestamp_start;
        /** Latest timestamp that values in this sample represent. */
        uint64_t timestamp_stop;
        /** Sequence number of this sample. Must match the value from GET_SAMPLE. */
        uint64_t seq;
        /** User data provided to HWC_CMD_START or HWC_CMD_SAMPLE_* */
        uint64_t user_data;
        /** Property flags. */
        sample_flag flags;
        /** Padding bytes. */
        uint32_t pad;
    };

    /** Metadata union. */
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    union metadata_union {
        /** Counter sample data metadata descriptor. */
        sample_metadata sample_md;
        /** Clock cycles metadata descriptor. */
        clock_metadata clock_md;
        /** Counter block metadata descriptor. */
        block_metadata block_md;
    };

    /** Header describing the type of item in the list. */
    header hdr;
    /** Padding bytes. */
    uint8_t padding[4];
    /** Structure containing descriptor for metadata type. */
    metadata_union u;
};

/** Control command. */
struct control_cmd {
    /** Control command code for client session. */
    enum class control_cmd_code : uint16_t {
        /** Start the counter data dump run for the calling client session. */
        start = 1,
        /** Stop the counter data dump run for the calling client session. */
        stop,
        /** Trigger a synchronous manual sample. */
        sample_sync,
        /** Trigger an asynchronous manual sample. */
        sample_async,
        /**
         * Discard all samples which have not yet been consumed by userspace. Note that
         * this can race with new samples if HWC_CMD_STOP is not called first.
         */
        discard,
    };

    /** Control command for the session. */
    control_cmd_code cmd;
    /** Padding bytes. */
    uint16_t pad[3];
    /**
     * Offset from the start of the mapped area to the first entry in the metadata list
     * (sample_metadata) for this sample.
     */
    uint64_t user_data;
};

/** Performance counter request item. */
struct request_item {
    /** Type of request descriptor. */
    enum class item_type : uint16_t {
        /** Sentinel value. */
        none = 0,
        /** Specify the capture mode to be used for the session. */
        mode = detail::flex_list_item_type(detail::flex_list_type::request, 0),
        /** Specify which performance counters to capture. */
        enable = detail::flex_list_item_type(detail::flex_list_type::request, 1),
        /** Specify the scope of the performance counters. */
        scope = detail::flex_list_item_type(detail::flex_list_type::request, 2),
    };

    /** Request header structure. */
    struct header {
        /** Type of the item. */
        item_type type;
        /** Protocol version. */
        uint16_t item_version;
    };

    /** Mode request descriptor. */
    struct request_mode {
        /** Capture mode for counter sampling. */
        enum class sampling_mode : uint8_t {
            /** Manual sampling mode. */
            manual,
            /** Periodic sampling mode. */
            periodic,
        };

        /** Periodic mode configuration struct. */
        struct periodic_type {
            /** Sample period (nanoseconds). */
            uint64_t period_ns;
        };

        /** Mode configuration union. */
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
        union mode_config_union {
            /** Periodic mode configuration. */
            periodic_type periodic;
        };

        /** Capture mode for the session, either manual or periodic. */
        sampling_mode mode;
        /** Padding bytes. */
        uint8_t pad[7];
        /** Structure containing configuration for periodic mode. */
        mode_config_union mode_config;
    };

    /** Enable request descriptor. */
    struct request_enable {
        /** Type of performance counter block. */
        block_type type;
        /** Which SET to use: primary, secondary or tertiary. */
        prfcnt_set set;
        /** Padding bytes. */
        uint8_t pad[6];
        /**
         * Bitmask that indicates which performance counters to enable. Unavailable
         * counters will be ignored.
         */
        uint64_t enable_mask[2];
    };

    /** Scope request descriptor. */
    struct request_scope {
        /** Scope of performance counters. */
        enum class counters_scope : uint8_t {
            /** Global scope. */
            global,
        };

        /** Scope of the performance counters to capture. */
        counters_scope scope;
        /** Padding bytes. */
        uint8_t pad[7];
    };

    /** Request union. */
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    union request_union {
        /** Mode request descriptor. */
        request_mode req_mode;
        /** Enable request descriptor. */
        request_enable req_enable;
        /** Scope request descriptor. */
        request_scope req_scope;
    };

    /** Header describing the type of item in the list. */
    header hdr;
    /** Padding bytes. */
    uint8_t padding[4];
    /** Structure containing descriptor for enumeration item type. */
    request_union u;
};

/** Metadata to access a sample. */
struct sample_access {
    /**
     * Sequence number for the sample. For GET_SAMPLE, it will be set by the kernel.
     * For PUT_SAMPLE, it shall be equal to the same value provided by the kernel for
     * GET_SAMPLE.
     */
    uint64_t sequence;
    /**
     * Offset from the start of the mapped area to the first entry in the metadata list
     * (sample_metadata) for this sample.
     */
    offset_pointer<const metadata_item, uint64_t> sample_metadata_ptr;
};

// clang-format off
inline metadata_item::block_metadata::block_state_type operator~(metadata_item::block_metadata::block_state_type val) {
    const auto val_raw = static_cast<uint32_t>(val);
    return static_cast<metadata_item::block_metadata::block_state_type>(~val_raw);
}

inline bool operator!(metadata_item::block_metadata::block_state_type val) {
    const auto val_raw = static_cast<uint32_t>(val);
    return val_raw == 0;
}

inline metadata_item::block_metadata::block_state_type operator|(metadata_item::block_metadata::block_state_type lhs, metadata_item::block_metadata::block_state_type rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<metadata_item::block_metadata::block_state_type>(lhs_raw | rhs_raw);
}

inline metadata_item::block_metadata::block_state_type operator&(metadata_item::block_metadata::block_state_type lhs, metadata_item::block_metadata::block_state_type rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<metadata_item::block_metadata::block_state_type>(lhs_raw & rhs_raw);
}

inline metadata_item::block_metadata::block_state_type operator^(metadata_item::block_metadata::block_state_type lhs, metadata_item::block_metadata::block_state_type rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<metadata_item::block_metadata::block_state_type>(lhs_raw ^ rhs_raw);
}

inline metadata_item::block_metadata::block_state_type &operator|=(metadata_item::block_metadata::block_state_type &lhs, metadata_item::block_metadata::block_state_type rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline metadata_item::block_metadata::block_state_type &operator&=(metadata_item::block_metadata::block_state_type &lhs, metadata_item::block_metadata::block_state_type rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline metadata_item::block_metadata::block_state_type &operator^=(metadata_item::block_metadata::block_state_type &lhs, metadata_item::block_metadata::block_state_type rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

inline metadata_item::sample_metadata::sample_flag operator~(metadata_item::sample_metadata::sample_flag val) {
    const auto val_raw = static_cast<uint32_t>(val);
    return static_cast<metadata_item::sample_metadata::sample_flag>(~val_raw);
}

inline bool operator!(metadata_item::sample_metadata::sample_flag val) {
    const auto val_raw = static_cast<uint32_t>(val);
    return val_raw == 0;
}

inline metadata_item::sample_metadata::sample_flag operator|(metadata_item::sample_metadata::sample_flag lhs, metadata_item::sample_metadata::sample_flag rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<metadata_item::sample_metadata::sample_flag>(lhs_raw | rhs_raw);
}

inline metadata_item::sample_metadata::sample_flag operator&(metadata_item::sample_metadata::sample_flag lhs, metadata_item::sample_metadata::sample_flag rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<metadata_item::sample_metadata::sample_flag>(lhs_raw & rhs_raw);
}

inline metadata_item::sample_metadata::sample_flag operator^(metadata_item::sample_metadata::sample_flag lhs, metadata_item::sample_metadata::sample_flag rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<metadata_item::sample_metadata::sample_flag>(lhs_raw ^ rhs_raw);
}

inline metadata_item::sample_metadata::sample_flag &operator|=(metadata_item::sample_metadata::sample_flag &lhs, metadata_item::sample_metadata::sample_flag rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline metadata_item::sample_metadata::sample_flag &operator&=(metadata_item::sample_metadata::sample_flag &lhs, metadata_item::sample_metadata::sample_flag rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline metadata_item::sample_metadata::sample_flag &operator^=(metadata_item::sample_metadata::sample_flag &lhs, metadata_item::sample_metadata::sample_flag rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

// clang-format on
} // namespace kinstr_prfcnt
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
