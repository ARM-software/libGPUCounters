/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hwcpipe/types.hpp"

#include <device/handle.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sample.hpp>
#include <device/hwcnt/sampler/manual.hpp>
#include <device/instance.hpp>

namespace hwcpipe {
namespace detail {

/**
 * Structure representing the block type/offset address of a counter within
 * a particular GPU's PMU data.
 */
struct block_offset {
    uint32_t offset;
    hwcpipe::device::hwcnt::block_type block_type;
};

struct hwcpipe_backend_policy {
    using handle_type = device::handle;
    using instance_type = device::instance;
    using sampler_type = device::hwcnt::sampler::manual;
    using sample_type = device::hwcnt::sample;
};

} // namespace detail
} // namespace hwcpipe
