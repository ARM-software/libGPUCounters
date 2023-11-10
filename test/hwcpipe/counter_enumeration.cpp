/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

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

    return (lhs.counter == rhs.counter) && !std::strcmp(lhs.name, rhs.name) &&
           !std::strcmp(lhs.units, rhs.units);
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

const std::vector<std::pair<gpu_id_type, std::vector<hwcpipe_counter>>> some_counters_for_gpu{
    std::pair<uint32_t, std::vector<hwcpipe_counter>>(
        0x7003,
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
        const gpu_id_type test_gpu_id = test_counter.first;

        auto counters_iterable = db.get_counters_for_gpu(test_gpu_id);
        std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());

        for (auto counter : test_counter.second) {
            REQUIRE(gpu_counters.count(counter) == 1);
        };
    }
}

TEST_CASE("FindCountersForGPUProduct___DoesNotFindGPU___AssertsErrorCodeIsCorrectlyAssigned") {
    const gpu_id_type test_gpu_id = 0xDEADBEEF;
    auto counters_iterable = detail::counter_database().get_counters_for_gpu(test_gpu_id);
    std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());
    REQUIRE(gpu_counters.empty());
}

TEST_CASE(
    "FindCountersForGPUProduct___ValidateRayTracingCountersNotPresentOnNonRayTracingCapableHardware___ErrorCodeNotSet"
    "ErrorCodeNotSet") {
    const std::vector<uint32_t> non_rtu_gpus = {0x6000, 0x6001, 0x7003, 0x7000, 0x7002, 0x7001, 0x9003, 0x9000,
                                                0x9004, 0x9002, 0x9005, 0xa002, 0xa007, 0xa003, 0xa004};

    detail::counter_database db{};
    for (auto non_rtu_gpu : non_rtu_gpus) {
        auto counters_iterable = db.get_counters_for_gpu(non_rtu_gpu);
        std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());

        for (auto ctr : rtu_counters) {
            REQUIRE(gpu_counters.count(ctr) == 0);
        }
    }
}

TEST_CASE(
    "FindCountersForGPUProduct___ValidateRayTracingCountersPresentOnRayTracingCapableHardware___IteratorYieldsCounters") {
    const std::vector<uint32_t> rtu_gpus = {0xb002, 0xb003};

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
    const uint32_t non_csf_gpu = 0x6001;
    auto counters_iterable = detail::counter_database().get_counters_for_gpu(non_csf_gpu);
    std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());

    for (const auto &csf_ctr : csf_counters) {
        REQUIRE(gpu_counters.count(csf_ctr) == 0);
    }
}

TEST_CASE("FindCountersForGPUProduct___CSFSpecificCountersPresentInCSFEnabledGPU") {
    // "Mali-G710": [0xa002] CSF
    const uint32_t csf_gpu = 0xa002;
    auto counters_iterable = detail::counter_database().get_counters_for_gpu(csf_gpu);
    std::set<hwcpipe_counter> gpu_counters(counters_iterable.begin(), counters_iterable.end());

    for (const auto &csf_ctr : csf_counters) {
        REQUIRE(gpu_counters.count(csf_ctr) == 1);
    }
}

TEST_CASE("FindCounterData___ReturnsCounterDataForSpecifiedCounterName") {
    detail::counter_database db {};
    counter_metadata metadata {};

    for (const auto &ctr : test_counters) {
        auto ec = db.describe_counter(ctr.counter, metadata);
        REQUIRE(!ec);
        REQUIRE_THAT(metadata.name, Catch::Matchers::Equals(ctr.name));
        REQUIRE_THAT(metadata.units, Catch::Matchers::Equals(ctr.units));
    }
}
} // namespace hwcpipe
