/*
 * Copyright (c) 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "device/product_id.hpp"
#include "hwcpipe/detail/internal_types.hpp"
#include "hwcpipe/hwcpipe_counter.h"

#include <system_error>
#include <unordered_map>

namespace hwcpipe {
namespace detail {

using backing_map_type = std::unordered_map<hwcpipe_counter, detail::counter_definition>;

/**
 * @brief An type that provides an enumerable view over the counters for a
 * particular GPU.
 */
template <typename counter_database_t>
class gpu_counter_view {
  public:
    using backing_iterator = backing_map_type::const_iterator;

  private:
    /** The actual iterator type that enumerates the counters for a GPU */
    class iterator {
      public:
        using value_type = hwcpipe_counter;
        using reference = const hwcpipe_counter &;
        using pointer = const hwcpipe_counter *;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        iterator(const counter_database_t &db, backing_iterator begin, backing_iterator end)
            : db_(db)
            , it_(std::move(begin))
            , end_(std::move(end)) {
            if (it_ != end_) {
                current_counter_ = it_->first;
            } else {
                current_counter_ = static_cast<hwcpipe_counter>(0);
            }
        }

        /**
         * @brief A convenience method that fetches the descriptive information
         * for the current counter.
         */
        counter_metadata describe_counter() {
            if (it_ == end_) {
                return {"", ""};
            }

            counter_metadata meta{};
            auto ec = db_.describe_counter(it_->first, meta);
            if (ec) {
                // should not happen - the counters for a gpu should always have
                // metatada in the database.
                assert(false && "Invalid counter - inconsistent database");
            }

            return meta;
        }

        reference operator*() const { return current_counter_; }
        pointer operator->() const { return &current_counter_; }

        iterator &operator++() {
            ++it_;
            if (it_ != end_) {
                current_counter_ = it_->first;
            } else {
                current_counter_ = static_cast<hwcpipe_counter>(0);
            }
            return *this;
        }

        iterator operator++(int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        friend bool operator==(const iterator &lhs, const iterator &rhs) { return lhs.it_ == rhs.it_; }

        friend bool operator!=(const iterator &lhs, const iterator &rhs) { return lhs.it_ != rhs.it_; }

      private:
        const counter_database_t &db_;
        backing_iterator it_;
        backing_iterator end_;

        hwcpipe_counter current_counter_;
    };

  public:
    gpu_counter_view(const counter_database_t &db, backing_map_type::const_iterator begin,
                     backing_map_type::const_iterator end)
        : db_(db)
        , begin_(std::move(begin))
        , end_(std::move(end)) {}

    auto begin() const { return iterator(db_, begin_, end_); }

    auto end() const { return iterator(db_, end_, end_); }

  private:
    const counter_database_t &db_;
    backing_map_type::const_iterator begin_;
    backing_map_type::const_iterator end_;
};

class counter_database {
  public:
    /**
     * @brief Checks to see if the database contains counter data for the
     * specified GPU ID value.
     */
    HWCP_NODISCARD bool is_gpu_known(device::product_id id) const;

    /**
     * @brief Returns an iterable view of all of the counters that are valid for
     * the specified GPU ID value.
     */
    HWCP_NODISCARD gpu_counter_view<counter_database> get_counters_for_gpu(device::product_id id) const;

    /**
     * @brief Fetches the descriptive information for a counter.
     */
    HWCP_NODISCARD std::error_code describe_counter(hwcpipe_counter counter, counter_metadata &metadata) const;

    /**
     * @brief Queries the database to find the block/offset address of a counter
     * for the specified GPU.
     */
    HWCP_NODISCARD counter_definition get_counter_def(device::product_id id, hwcpipe_counter counter,
                                                      std::error_code &ec);
};

} // namespace detail
} // namespace hwcpipe
