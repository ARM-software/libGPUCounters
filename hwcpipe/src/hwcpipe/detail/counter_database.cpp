/*
 * Copyright (c) 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include "device/product_id.hpp"
#include "hwcpipe/all_gpu_counters.hpp"
#include "hwcpipe/counter_metadata.hpp"
#include "hwcpipe/detail/counter_database.hpp"
#include "hwcpipe/error.hpp"

namespace hwcpipe {
namespace detail {

bool counter_database::is_gpu_known(device::product_id id) const {
    namespace db = hwcpipe::database;

    auto it = db::all_gpu_counters.find(id);
    return it != db::all_gpu_counters.end();
}

gpu_counter_view<counter_database> counter_database::get_counters_for_gpu(device::product_id id) const {
    namespace db = hwcpipe::database;

    auto it = db::all_gpu_counters.find(id);
    if (it == db::all_gpu_counters.end()) {
        return {*this, {}, {}};
    }

    const auto &counters_map = it->second;
    return {*this, counters_map.cbegin(), counters_map.cend()};
}

std::error_code counter_database::describe_counter(hwcpipe_counter counter, counter_metadata &metadata) const {
    namespace db = hwcpipe::database;
    if (static_cast<size_t>(counter) >= db::all_counter_metadata.size()) {
        return hwcpipe::make_error_code(hwcpipe::errc::unknown_counter);
    }

    metadata = db::all_counter_metadata[static_cast<size_t>(counter)];
    return {};
}

counter_definition counter_database::get_counter_def(device::product_id id, hwcpipe_counter counter,
                                                     std::error_code &ec) {
    namespace db = hwcpipe::database;

    auto it = db::all_gpu_counters.find(id);
    if (it == db::all_gpu_counters.end()) {
        ec = make_error_code(hwcpipe::errc::invalid_device);
        return {};
    }

    auto counter_address_it = it->second.find(counter);
    if (counter_address_it == it->second.end()) {
        ec = make_error_code(errc::invalid_counter_for_device);
        return {};
    }

    return counter_address_it->second;
}

} // namespace detail
} // namespace hwcpipe