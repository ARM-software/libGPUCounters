/*
 * Copyright (c) 2023-2026 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hwcpipe/all_gpu_counters.hpp"

namespace hwcpipe {
namespace database {
namespace detail {
namespace counter_map {

    /* Bifrost */
    extern const gpu_counters_map* g31;
    extern const gpu_counters_map* g51;
    extern const gpu_counters_map* g52;
    extern const gpu_counters_map* g71;
    extern const gpu_counters_map* g72;
    extern const gpu_counters_map* g76;
    /* Valhall */
    extern const gpu_counters_map* g57;
    extern const gpu_counters_map* g57_2;
    extern const gpu_counters_map* g77;
    extern const gpu_counters_map* g68;
    extern const gpu_counters_map* g78;
    extern const gpu_counters_map* g78ae;
    extern const gpu_counters_map* g310;
    extern const gpu_counters_map* g510;
    extern const gpu_counters_map* g610;
    extern const gpu_counters_map* g710;
    extern const gpu_counters_map* g615;
    extern const gpu_counters_map* g715;
    /* 5th Generation */
    extern const gpu_counters_map* g620;
    extern const gpu_counters_map* g720;
    extern const gpu_counters_map* g625;
    extern const gpu_counters_map* g725;
    extern const gpu_counters_map* g1_pro;
    extern const gpu_counters_map* g1_premium;
    extern const gpu_counters_map* g1_ultra;
    extern const gpu_counters_map* g2_pro;
    extern const gpu_counters_map* g2_premium;
    extern const gpu_counters_map* g2_ultra;

} // namespace counter_map
} // namespace detail
} // namespace database
} // namespace hwcpipe
