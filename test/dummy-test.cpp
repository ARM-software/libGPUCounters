/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */
#include <catch2/catch.hpp>

#include <lib.hpp>

using namespace hwcpipe;

TEST_CASE("Dummy test") {
    auto product = hwcpipe::get_product_id();
    REQUIRE(product.get_gpu_family() == product_id::gpu_family::midgard);
}
