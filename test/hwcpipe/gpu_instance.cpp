/*
 * Copyright (c) 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include "device/product_id.hpp"

#include <catch2/catch.hpp>

#include <device/handle.hpp>
#include <hwcpipe/gpu.hpp>

#include <sstream>
#include <system_error>

namespace hwcpipe {

TEST_CASE("gpu_family") {
    using pid = device::product_id;
    std::stringstream ss{};

    SECTION("midgard") {
        ss << device::gpu_family::midgard;
        REQUIRE(ss.str() == "midgard");
    }

    SECTION("bifrost") {
        ss << device::gpu_family::bifrost;
        REQUIRE(ss.str() == "bifrost");
    }

    SECTION("valhall") {
        ss << device::gpu_family::valhall;
        REQUIRE(ss.str() == "valhall");
    }
}

TEST_CASE("gpu_frontend") {
    using pid = device::product_id;
    std::stringstream ss{};

    SECTION("jm") {
        ss << device::gpu_frontend::jm;
        REQUIRE(ss.str() == "jm");
    }

    SECTION("csf") {
        ss << device::gpu_frontend::csf;
        REQUIRE(ss.str() == "csf");
    }
}

/*
TEST_CASE("gpu_instance") {
    using gpu_instance = detail::gpu_instance_impl<mock_handle_factory, mock_instance_factory>;

    mock_handle_factory handle_factory{};
    mock_instance_factory instance_factory{};

    SECTION("existing device") {
        std::error_code ec{};
        gpu_instance dev0{0, ec, handle_factory, instance_factory};
        REQUIRE(!ec);
    }

    SECTION("non-existing device") {
        std::error_code ec{};
        gpu_instance dev2{2, ec, handle_factory, instance_factory};
        REQUIRE(ec);
    }
}

TEST_CASE("gpu_instance_finder") {
    using gpu_instance_finder = detail::gpu_instance_finder_impl<mock_handle_factory>;

    mock_handle_factory handle_factory{};
    gpu_instance_finder finder{handle_factory};

    SECTION("find_all") {
        auto devices = finder.find_all();

        CHECKED_IF(devices.size() == 4) {
            REQUIRE(devices[0] == 0);
            REQUIRE(devices[1] == 1);
            REQUIRE(devices[2] == 4);
            REQUIRE(devices[3] == 31);
        }
    }
}
*/
} // namespace hwcpipe