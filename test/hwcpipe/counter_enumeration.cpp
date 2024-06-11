/*
 * Copyright (c) 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include "device/product_id.hpp"

#include <catch2/catch.hpp>

#include <hwcpipe/detail/assert.hpp>
#include <hwcpipe/detail/counter_database.hpp>

#include <cassert>
#include <cstring>
#include <iostream>
#include <set>
#include <system_error>
#include <vector>

#include <hwcpipe/hwcpipe_counter.h>
#include <sys/types.h>

namespace hwcpipe {
struct counter_info {
    /** Counter Identifier */
    hwcpipe_counter counter;

    /** Counter name. */
    const char *name;

    /** Units that are represented by the counter. */
    const char *units;
};

inline bool operator==(const counter_info &lhs, const counter_info &rhs) {
    assert(lhs.name && rhs.name);
    assert(lhs.units && rhs.units);

    return (lhs.counter == rhs.counter) && !std::strcmp(lhs.name, rhs.name) && !std::strcmp(lhs.units, rhs.units);
}

const std::vector<hwcpipe_counter> rtu_counters = {
    hwcpipe_counter::MaliRTUTri,          hwcpipe_counter::MaliRTUBox,       hwcpipe_counter::MaliRTUTriBin1,
    hwcpipe_counter::MaliRTUTriBin5,      hwcpipe_counter::MaliRTUTriBin9,   hwcpipe_counter::MaliRTUTriBin13,
    hwcpipe_counter::MaliRTUBoxBin1,      hwcpipe_counter::MaliRTUBoxBin5,   hwcpipe_counter::MaliRTUBoxBin9,
    hwcpipe_counter::MaliRTUBoxBin13,     hwcpipe_counter::MaliRTUOpaqueHit, hwcpipe_counter::MaliRTUNonOpaqueHit,
    hwcpipe_counter::MaliRTUFirstHitTerm, hwcpipe_counter::MaliRTUMiss,      hwcpipe_counter::MaliRTURay,
};

const std::vector<hwcpipe_counter> csf_counters = {
    hwcpipe_counter::MaliCSFMCUActiveCy, hwcpipe_counter::MaliCSFCEUActiveCy, hwcpipe_counter::MaliCSFLSUActiveCy,
    hwcpipe_counter::MaliCSFCS0ActiveCy, hwcpipe_counter::MaliCSFCS1ActiveCy, hwcpipe_counter::MaliCSFCS2ActiveCy,
    hwcpipe_counter::MaliCSFCS3ActiveCy,
};

const std::vector<std::pair<device::product_id, std::vector<hwcpipe_counter>>> some_counters_for_gpu{
    std::pair<device::product_id, std::vector<hwcpipe_counter>>(
        device::product_id::g31,
        {hwcpipe_counter::MaliFragTileKill, hwcpipe_counter::MaliGPUActiveCy, hwcpipe_counter::MaliExtBusRdLat192,
         hwcpipe_counter::MaliSCBusTexExtRdBt, hwcpipe_counter::MaliLSFullRd})};

// As there are a lot of counters we can test a few
const std::vector<counter_info> test_counters{
    {hwcpipe_counter::MaliFragTileKill, "Killed unchanged tiles", "tiles"},
    {hwcpipe_counter::MaliGPUActiveCy, "GPU active cycles", "cycles"},
    {hwcpipe_counter::MaliExtBusRdLat192, "Output external read latency 192-255 cycles", "beats"},
    {hwcpipe_counter::MaliSCBusTexExtRdBt, "Texture unit read beats from external memory", "beats"},
    {hwcpipe_counter::MaliLSFullRd, "Load/store unit full read issues", "cycles"}};

TEST_CASE("FindCountersForGPUProduct___FindsGPUCounters___AssertsCorrectGPUCountersReturned___ErrorCodeNotSet") {
    detail::counter_database db{};
    for (const auto &test_counter : some_counters_for_gpu) {
        const device::product_id test_pid = test_counter.first;

        auto counters_iterable = db.get_counters_for_gpu(test_pid);
        std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());

        for (auto counter : test_counter.second) {
            REQUIRE(gpu_counters.count(counter) == 1);
        };
    }
}

TEST_CASE("FindCountersForGPUProduct___DoesNotFindGPU___AssertsErrorCodeIsCorrectlyAssigned") {
    const device::product_id useless_pid{};
    auto counters_iterable = detail::counter_database().get_counters_for_gpu(useless_pid);
    std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());
    REQUIRE(gpu_counters.empty());
}

TEST_CASE(
    "FindCountersForGPUProduct___ValidateRayTracingCountersNotPresentOnNonRayTracingCapableHardware___ErrorCodeNotSet"
    "ErrorCodeNotSet") {
    const std::vector<device::product_id> non_rtu_gpus = {
        device::product_id::g71,  device::product_id::g72,  device::product_id::g31,   device::product_id::g51,
        device::product_id::g52,  device::product_id::g76,  device::product_id::g57_2, device::product_id::g77,
        device::product_id::g68,  device::product_id::g78,  device::product_id::g78ae, device::product_id::g710,
        device::product_id::g610, device::product_id::g510, device::product_id::g310};

    detail::counter_database db{};
    for (auto non_rtu_gpu : non_rtu_gpus) {
        auto counters_iterable = db.get_counters_for_gpu(non_rtu_gpu);
        std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());

        for (auto ctr : rtu_counters) {
            REQUIRE(gpu_counters.count(ctr) == 0);
        }
    }
}

TEST_CASE("FindCountersForGPUProduct___ValidateRayTracingCountersPresentOnRayTracingCapableHardware___"
          "IteratorYieldsCounters") {
    const std::vector<device::product_id> rtu_gpus = {device::product_id::g715, device::product_id::g615};

    detail::counter_database db{};
    for (auto rtu_gpu : rtu_gpus) {
        auto counters_iterable = db.get_counters_for_gpu(rtu_gpu);
        std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());

        for (auto ctr : rtu_counters) {
            REQUIRE(gpu_counters.count(ctr) == 1);
        }
    }
}

TEST_CASE("FindCountersForGPUProduct___CSFSpecificCountersNotPresentInJobManagerEnabledGPU___ErrorCodeNotSet") {
    // "Mali-G72": [0x6001] Job Manager
    const device::product_id non_csf_gpu = device::product_id::g72;
    auto counters_iterable = detail::counter_database().get_counters_for_gpu(non_csf_gpu);
    std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());

    for (const auto &csf_ctr : csf_counters) {
        REQUIRE(gpu_counters.count(csf_ctr) == 0);
    }
}

TEST_CASE("FindCountersForGPUProduct___CSFSpecificCountersPresentInCSFEnabledGPU") {
    // "Mali-G710": [0xa002] CSF
    const device::product_id csf_gpu = device::product_id::g710;
    auto counters_iterable = detail::counter_database().get_counters_for_gpu(csf_gpu);
    std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());

    for (const auto &csf_ctr : csf_counters) {
        REQUIRE(gpu_counters.count(csf_ctr) == 1);
    }
}

TEST_CASE("FindCounterData___ReturnsCounterDataForSpecifiedCounterName") {
    detail::counter_database db{};
    counter_metadata metadata{};

    for (const auto &ctr : test_counters) {
        auto ec = db.describe_counter(ctr.counter, metadata);
        REQUIRE(!ec);
        REQUIRE_THAT(metadata.name, Catch::Matchers::Equals(ctr.name));
        REQUIRE_THAT(metadata.units, Catch::Matchers::Equals(ctr.units));
    }
}
} // namespace hwcpipe
