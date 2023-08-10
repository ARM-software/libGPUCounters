/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "handle.hpp"
#include "mock_helper.h"

#include <device/constants.hpp>
#include <device/hwcnt/block_extents.hpp>

#include <cstddef>
#include <memory>

namespace hwcpipe {
namespace mock {

using sample_values_type = device::hwcnt::sample_values_type;

class block_extents_mock {
  public:
    MOCK(sample_values_type, values_type, ());
};
MOCK_DEFAULT_RET(sample_values_type, block_extents_mock, values_type, sample_values_type::uint32);

class instance_mock {
  public:
    using instance_ptr = std::unique_ptr<instance_mock>;

    static bool return_valid_instance;

    static instance_ptr create(handle_mock &hndl) {
        if (!return_valid_instance) {
            return_valid_instance = true;
            return nullptr;
        }
        return std::make_unique<instance_mock>();
    }

    instance_mock() = default;

    MOCK(block_extents_mock, get_hwcnt_block_extents, ());
    MOCK(device::constants, get_constants, ());
};

bool instance_mock::return_valid_instance = true;

MOCK_DEFAULT_RET(block_extents_mock, instance_mock, get_hwcnt_block_extents, {});
MOCK_DEFAULT_RET(device::constants, instance_mock, get_constants, {});

} // namespace mock
} // namespace hwcpipe
