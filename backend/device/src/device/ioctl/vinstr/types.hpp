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

/** VINSTR performance counters ioctl interface. */
namespace vinstr {
/** HWCNT dumping events. */
enum class reader_event : uint32_t {
    /** Manual request for dump. */
    manual,
    /** Periodic dump. */
    periodic,
    /** Prejob dump request. */
    prejob,
    /** Postjob dump request. */
    postjob,
};

/** Features that HWCNT reader supports. */
enum class reader_features : uint32_t {
    /** HWCNT samples are annotated with the top cycle counter. */
    cycles_top = (uint32_t{1} << 0),
    /** HWCNT samples are annotated with the shader cores cycle counter. */
    cycles_shader_core = (uint32_t{1} << 1),
};

/** GPU clock cycles. */
struct reader_metadata_cycles {
    /** The number of cycles associated with the main clock for the GPU. */
    uint64_t top;
    /** The cycles that have elapsed on the GPU shader cores. */
    uint64_t shader_cores;
};

/** HWCNT reader sample buffer metadata. */
struct reader_metadata {
    /** Time when sample was collected. */
    uint64_t timestamp;
    /** ID of an event that triggered sample collection. */
    reader_event event_id;
    /** Position in sampling area where sample buffer was stored. */
    uint32_t buffer_idx;
};

/** HWCNT reader sample buffer metadata annotated with cycle counts. */
struct reader_metadata_with_cycles {
    /** Reader metadata. */
    reader_metadata metadata;
    /** The GPU cycles that occurred since the last sample. */
    reader_metadata_cycles cycles;
};

/** HWCNT reader API version. */
struct reader_api_version {
    /** API version */
    uint32_t version;
    /** Available features in this API version. */
    reader_features features;
};

// clang-format off
inline reader_features operator~(reader_features val) {
    const auto val_raw = static_cast<uint32_t>(val);
    return static_cast<reader_features>(~val_raw);
}

inline bool operator!(reader_features val) {
    const auto val_raw = static_cast<uint32_t>(val);
    return val_raw == 0;
}

inline reader_features operator|(reader_features lhs, reader_features rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<reader_features>(lhs_raw | rhs_raw);
}

inline reader_features operator&(reader_features lhs, reader_features rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<reader_features>(lhs_raw & rhs_raw);
}

inline reader_features operator^(reader_features lhs, reader_features rhs) {
    const auto lhs_raw = static_cast<uint32_t>(lhs);
    const auto rhs_raw = static_cast<uint32_t>(rhs);
    return static_cast<reader_features>(lhs_raw ^ rhs_raw);
}

inline reader_features &operator|=(reader_features &lhs, reader_features rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline reader_features &operator&=(reader_features &lhs, reader_features rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline reader_features &operator^=(reader_features &lhs, reader_features rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

// clang-format on
} // namespace vinstr
} // namespace ioctl
} // namespace device
} // namespace hwcpipe
