/*
 * Copyright (c) 2023-2026 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include "hwcpipe/all_gpu_counters.hpp"
#include "hwcpipe/detail/counter_map/counter_maps.hpp"

namespace hwcpipe {
namespace database {

    const std::unordered_map<device::product_id, const gpu_counters_map*> all_gpu_counters {
        {device::product_id::g31, detail::counter_map::g31},
        {device::product_id::g51, detail::counter_map::g51},
        {device::product_id::g52, detail::counter_map::g52},
        {device::product_id::g71, detail::counter_map::g71},
        {device::product_id::g72, detail::counter_map::g72},
        {device::product_id::g76, detail::counter_map::g76},
        {device::product_id::g57, detail::counter_map::g57},
        {device::product_id::g57_2, detail::counter_map::g57_2},
        {device::product_id::g77, detail::counter_map::g77},
        {device::product_id::g68, detail::counter_map::g68},
        {device::product_id::g78, detail::counter_map::g78},
        {device::product_id::g78ae, detail::counter_map::g78ae},
        {device::product_id::g310, detail::counter_map::g310},
        {device::product_id::g510, detail::counter_map::g510},
        {device::product_id::g610, detail::counter_map::g610},
        {device::product_id::g710, detail::counter_map::g710},
        {device::product_id::g615, detail::counter_map::g615},
        {device::product_id::g715, detail::counter_map::g715},
        {device::product_id::g620, detail::counter_map::g620},
        {device::product_id::g720, detail::counter_map::g720},
        {device::product_id::g625, detail::counter_map::g625},
        {device::product_id::g725, detail::counter_map::g725},
        {device::product_id::g1_pro, detail::counter_map::g1_pro},
        {device::product_id::g1_premium, detail::counter_map::g1_premium},
        {device::product_id::g1_ultra, detail::counter_map::g1_ultra},
        {device::product_id::g2_pro, detail::counter_map::g2_pro},
        {device::product_id::g2_premium, detail::counter_map::g2_premium},
        {device::product_id::g2_ultra, detail::counter_map::g2_ultra}
    };

} // namespace database
} // namespace hwcpipe
