/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include <device/product_id.hpp>
#include <hwcpipe/counter_database.hpp>
#include <hwcpipe/gpu.hpp>
#include <hwcpipe/sampler.hpp>

#include <iomanip>
#include <iostream>

#include <unistd.h>

const char *get_product_family_name(hwcpipe::device::product_id::gpu_family f) {
    using gpu_family = hwcpipe::device::product_id::gpu_family;

    switch (f) {
    case gpu_family::bifrost:
        return "Bifrost";
    case gpu_family::midgard:
        return "Midgard";
    case gpu_family::valhall:
        return "Valhall";
    default:
        return "Unknown";
    }
}

int main() {
    // Detect all GPUs & print some info
    for (const auto &gpu : hwcpipe::find_gpus()) {
        std::cout << "------------------------------------------------------------\n"
                  << " GPU Device " << gpu.get_device_number() << ":\n"
                  << "------------------------------------------------------------\n";
        std::cout << "    Product Family:  " << get_product_family_name(gpu.get_product_id().get_gpu_family())
                  << std::endl;
        std::cout << "    Number of Cores: " << gpu.num_shader_cores() << std::endl;
        std::cout << "    Bus Width:       " << gpu.bus_width() << std::endl;
    }

    // Probe device 0 (i.e. /dev/mali0)
    auto gpu = hwcpipe::gpu(0);
    if (!gpu) {
        std::cout << "Mali GPU device 0 is missing" << std::endl;
        return -1;
    }

    // Print the counters that it supports
    auto counter_db = hwcpipe::counter_database{};
    hwcpipe::counter_metadata meta;

    std::cout << '\n'
              << "------------------------------------------------------------\n"
              << " GPU 0 Supported counters:\n"
              << "------------------------------------------------------------\n";

    for (hwcpipe_counter counter : counter_db.counters_for_gpu(gpu)) {
        auto ec = counter_db.describe_counter(counter, meta);
        if (ec) {
            // Should not happen because counters_for_gpu(gpu) only returns
            // known counters, by definition
            assert(false);
        }
        std::cout << "    " << meta.name << std::endl;
    }

    std::cout << '\n'
              << "------------------------------------------------------------\n"
              << " Sample Capture:\n"
              << "------------------------------------------------------------\n";

    std::error_code ec;
    auto config = hwcpipe::sampler_config(gpu);
    ec = config.add_counter(MaliGPUActiveCy);
    if (ec) {
        std::cout << "GPU Active Cycles counter not supported by this GPU." << std::endl;
        return -1;
    }
    ec = config.add_counter(MaliFragActiveCy);
    if (ec) {
        std::cout << "Fragment Active Cycles counter not supported by this GPU." << std::endl;
        return -1;
    }

    auto sampler = hwcpipe::sampler<>(config);

    ec = sampler.start_sampling();
    if (ec) {
        std::cout << ec.message() << std::endl;
        return -1;
    }

    hwcpipe::counter_sample sample;

    for (int i = 0; i < 10; ++i) {
        usleep(100000);
        ec = sampler.sample_now();
        if (ec) {
            std::cout << ec.message() << std::endl;
            continue;
        }

        // fetch the GPU Active Cycles counter
        ec = sampler.get_counter_value(MaliGPUActiveCy, sample);
        if (ec) {
            std::cout << ec.message() << std::endl;
            continue;
        }
        uint64_t active_cycles = sample.value;

        // Fetch the Fragment Active Cycles counter
        ec = sampler.get_counter_value(MaliFragActiveCy, sample);
        if (ec) {
            std::cout << ec.message() << std::endl;
            continue;
        }
        uint64_t fragment_cycles = sample.value;

        std::cout << "    " << std::setw(3) << i << ": "
                  << "GPU Active Cycles [" << active_cycles << "] ; "
                  << "Fragment Active Cycles [" << fragment_cycles << "]" << std::endl;
    }

    ec = sampler.stop_sampling();
    if (ec) {
        std::cout << ec.message() << std::endl;
    }
    return 0;
}
