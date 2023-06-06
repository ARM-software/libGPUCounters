/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

#if __cplusplus >= 201703L
#define HWCP_NODISCARD [[nodiscard]]
#else
#define HWCP_NODISCARD __attribute__((warn_unused_result))
#endif

namespace hwcpipe {

using gpu_id_type = uint64_t;

/**
 * Structure that holds descriptive information for a counter.
 */
struct counter_metadata {
    const char *name;
    const char *units;
};

} // namespace hwcpipe