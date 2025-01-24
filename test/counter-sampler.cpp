/*
 * Copyright (c) 2023-2025 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include "device/constants.hpp"
#include "device/handle.hpp"
#include "device/hwcnt/block_metadata.hpp"
#include "device/hwcnt/sample.hpp"
#include "device/hwcnt/sampler/manual.hpp"
#include "device/instance.hpp"
#include "hwcpipe/error.hpp"
#include "hwcpipe/gpu.hpp"
#include "hwcpipe/hwcpipe_counter.h"
#include "hwcpipe/mock/backend_manual_sampler.hpp"
#include "hwcpipe/mock/backend_sample.hpp"
#include "hwcpipe/mock/handle.hpp"
#include "hwcpipe/mock/instance.hpp"
#include "hwcpipe/mock/mock_helper.h"
#include "hwcpipe/types.hpp"

#include <catch2/catch.hpp>

#include <hwcpipe/sampler.hpp>

#include <cstddef>
#include <cstdint>
#include <ios>
#include <memory>
#include <system_error>

#include <sys/types.h>

namespace hwcpipe {

struct hwcpipe_sampler_mock_policy {
    using handle_type = hwcpipe::mock::handle_mock;
    using instance_type = hwcpipe::mock::instance_mock;
    using sampler_type = hwcpipe::mock::backend_manual_sampler_mock;
    using sample_type = hwcpipe::mock::backend_sample_mock;
};

using namespace hwcpipe::mock;
using sampler_t = hwcpipe::sampler<hwcpipe_sampler_mock_policy>;

TEST_CASE("counter_sampler__DefaultConstructor") {
    SECTION("uint64") {
        counter_sample sample;

        REQUIRE(sample.timestamp == 0);
        REQUIRE(sample.value.uint64 == static_cast<uint64_t>(0UL));
        REQUIRE(sample.type == counter_sample::type::uint64);
    }
}

TEST_CASE("counter_sampler__CorrectTypesAreAssigned") {
    SECTION("uint64") {
        counter_sample sample{hwcpipe_counter(), 0xbaadcafe, static_cast<uint64_t>(42UL)};

        REQUIRE(sample.timestamp == 0xbaadcafe);
        REQUIRE(sample.value.uint64 == static_cast<uint64_t>(42));
        REQUIRE(sample.type == counter_sample::type::uint64);
    }

    SECTION("float64") {
        counter_sample sample{hwcpipe_counter(), 0xbaadcafe, 4.2};

        REQUIRE(sample.timestamp == 0xbaadcafe);
        REQUIRE(sample.value.float64 == 4.2);
        REQUIRE(sample.type == counter_sample::type::float64);
    }
}

TEST_CASE("ValidGpuIDPassedToCounter___CountersCorrectlySetInConfiguration___NoErrorCode") {
    std::error_code ec;

    sampler_config config{device::product_id::g31, 0};

    hwcpipe_counter some_counter_1 = hwcpipe_counter::MaliGPUActiveCy;
    uint32_t some_counter_offset_1 = 6;
    hwcpipe_counter some_counter_2 = hwcpipe_counter::MaliTilerPosCacheHit;
    uint32_t some_counter_offset_2 = 26;

    SECTION("Single Counter Set") {
        auto ec = config.add_counter(some_counter_1);
        REQUIRE(!ec);

        auto backend_configs = config.build_backend_config_list();

        // Validate enable map is set correctly
        REQUIRE(backend_configs.size() == 1);
        const auto &cfg0 = backend_configs[0];
        REQUIRE(cfg0.type == hwcpipe::device::hwcnt::block_type::fe);
        REQUIRE(cfg0.enable_map.test(some_counter_offset_1));
    }

    SECTION("Multiple Counter Set") {
        auto ec = config.add_counter(some_counter_1);
        REQUIRE(!ec);
        ec = config.add_counter(some_counter_2);
        REQUIRE(!ec);

        auto backend_configs = config.build_backend_config_list();

        // Validate enable map is set correctly
        REQUIRE(backend_configs.size() == 2);
        for (const auto &cfg : backend_configs) {
            if (cfg.type == device::hwcnt::block_type::fe) {
                REQUIRE(cfg.enable_map.test(some_counter_offset_1));
            } else if (cfg.type == device::hwcnt::block_type::tiler) {
                REQUIRE(cfg.enable_map.test(some_counter_offset_2));
            } else {
                FAIL("Unexpected block type");
            }
        }
    }
}

TEST_CASE("ValidCorrectErrorsReported___ErrorCodeIsSetAppropriately") {
    std::error_code ec;

    hwcpipe_counter some_counter_1 = hwcpipe_counter::MaliGPUActiveCy;
    uint32_t some_counter_offset_1 = 6;
    hwcpipe_counter some_counter_2 = hwcpipe_counter::MaliTilerPosCacheHit;
    uint32_t some_counter_offset_2 = 26;

    REQUIRE(!ec);

    SECTION("GPU ID not found") {
        sampler_config config({}, 0);

        auto ec = config.add_counter(some_counter_2);

        REQUIRE(ec == hwcpipe::make_error_code(hwcpipe::errc::invalid_device));
    }

    SECTION("Counter not found") {
        sampler_config config(device::product_id::g31, 0);

        const auto unknown_counter = hwcpipe_counter::MaliRTURay;
        auto ec = config.add_counter(unknown_counter);

        REQUIRE(ec == hwcpipe::make_error_code(hwcpipe::errc::invalid_counter_for_device));
    }

    SECTION("Config with no counters results in invalid sampler") {
        sampler_config config(device::product_id::g31, 0);

        sampler_t sampler(config);
        REQUIRE_FALSE(sampler);

        auto ec = sampler.start_sampling();
        REQUIRE(ec == make_error_code(errc::sampler_config_invalid));
    }
}

TEST_CASE("SamplerReportsCorrectErrorCode__WhenSartSamplingIsCalled") {
    sampler_config config(device::product_id::g31, 0);
    auto ec = config.add_counter(hwcpipe_counter::MaliGPUActiveCy);
    REQUIRE(!ec);

    sampler_t test_sampler(config);
    REQUIRE(test_sampler);

    SECTION("Starting sampler called twice") {
        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        ec = test_sampler.start_sampling();
        REQUIRE(ec == hwcpipe::make_error_code(hwcpipe::errc::sampling_already_started));
    }

    SECTION("Sample can be restarted if stop was called") {
        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        ec = test_sampler.stop_sampling();
        REQUIRE(!ec);

        ec = test_sampler.start_sampling();
        REQUIRE(!ec);
    }

    SECTION("Starting sampler can't create backend instance") {
        mock::instance_mock::return_valid_instance = false;
        test_sampler = sampler_t(config);

        auto ec = test_sampler.start_sampling();
        REQUIRE(ec == hwcpipe::make_error_code(hwcpipe::errc::backend_creation_failed));
    }

    SECTION("Starting sampler can't start backend accumulation") {
        EXPECT_CALL(backend_manual_sampler_mock, accumulation_start, //
                    std::make_error_code(std::errc::invalid_argument));

        auto ec = test_sampler.start_sampling();
        REQUIRE(ec == make_error_code(errc::accumulation_start_failed));
    }
}

TEST_CASE("SamplerReportsCorrectErrorCode__WhenSampleNowIsCalled") {
    sampler_config config{device::product_id::g31, 0};
    auto ec = config.add_counter(MaliGPUActiveCy);
    REQUIRE(!ec);
    ec = config.add_counter(MaliFragActiveCy);
    REQUIRE(!ec);

    sampler_t test_sampler(config);
    REQUIRE(test_sampler);

    using counter_offset_pair = std::pair<hwcpipe_counter, uint32_t>;

    counter_offset_pair c1_fe = {MaliGPUActiveCy, 6};
    counter_offset_pair c2_core = {MaliFragActiveCy, 4};

    std::vector<block_metadata> blocks_list(2);
    std::vector<uint32_t> values_fe(10, 0);
    std::vector<uint32_t> values_core(10, 0);

    SECTION("Sample now is called after start sampling") {
        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        ec = test_sampler.sample_now();
        REQUIRE(!ec);
    }

    SECTION("Sample now is called without start sampling") {
        auto ec = test_sampler.sample_now();
        REQUIRE(ec == make_error_code(errc::sampling_not_started));
    }

    SECTION("Sample now fail to request sample from backend") {
        EXPECT_CALL(backend_manual_sampler_mock, request_sample, //
                    std::make_error_code(std::errc::invalid_argument));

        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        ec = test_sampler.sample_now();
        REQUIRE(ec == make_error_code(errc::sample_collection_failure));
    }

    SECTION("Sample now fail to create backend sample") {
        EXPECT_CALL(reader_mock, is_valid, false);

        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        ec = test_sampler.sample_now();
        REQUIRE(ec);
    }

    SECTION("Sample now has no metadata error flags") {
        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        blocks_list[0] = {hwcnt::block_type::fe, values_fe.data()};
        blocks_list[1] = {hwcnt::block_type::core, values_core.data()};
        EXPECT_CALL(backend_sample_mock, blocks, blocks_list);

        ec = test_sampler.sample_now();
        REQUIRE(!ec);
    }

    SECTION("Sample now gets unrequested block type") {
        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        blocks_list[0] = {hwcnt::block_type::fe, values_fe.data()};
        // Unrequested block
        blocks_list[1] = {hwcnt::block_type::tiler, nullptr};
        EXPECT_CALL(backend_sample_mock, blocks, blocks_list);

        ec = test_sampler.sample_now();
        REQUIRE(!ec);
    }

    SECTION("Sample now metadata has error flags") {
        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        sample_metadata metadata{};
        metadata.flags.error = 1;

        blocks_list[0] = {hwcnt::block_type::fe, values_fe.data()};
        blocks_list[1] = {hwcnt::block_type::core, values_core.data()};
        EXPECT_CALL(backend_sample_mock, blocks, blocks_list);
        EXPECT_CALL(backend_sample_mock, get_metadata, metadata);

        ec = test_sampler.sample_now();
        REQUIRE(ec == make_error_code(errc::sample_collection_failure));
    }
}

TEST_CASE("SamplerReportsCorrectErrorCode__WhenStopSamplingIsCalled") {
    sampler_config config(device::product_id::g31, 0);
    auto ec = config.add_counter(hwcpipe_counter::MaliGPUActiveCy);
    REQUIRE(!ec);

    sampler_t test_sampler(config);
    REQUIRE(test_sampler);

    SECTION("Stop Sampling is called without sampling started") {
        auto ec = test_sampler.stop_sampling();
        REQUIRE(ec == make_error_code(errc::sampling_not_started));
    }

    SECTION("Stop Sampling is called after sampling started") {
        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        ec = test_sampler.stop_sampling();
        REQUIRE(!ec);
    }

    SECTION("Stop Sampling is called twice after sampling started") {
        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        ec = test_sampler.stop_sampling();
        REQUIRE(!ec);

        ec = test_sampler.stop_sampling();
        REQUIRE(ec == make_error_code(errc::sampling_not_started));
    }

    SECTION("Stop Sampling can't stop backend accumulation") {
        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        EXPECT_CALL(backend_manual_sampler_mock, accumulation_stop, //
                    std::make_error_code(std::errc::invalid_argument));

        ec = test_sampler.stop_sampling();
        REQUIRE(ec == make_error_code(errc::accumulation_stop_failed));
    }

    SECTION("Sample now is called after sampling stop") {
        auto ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        ec = test_sampler.stop_sampling();
        REQUIRE(!ec);

        ec = test_sampler.sample_now();
        REQUIRE(ec == make_error_code(errc::sampling_not_started));
    }
}

TEST_CASE("SamplerReportsCorrectErrorCode__WhenGetCounterValueIsCalled") {
    using counter_offset_pair = std::pair<hwcpipe_counter, uint32_t>;

    counter_offset_pair c1_fe = {MaliGPUActiveCy, 6};
    counter_offset_pair c2_core = {MaliFragActiveCy, 4};

    std::vector<block_metadata> blocks_list(3);
    std::vector<uint32_t> values_fe(10, 0);
    std::vector<uint32_t> values_core0(10, 0);
    std::vector<uint32_t> values_core1(10, 0);

    values_fe[c1_fe.second] = 0xFEFE;
    values_core0[c2_core.second] = 0xC0C0;
    values_core1[c2_core.second] = 0x0C0C;

    sampler_config config(device::product_id::g31, 0);
    auto ec = config.add_counter(c1_fe.first);
    REQUIRE(!ec);
    ec = config.add_counter(c2_core.first);
    REQUIRE(!ec);

    sampler_t test_sampler(config);
    ec = test_sampler.start_sampling();
    REQUIRE(!ec);

    SECTION("Reading registered counters") {
        blocks_list[0] = {hwcnt::block_type::fe, values_fe.data()};
        blocks_list[1] = {hwcnt::block_type::core, values_core0.data()};
        blocks_list[2] = {hwcnt::block_type::core, values_core1.data()};

        EXPECT_CALL(backend_sample_mock, blocks, blocks_list);

        ec = test_sampler.sample_now();
        REQUIRE(!ec);

        hwcpipe::counter_sample sample{};
        ec = test_sampler.get_counter_value(c1_fe.first, sample);
        REQUIRE(!ec);
        REQUIRE(sample.value.uint64 == 0xFEFE);
        REQUIRE(sample.type == counter_sample::type::uint64);

        ec = test_sampler.get_counter_value(c2_core.first, sample);
        REQUIRE(!ec);
        // Sum of values from Core[0] and Core[1]
        REQUIRE(sample.value.uint64 == 0xCCCC);
        REQUIRE(sample.type == counter_sample::type::uint64);

        for (const auto &sample : test_sampler.sample_view()) {
            if (sample.counter == c1_fe.first) {
                REQUIRE(sample.value.uint64 == 0xFEFE);
                REQUIRE(sample.type == counter_sample::type::uint64);
            } else if (sample.counter == c2_core.first) {
                REQUIRE(sample.value.uint64 == 0xCCCC);
                REQUIRE(sample.type == counter_sample::type::uint64);
            } else {
                FAIL("Unexpected counter value");
            }
        }
    }

    SECTION("Sample and read value two consequent times") {
        blocks_list[0] = {hwcnt::block_type::fe, values_fe.data()};
        blocks_list[1] = {hwcnt::block_type::core, values_core0.data()};
        blocks_list[2] = {hwcnt::block_type::core, values_core1.data()};

        // First Sample
        EXPECT_CALL(backend_sample_mock, blocks, blocks_list);
        ec = test_sampler.sample_now();
        REQUIRE(!ec);

        hwcpipe::counter_sample sample{};
        ec = test_sampler.get_counter_value(c1_fe.first, sample);
        REQUIRE(!ec);
        REQUIRE(sample.value.uint64 == 0xFEFE);
        REQUIRE(sample.type == counter_sample::type::uint64);

        ec = test_sampler.get_counter_value(c2_core.first, sample);
        REQUIRE(!ec);
        // Sum of values from Core[0] and Core[1]
        REQUIRE(sample.value.uint64 == 0xCCCC);
        REQUIRE(sample.type == counter_sample::type::uint64);

        // Second Sample
        EXPECT_CALL(backend_sample_mock, blocks, blocks_list);
        ec = test_sampler.sample_now();
        REQUIRE(!ec);

        ec = test_sampler.get_counter_value(c1_fe.first, sample);
        REQUIRE(!ec);
        REQUIRE(sample.value.uint64 == 0xFEFE);
        REQUIRE(sample.type == counter_sample::type::uint64);

        ec = test_sampler.get_counter_value(c2_core.first, sample);
        REQUIRE(!ec);
        // Sum of values from Core[0] and Core[1]
        REQUIRE(sample.value.uint64 == 0xCCCC);
        REQUIRE(sample.type == counter_sample::type::uint64);
    }

    SECTION("Reading unregistered counters") {
        blocks_list[0] = {hwcnt::block_type::fe, values_fe.data()};
        blocks_list[1] = {hwcnt::block_type::core, values_core0.data()};
        blocks_list[2] = {hwcnt::block_type::core, values_core1.data()};
        EXPECT_CALL(backend_sample_mock, blocks, blocks_list);

        ec = test_sampler.sample_now();
        REQUIRE(!ec);

        hwcpipe::counter_sample sample{};
        ec = test_sampler.get_counter_value(hwcpipe_counter::MaliTilerActiveCy, sample);
        REQUIRE(ec == make_error_code(errc::unknown_counter));
    }
}

TEST_CASE("SamplerReadsCorrectValues__WhenValuesTypeIsUint64") {
    using counter_offset_pair = std::pair<hwcpipe_counter, uint32_t>;

    counter_offset_pair c1_fe = {MaliGPUActiveCy, 6};
    counter_offset_pair c2_core = {MaliFragActiveCy, 4};

    std::vector<block_metadata> blocks_list(3);
    std::vector<uint64_t> values_fe(10, 0);
    std::vector<uint64_t> values_core0(10, 0);
    std::vector<uint64_t> values_core1(10, 0);

    values_fe[c1_fe.second] = 0xFEFE0000UL;
    values_core0[c2_core.second] = 0xC0C00000UL;
    values_core1[c2_core.second] = 0x0C0C0000UL;

    sampler_config config(device::product_id::g31, 0);
    auto ec = config.add_counter(c1_fe.first);
    REQUIRE(!ec);
    ec = config.add_counter(c2_core.first);
    REQUIRE(!ec);

    SECTION("Reading registered counters") {
        blocks_list[0] = {hwcnt::block_type::fe, values_fe.data()};
        blocks_list[1] = {hwcnt::block_type::core, values_core0.data()};
        blocks_list[2] = {hwcnt::block_type::core, values_core1.data()};

        EXPECT_CALL(backend_sample_mock, blocks, blocks_list);
        EXPECT_CALL(block_extents_mock, values_type, sample_values_type::uint64);

        sampler_t test_sampler(config);
        ec = test_sampler.start_sampling();
        REQUIRE(!ec);

        auto ec = test_sampler.sample_now();
        REQUIRE(!ec);

        hwcpipe::counter_sample sample{};
        ec = test_sampler.get_counter_value(c1_fe.first, sample);
        REQUIRE(!ec);
        REQUIRE(sample.value.uint64 == 0xFEFE0000UL);
        REQUIRE(sample.type == counter_sample::type::uint64);

        ec = test_sampler.get_counter_value(c2_core.first, sample);
        REQUIRE(!ec);
        // Sum of values from Core[0] and Core[1]
        REQUIRE(sample.value.uint64 == 0xCCCC0000UL);
        REQUIRE(sample.type == counter_sample::type::uint64);
    }
}

TEST_CASE("SamplerReadsCorrectValues__WhenCounterValueIsShifted") {
    sampler_config config(device::product_id::g715, 0); // Turse

    SECTION("MaliFragThread shifted by 2") {
        using counter_offset_pair = std::pair<hwcpipe_counter, uint32_t>;

        // set up MaliFragThread to return 0xbaadcafe value
        std::vector<block_metadata> blocks_list(1);
        std::vector<uint32_t> values_core(70);
        values_core[69] = 0xbaadcafe;
        blocks_list[0] = {hwcnt::block_type::core, values_core.data()};
        EXPECT_CALL(backend_sample_mock, blocks, blocks_list);

        // sample counter
        REQUIRE(!config.add_counter(hwcpipe_counter::MaliFragThread));
        sampler_t test_sampler = sampler_t(config);
        REQUIRE(!test_sampler.start_sampling());
        REQUIRE(!test_sampler.sample_now());
        REQUIRE(!test_sampler.stop_sampling());
        hwcpipe::counter_sample sample{};
        REQUIRE(!test_sampler.get_counter_value(hwcpipe_counter::MaliFragThread, sample));
        REQUIRE(sample.value.uint64 == (0xbaadcafe << 2));
        REQUIRE(sample.type == counter_sample::type::uint64);
    }
}

TEST_CASE("SamplerReadsCorrectValues__WhenSamplesAreExpressions") {
    std::error_code ec{};

    sampler_config config(device::product_id::g31, 0);

    SECTION("Read expression") {
        using counter_offset_pair = std::pair<hwcpipe_counter, uint32_t>;

        counter_offset_pair c1_tiler = {MaliTilerActiveCy, 4};
        counter_offset_pair c2_fe = {MaliGPUActiveCy, 6};

        std::vector<block_metadata> blocks_list(2);
        std::vector<uint32_t> values_tiler(5);
        std::vector<uint32_t> values_fe(7);
        values_tiler[c1_tiler.second] = 4;
        values_fe[c2_fe.second] = 2;

        blocks_list[0] = {hwcnt::block_type::fe, values_fe.data()};
        blocks_list[1] = {hwcnt::block_type::tiler, values_tiler.data()};
        EXPECT_CALL(backend_sample_mock, blocks, blocks_list);

        ec = config.add_counter(hwcpipe_counter::MaliTilerUtil); // (MaliTilerActiveCy / MaliGPUActiveCy) * 100
        REQUIRE(!ec);

        sampler_t test_sampler = sampler_t(config);

        ec = test_sampler.start_sampling();
        REQUIRE(!ec);
        ec = test_sampler.sample_now();
        REQUIRE(!ec);
        ec = test_sampler.stop_sampling();
        REQUIRE(!ec);

        hwcpipe::counter_sample sample{};

        ec = test_sampler.get_counter_value(hwcpipe_counter::MaliTilerUtil, sample);
        REQUIRE(!ec);
        REQUIRE(sample.value.float64 == (4.0 / 2.0) * 100.0);
        REQUIRE(sample.type == counter_sample::type::float64);
    }
}

TEST_CASE("ExpressionCounterGivenToSamplerConfig__CounterDependenciesAreSet") {
    std::error_code ec;
    sampler_config config{device::product_id::g31, 0};

    SECTION("Unsupported expression counter") {
        ec = config.add_counter(hwcpipe_counter::MaliRTUUtil);
        REQUIRE(ec);
    }

    SECTION("Single expression counter") {

        // {hwcpipe_counter::MaliTilerUtil,
        //  hwcpipe::detail::counter_definition(
        //      {MaliTilerUtil_v0, {hwcpipe_counter::MaliGPUActiveCy, hwcpipe_counter::MaliTilerActiveCy}})},

        ec = config.add_counter(hwcpipe_counter::MaliTilerUtil);
        REQUIRE(!ec);

        CHECKED_IF(config.get_valid_counters().size() == 3) {
            const auto &counters = config.get_valid_counters();
            REQUIRE(counters.find(hwcpipe_counter::MaliTilerUtil) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliTilerActiveCy) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliGPUActiveCy) != counters.end());
        }
    }

    SECTION("Two expression counters with overlapping counters") {

        ec = config.add_counter(hwcpipe_counter::MaliGeomSampleCullRate);
        REQUIRE(!ec);

        CHECKED_IF(config.get_valid_counters().size() == 5) {
            const auto &counters = config.get_valid_counters();
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomSampleCullRate) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomSampleCullPrim) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomFaceXYPlaneCullPrim) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomVisiblePrim) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomZPlaneCullPrim) != counters.end());
        }

        // {hwcpipe_counter::MaliGeomZPlaneCullRate, hwcpipe::detail::counter_definition({get_stub,
        //      {hwcpipe_counter::MaliGeomZPlaneCullPrim,
        //       hwcpipe_counter::MaliGeomSampleCullPrim,
        //       hwcpipe_counter::MaliGeomVisiblePrim,
        //       hwcpipe_counter::MaliGeomFaceXYPlaneCullPrim}})},

        ec = config.add_counter(hwcpipe_counter::MaliGeomZPlaneCullRate);
        REQUIRE(!ec);

        CHECKED_IF(config.get_valid_counters().size() == 6) {
            const auto &counters = config.get_valid_counters();
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomSampleCullRate) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomSampleCullPrim) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomFaceXYPlaneCullPrim) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomVisiblePrim) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomZPlaneCullPrim) != counters.end());
            REQUIRE(counters.find(hwcpipe_counter::MaliGeomZPlaneCullRate) != counters.end());
        }
    }
}

} // namespace hwcpipe
