/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "detail/counter_database.hpp"
#include "hwcpipe/gpu.hpp"

namespace hwcpipe {

/**
 * @brief Provides access to information about GPU counters.
 */
class counter_database : private detail::counter_database {
  public:
    counter_database() = default;

    /**
     * @brief Returns an iterable object that yields the hwcpipe_counter values
     * that are valid for the specified GPU.
     *
     * @par
     * @code
     * hwcpipe::gpu gpu(0);
     * hwcpipe::counter_database db {};
     * hwcpipe::counter_metadata meta;
     * for (hwcpipe_counter counter : db.counters_for_gpu(gpu)) {
     *    auto ec = db.describe_counter(counter, meta);
     *    if (ec) {
     *        continue;
     *    }
     *    std::cout << "Counter[" << counter << "] - " << meta.name << std::endl;
     * }
     * @endcode
     *
     * @param [in]  gpu  The GPU to fetch counter information for.
     * @return An iterable (provides begin()/end()) list of hwcpipe_counters.
     */
    HWCP_NODISCARD auto counters_for_gpu(const gpu &gpu) const {
        return detail::counter_database::get_counters_for_gpu(gpu.get_product_id());
    }

    /**
     * Retrieves the descriptive information (name, units, etc.) for a counter.
     *
     * @param [in]  counter   The hwcpipe_counter to describe.
     * @param [out] metadata  If the counter is known this will be populated
     *                        with the descriptive information.
     * @return If the counter is not known hwcpipe::errc::unknown_counter will
     *         be returned, otherwise an empty error_code is returned.
     */
    HWCP_NODISCARD std::error_code describe_counter(hwcpipe_counter counter, counter_metadata &metadata) const {
        return detail::counter_database::describe_counter(counter, metadata);
    }
};

} // namespace hwcpipe