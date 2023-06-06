/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */
#include "hwcpipe/error.hpp"

#include <system_error>

namespace hwcpipe {
namespace {

class hwcpipe_error_category : public std::error_category {
  public:
    const char *name() const noexcept override { return "HWCPipe error"; }

    std::string message(int code) const noexcept override {
        switch (static_cast<errc>(code)) {
        case errc::invalid_device:
            return "Invalid GPU device.";
        case errc::unknown_counter:
            return "Unknown counter name";
        case errc::invalid_counter_for_device:
            return "Counter not supported by GPU";
        case errc::sampler_config_invalid:
            return "Invalid configuration (no counters added?)";
        case errc::backend_sampler_failure:
            return "Backend sampler failure";
        case errc::backend_creation_failed:
            return "Backend creation failed";
        case errc::sampling_already_started:
            return "Sampling already started";
        case errc::sampling_not_started:
            return "Sampling not started";
        case errc::sample_collection_failure:
            return "Sample collection failure";
        case errc::accumulation_start_failed:
            return "Failed to start accumulation";
        case errc::accumulation_stop_failed:
            return "Failed to stop accumulation";

        default:
            return "Unknown error";
        }
    }
};

} // unnamed namespace

const hwcpipe_error_category lib_error_category{};

const std::error_category &error_category() { return lib_error_category; }

} // namespace hwcpipe