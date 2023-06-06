/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <string>
#include <system_error>

namespace hwcpipe {

enum class errc {
    invalid_device = 1,
    unknown_counter,
    invalid_counter_for_device,
    // Sampler - construction errors
    sampler_config_invalid,
    backend_sampler_failure,
    backend_creation_failed,
    // Sampler - sample process errors
    sampling_already_started,
    sampling_not_started,
    sample_collection_failure,
    accumulation_start_failed,
    accumulation_stop_failed
};

/**
 * @brief Returns a reference to HWCPipe's custom error category for use when
 * constructing error codes.
 */
const std::error_category &error_category();

/**
 * @brief Constructs a standard error_code structure for the given HWCpipe error.
 *
 * @param e The HWCPipe error code.
 * @return std::error_code
 */
inline std::error_code make_error_code(errc e) { return {static_cast<int>(e), error_category()}; }

} // namespace hwcpipe
