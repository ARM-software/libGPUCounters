/*
 * Copyright (c) 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hwcpipe/detail/internal_types.hpp"

#include <array>

namespace hwcpipe {
namespace database {

extern const std::array<counter_metadata, 417> all_counter_metadata;

} // namespace database

} // namespace hwcpipe