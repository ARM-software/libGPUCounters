/*
 * Copyright (c) 2023-2024 Arm Limited.
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

const char *get_product_family_name(hwcpipe::device::gpu_family f) {
    using gpu_family = hwcpipe::device::gpu_family;

    switch (f) {
    case gpu_family::bifrost:
        return "Bifrost";
    case gpu_family::midgard:
        return "Midgard";
    case gpu_family::valhall:
        return "Valhall";
    case gpu_family::fifthgen:
        return "Arm 5th Gen";
    default:
        return "Unknown";
    }
}

void print_sample_value(const hwcpipe::counter_sample &sample) {
    switch (sample.type) {
    case hwcpipe::counter_sample::type::uint64:
        std::cout << sample.value.uint64;
        return;
    case hwcpipe::counter_sample::type::float64:
        std::cout << sample.value.float64;
        return;
    }
}

int main() {
    // Detect all GPUs & print some info
    for (const auto &gpu : hwcpipe::find_gpus()) {
        std::cout << "------------------------------------------------------------\n"
                  << " GPU Device " << gpu.get_device_number() << ":\n"
                  << "------------------------------------------------------------\n";
        std::cout << "    Product Family:  " << get_product_family_name(gpu.get_gpu_family()) << std::endl;
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


    hwcpipe_counter activity_counter = MaliFragActiveCy;
    const char * activity_counter_name = "Fragment Active Cycles";
    if (gpu.get_gpu_family() == hwcpipe::device::gpu_family::fifthgen) {
        // 5th Gen GPUs don't have the FragActiveCy counter
        activity_counter = MaliAnyActiveCy;
        activity_counter_name = "Shader Core Active";
    }

    ec = config.add_counter(activity_counter);
    if (ec) {
        std::cout << activity_counter_name << " counter not supported by this GPU." << std::endl;
        return -1;
    }
    ec = config.add_counter(MaliGeomSampleCullRate);
    if (ec) {
        std::cout << "Geometry Sample Cull Rate counter not supported by this GPU." << std::endl;
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

        std::cout << "GPU Active cycles [";
        print_sample_value(sample);
        std::cout << "] ; ";

        // Fetch the Fragment Active Cycles counter
        ec = sampler.get_counter_value(activity_counter, sample);
        if (ec) {
            std::cout << ec.message() << std::endl;
            continue;
        }

        std::cout << activity_counter_name << " [";
        print_sample_value(sample);
        std::cout << "] ; ";

        // Fetch the Geometry Sample Cull Rate counter
        ec = sampler.get_counter_value(MaliGeomSampleCullRate, sample);
        if (ec) {
            std::cout << ec.message() << std::endl;
            continue;
        }

        std::cout << "Geometry Sample Cull Rate [";
        print_sample_value(sample);
        std::cout << "]" << std::endl;
    }

    ec = sampler.stop_sampling();
    if (ec) {
        std::cout << ec.message() << std::endl;
    }
    return 0;
}
