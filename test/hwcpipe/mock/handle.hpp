/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "mock_helper.h"

#include <device/handle.hpp>
#include <device/hwcnt/block_extents.hpp>

#include <cstddef>
#include <memory>

namespace hwcpipe {
namespace mock {

class handle_mock {
  public:
    using handle_ptr = std::unique_ptr<handle_mock>;

    handle_mock() = default;

    static bool return_valid_instance;
    static handle_ptr create(int device_number) {
        if (!return_valid_instance) {
            return_valid_instance = true;
            return nullptr;
        }
        return std::make_unique<handle_mock>();
    }
};
bool handle_mock::return_valid_instance = true;

} // namespace mock
} // namespace hwcpipe
