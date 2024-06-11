/*
 * Copyright (c) 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "instance.hpp"
#include "mock_helper.h"

#include <device/hwcnt/sampler/configuration.hpp>

#include <memory>
#include <system_error>

namespace hwcpipe {
namespace mock {

class reader_mock {
  public:
    MOCK(bool, is_valid, ());
};
MOCK_DEFAULT_RET(bool, reader_mock, is_valid, true);

class backend_manual_sampler_mock {
  public:
    backend_manual_sampler_mock(const instance_mock &inst,                                    //
                                const hwcpipe::device::hwcnt::sampler::configuration *config, //
                                size_t config_len) {}
    MOCK(std::error_code, accumulation_start, ())
    MOCK(std::error_code, accumulation_stop, (uint64_t user_data));
    MOCK(std::error_code, request_sample, (uint64_t user_data));
    MOCK(bool, valid, ());

    operator bool() { return valid(); }

    reader_mock &get_reader() { return reader_; }

  private:
    reader_mock reader_;
};

MOCK_DEFAULT_RET(std::error_code, backend_manual_sampler_mock, accumulation_start, std::error_code{});
MOCK_DEFAULT_RET(std::error_code, backend_manual_sampler_mock, accumulation_stop, std::error_code{});
MOCK_DEFAULT_RET(std::error_code, backend_manual_sampler_mock, request_sample, std::error_code{});
MOCK_DEFAULT_RET(bool, backend_manual_sampler_mock, valid, true);

} // namespace mock
} // namespace hwcpipe
