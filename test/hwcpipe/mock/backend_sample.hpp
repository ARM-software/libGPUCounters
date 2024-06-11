/*
 * Copyright (c) 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "backend_manual_sampler.hpp"
#include "catch2/catch.hpp"
#include "mock_helper.h"

#include <device/hwcnt/block_metadata.hpp>

#include <cstdint>
#include <system_error>
#include <vector>

namespace hwcpipe {
namespace mock {

namespace hwcnt = device::hwcnt;

struct block_metadata {
    hwcnt::block_type type;
    const void *values;
};

struct sample_flags {
    uint32_t stretched : 1;
    uint32_t error : 1;
};

struct sample_metadata {
    uint64_t user_data;
    sample_flags flags;
    uint64_t sample_nr;
    uint64_t timestamp_ns_begin;
    uint64_t timestamp_ns_end;
    uint64_t gpu_cycle;
    uint64_t sc_cycle;
};

class backend_sample_mock {
  public:
    backend_sample_mock(reader_mock &reader, std::error_code &ec) {
        if (!reader.is_valid()) {
            ec = std::make_error_code(std::errc::invalid_argument);
        }
    }
    MOCK(sample_metadata, get_metadata, ());
    MOCK(std::vector<block_metadata>, blocks, ());
};
MOCK_DEFAULT_RET(sample_metadata, backend_sample_mock, get_metadata, {});
MOCK_DEFAULT_RET(std::vector<block_metadata>, backend_sample_mock, blocks, {});

} // namespace mock
} // namespace hwcpipe
