/*
 * Copyright (c) 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "device/constants.hpp"
#include "device/hwcnt/prfcnt_set.hpp"
#include "device/product_id.hpp"
#include "hwcpipe/detail/counter_database.hpp"
#include "hwcpipe/detail/internal_types.hpp"
#include "hwcpipe/error.hpp"
#include "hwcpipe/gpu.hpp"
#include "hwcpipe/hwcpipe_counter.h"
#include "hwcpipe/types.hpp"

#include <device/handle.hpp>
#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sampler/configuration.hpp>
#include <device/hwcnt/sampler/manual.hpp>

#include <cassert>
#include <cstdint>
#include <memory>
#include <set>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace hwcpipe {

/**
 * @brief a counter_sample object holds the sampled information from a counter at
 * a specific timestamp.
 */
struct counter_sample {
    /** @brief Type of the sampled data. */
    enum class type { uint64, float64 };

    /** @brief Value of the sample. */
    union value {
        uint64_t uint64;
        double float64;

        explicit value(uint64_t int_v)
            : uint64(int_v) {}

        explicit value(double double_v)
            : float64(double_v) {}
    };

    /** The counter id. */
    hwcpipe_counter counter;
    /** The timestamp of the sample. */
    uint64_t timestamp;
    /** The sample value (variant is defined by the type field). */
    value value;
    /** The data type of the sampled value. */
    type type;

    counter_sample(hwcpipe_counter counter, uint64_t timestamp, uint64_t value)
        : counter(counter)
        , timestamp(timestamp)
        , value(value)
        , type(type::uint64) {}

    counter_sample(hwcpipe_counter counter, uint64_t timestamp, double value)
        : counter(counter)
        , timestamp(timestamp)
        , value(value)
        , type(type::float64) {}

    /** Default constructor */
    counter_sample()
        : counter_sample(hwcpipe_counter(), 0, 0UL) {}
};

/**
 * @brief A sampler_config object holds the list of counters that were selected
 * by the user and builds the state that is needed by the sampler to set up the
 * GPU.
 */
class sampler_config {
  public:
    using backend_cfg_type = device::hwcnt::sampler::configuration;

    // structure to tie together the counter ID and its block address or
    // evaluator function, once they've been retrieved from the database
    struct registered_counter {
        hwcpipe_counter counter;
        detail::counter_definition definition;

        registered_counter(hwcpipe_counter counter, detail::counter_definition definition)
            : counter(counter)
            , definition(definition) {}

        registered_counter(hwcpipe_counter counter)
            : counter(counter) {}

        friend bool operator==(const registered_counter &lhs, const registered_counter &rhs) {
            return lhs.counter == rhs.counter;
        }

        friend bool operator!=(const registered_counter &lhs, const registered_counter &rhs) {
            return lhs.counter != rhs.counter;
        }

        friend bool operator<(const registered_counter &lhs, const registered_counter &rhs) {
            return lhs.counter < rhs.counter;
        }
    };

    /**
     * @brief Construct a sampler configuration for a GPU.
     */
    sampler_config(const gpu &gpu)
        : sampler_config(gpu.get_product_id(), gpu.get_device_number()) {}

    /**
     * @brief Construct a sampler configuration from the provided GPU ID and
     * device device.
     */
    sampler_config(device::product_id pid, int device_number)
        : pid_(pid)
        , device_number_(device_number) {
        constexpr auto prfcnt_set = device::hwcnt::prfcnt_set::primary;

        // preallocate the backend enable maps for each possible block type
        for (size_t i = 0; i != static_cast<size_t>(device::hwcnt::block_extents::num_block_types); ++i) {
            const auto type = static_cast<block_type>(i);
            backend_config_.emplace(type, backend_cfg_type{type, prfcnt_set, {}});
        }
    }

    /**
     * @brief Requests that a counter is collected by the sampler. The counter
     * is validated to make sure that it's supported by the GPU.
     *
     * @param [in] counter  The counter to sample.
     * @return Returns hwcpipe::errc::invalid_counter_for_device if the counter
     *                 is not supported by the current GPU.
     */
    HWCP_NODISCARD std::error_code add_counter(hwcpipe_counter counter) {
        if (counters_.find(counter) != counters_.end()) {
            return {};
        }

        std::error_code ec;
        // validate that the GPU actually has the counter
        auto definition = db_.get_counter_def(pid_, counter, ec);
        if (ec) {
            return ec;
        }

        switch (definition.tag) {
        case detail::counter_definition::type::hardware: {
            const auto &address = definition.u.address;
            counters_.emplace(counter, definition);

            backend_config_[address.block_type].enable_map[address.offset] = 1;
            break;
        }
        case detail::counter_definition::type::expression: {
            counters_.emplace(counter, definition);
            ec = add_expression_depedencies(definition.u.expression);
            if (ec) {
                return ec;
            }
            break;
        }
        case detail::counter_definition::type::invalid:
        default:
            return hwcpipe::make_error_code(hwcpipe::errc::invalid_counter_for_device);
        }

        return {};
    }

    /**
     * @brief Fetches the list of counters that have been validated and added
     * to this config.
     */
    HWCP_NODISCARD const std::set<registered_counter> &get_valid_counters() const { return counters_; }

    /**
     * @brief Constructs a list of enable maps, specific to the selected GPU,
     * that can be used by the sampler backend to enable the performance
     * counters.
     */
    HWCP_NODISCARD std::vector<backend_cfg_type> build_backend_config_list() const {
        std::vector<backend_cfg_type> config_list{};
        for (const auto &it : backend_config_) {
            if (it.second.enable_map.any()) {
                config_list.push_back(it.second);
            }
        }
        return config_list;
    }

    /**
     * @brief Returns the device instance number (e.g. 0 for /dev/mali0) for the
     * selected GPU.
     */
    HWCP_NODISCARD int get_device_number() const { return device_number_; }

  private:
    using block_type = device::hwcnt::block_type;

    device::product_id pid_;
    int device_number_;

    detail::counter_database db_{};
    std::set<registered_counter> counters_{};
    std::unordered_map<block_type, backend_cfg_type> backend_config_{};

    HWCP_NODISCARD std::error_code
    add_expression_depedencies(const detail::expression::expression_definition &expression) {
        std::error_code ec;
        for (auto dependency : expression.dependencies) {
            ec = add_counter(dependency);
            if (ec) {
                return ec;
            }
        }
        return {};
    }
};

/**
 * @brief A sampler is responsible for collecting counter samples from the GPU
 * and presenting them in a more user-friendly manner.
 */
template <typename backend_policy_t = hwcpipe::detail::hwcpipe_backend_policy>
class sampler : private detail::expression::context {
    using handle_type = typename backend_policy_t::handle_type;
    using instance_type = typename backend_policy_t::instance_type;
    using sampler_type = typename backend_policy_t::sampler_type;
    using sample_type = typename backend_policy_t::sample_type;
    using block_type = device::hwcnt::block_type;

    template <typename iterator_t>
    class sample_iterator {
      public:
        using value_type = counter_sample;
        using reference = const counter_sample &;
        using pointer = const counter_sample *;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        sample_iterator(sampler &sampler, iterator_t begin, iterator_t end)
            : sampler_(sampler)
            , it_(std::move(begin))
            , end_(std::move(end))
            , sample_() {
            if (it_ != end_) {
                set_sample_value();
            }
        }

        reference operator*() { return sample_; }
        pointer operator->() { return &sample_; }

        auto &operator++() {
            ++it_;
            if (it_ != end_) {
                set_sample_value();
            }
            return *this;
        }

        auto operator++(int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        friend bool operator==(const sample_iterator<iterator_t> &lhs, const sample_iterator<iterator_t> &rhs) {
            return lhs.it_ == rhs.it_;
        }

        friend bool operator!=(const sample_iterator<iterator_t> &lhs, const sample_iterator<iterator_t> &rhs) {
            return lhs.it_ != rhs.it_;
        }

      private:
        sampler &sampler_;
        iterator_t it_;
        iterator_t end_;

        counter_sample sample_;

        void set_sample_value() { auto ec = sampler_.get_counter_value(it_->first, sample_); }
    };

    template <typename iterator_t>
    class sample_iterable {
      public:
        sample_iterable(sampler &sampler, iterator_t begin, iterator_t end)
            : sampler_(sampler)
            , begin_(std::move(begin))
            , end_(std::move(end)) {}

        auto begin() { return sample_iterator<iterator_t>(sampler_, begin_, end_); }

        auto end() { return sample_iterator<iterator_t>(sampler_, end_, end_); }

      private:
        sampler &sampler_{};
        iterator_t begin_;
        iterator_t end_;
    };

  public:
    /**
     * Constructs a sampler from the specified config object. This will attempt
     * to create the relevant resources in the kernel driver. If this fails then
     * the resulting sampler instance will be invalid and any attempts to start
     * sampling will fail with an error_code.
     *
     * The caller must check that the resulting instance is valid before trying
     * to use it.
     * @par
     * @code
     * auto sampler = hwcpipe::sampler(config);
     * if (!sampler) {
     *    // sampler creation failed
     * }
     * @endcode
     */
    sampler(const sampler_config &config) {
        // try to open the backend instance
        handle_ = handle_type::create(config.get_device_number());
        if (!handle_) {
            ec_ = make_error_code(errc::backend_creation_failed);
            return;
        }

        instance_ = instance_type::create(*handle_);
        if (!instance_) {
            ec_ = make_error_code(errc::backend_creation_failed);
            return;
        }

        constants_ = instance_->get_constants();

        // if we're dealing with a GPU >= G715/G615 then counters are 64bit
        values_are_64bit_ =
            instance_->get_hwcnt_block_extents().values_type() == device::hwcnt::sample_values_type::uint64;

        // reserve the sample list buffer and map counters to posisions within
        // the buffer
        const auto &valid_counters = config.get_valid_counters();
        if (valid_counters.empty()) {
            ec_ = make_error_code(errc::sampler_config_invalid);
            return;
        }
        build_sample_buffer_mappings(valid_counters);

        // finally, try to create the backend sampler using the config array
        auto config_array = config.build_backend_config_list();
        auto sampler = std::make_unique<sampler_type>(*instance_, config_array.data(), config_array.size());

        if (!sampler || !(*sampler)) {
            ec_ = hwcpipe::make_error_code(hwcpipe::errc::backend_sampler_failure);
            return;
        }

        sampler_ = std::move(sampler);
    }

    operator bool() const { return !ec_; }

    /**
     * @brief Starts counter accumulation.
     *
     * @return Returns an error if the sampler backend could not be started or
     * if sampling was already in progress, otherwise returns a default
     * constructed error_code.
     */
    HWCP_NODISCARD std::error_code start_sampling() {
        if (ec_) {
            return ec_;
        }
        if (sampling_in_progress_) {
            return make_error_code(errc::sampling_already_started);
        }
        auto ec = sampler_->accumulation_start();
        if (ec) {
            return make_error_code(errc::accumulation_start_failed);
        }
        sampling_in_progress_ = true;
        return {};
    }

    /**
     * @brief Requests that counter accumulation is stopped. If the request
     * fails an error is returned, the sampler should be considered to be in an
     * undefined state and should not be used further.
     *
     * @return A default constructed error_code on success, or an error if the
     * request failed.
     */
    HWCP_NODISCARD std::error_code stop_sampling() {
        if (ec_) {
            return ec_;
        }
        if (!sampling_in_progress_) {
            return make_error_code(errc::sampling_not_started);
        }
        auto ec = sampler_->accumulation_stop(0);
        if (ec) {
            return make_error_code(errc::accumulation_stop_failed);
        }
        sampling_in_progress_ = false;
        return {};
    }

    /**
     * @brief Updates the sample buffer with the most recent counter values.
     * The buffer can then be queried via get_counter_value(). If an error
     * occurs while trying to read the counters then the contents of the sample
     * buffer are unchanged and further queries will return the old values.
     *
     * @return An error if sampling has not been started, or if an error
     * occurred while reading counters from the GPU.
     */
    HWCP_NODISCARD std::error_code sample_now() {
        if (!sampling_in_progress_) {
            return make_error_code(errc::sampling_not_started);
        }

        auto ec = sampler_->request_sample(0);
        if (ec) {
            return make_error_code(errc::sample_collection_failure);
        }

        auto &reader = sampler_->get_reader();
        auto backend_sample = sample_type(reader, ec);
        if (ec) {
            return make_error_code(errc::sample_collection_failure);
        }

        // if there was an error fetching the samples then there's no point
        // trying to read the block data. Leave the last captured values in the
        // buffer.
        const auto &metadata = backend_sample.get_metadata();
        if (metadata.flags.error || metadata.flags.stretched) {
            valid_sample_buffer_ = false;
            return make_error_code(errc::sample_collection_failure);
        }

        last_collection_timestamp_ = metadata.timestamp_ns_begin;

        // clear out any samples from the previous poll
        for (auto &sample : sample_buffer_) {
            sample = 0;
        }

        // loop over the counter blocks returned by the reader and fetch any
        // samples that were requested
        if (values_are_64bit_) {
            fill_sample_buffer<uint64_t>(backend_sample);
        } else {
            fill_sample_buffer<uint32_t>(backend_sample);
        }

        valid_sample_buffer_ = true;
        return {};
    }

    /**
     * @brief Fetches the last sampled value for a counter.
     *
     * @param [in]  counter  The counter to read.
     * @param [out] sample   Populated with data from the counter.
     * @return Returns hwcpipe::errc::unknown_counter if the counter was not
     * configured for sampling, otherwise an empty error_code.
     */
    HWCP_NODISCARD std::error_code get_counter_value(hwcpipe_counter counter, counter_sample &sample) const {
        // if the data wasn't captured during the last poll flag an error for
        // the caller
        if (!valid_sample_buffer_) {
            return make_error_code(errc::sample_collection_failure);
        }

        auto expression_eval = counter_to_evaluator_.find(counter);
        if (expression_eval != counter_to_evaluator_.end()) {
            return get_expression_counter_value(counter, sample, expression_eval->second);
        }

        auto it = counter_to_buffer_pos_.find(counter);
        if (it == counter_to_buffer_pos_.end()) {
            return make_error_code(errc::unknown_counter);
        }

        sample = counter_sample(counter, last_collection_timestamp_, get_hardware_counter_value(it));
        return {};
    }

    /**
     * @brief Returns an iterable view of the counter_sample data collected in
     * the sample buffer.
     * @par
     * @code
     * hwcpipe::sampler sampler(config);
     * auto ec = sampler.start_sampling();
     * while (running) {
     *     // ... workload goes here ...
     *     ec = sampler.sample_now();
     *     if (!ec) {
     *         for (const counter_sample &sample : sampler.sample_view()) {
     *             std::cout << '[' << sample.counter_name << "] - " << sample.value << std::endl
     *         }
     *     }
     * }
     * @endcode
     *
     * @return An iterable object (provides begin()/end()) of counter_samples.
     */
    HWCP_NODISCARD auto sample_view() {
        using type = counter_to_buffer_pos_map_type::iterator;
        return sample_iterable<type>(*this, counter_to_buffer_pos_.begin(), counter_to_buffer_pos_.end());
    }

  private:
    /* holds the mapping from a block offset to a position in the sample buffer */
    struct offset_to_buffer_pos {
        size_t block_offset;
        size_t buffer_pos;
        size_t shift;

        offset_to_buffer_pos(size_t block_offset, size_t buffer_pos, size_t shift)
            : block_offset(block_offset)
            , buffer_pos(buffer_pos)
            , shift(shift) {}
    };

    // backend type aliases
    using handle_ptr_type = typename handle_type::handle_ptr;
    using instance_ptr_type = typename instance_type::instance_ptr;
    using sampler_ptr_type = std::unique_ptr<typename backend_policy_t::sampler_type>;

    // mapping types for counters & buffer positions
    using counter_to_buffer_pos_map_type = std::unordered_map<hwcpipe_counter, size_t>;
    using counters_by_block_map_type = std::unordered_map<block_type, std::vector<offset_to_buffer_pos>>;
    using counter_to_evaluator_type = std::unordered_map<hwcpipe_counter, detail::expression::evaluator>;

    std::error_code ec_;

    // handles to the hwcpipe backend
    handle_ptr_type handle_;
    instance_ptr_type instance_;
    sampler_ptr_type sampler_;
    device::constants constants_;

    // sample buffer and index mappings
    counter_to_buffer_pos_map_type counter_to_buffer_pos_{};
    counters_by_block_map_type counters_by_block_map_{};
    std::vector<uint64_t> sample_buffer_{};
    bool valid_sample_buffer_ = false;
    counter_to_evaluator_type counter_to_evaluator_{};

    // sampler state
    bool values_are_64bit_{};
    uint64_t last_collection_timestamp_{};
    bool sampling_in_progress_{};

    HWCP_NODISCARD uint64_t get_hardware_counter_value(counter_to_buffer_pos_map_type::const_iterator it) const {
        return static_cast<double>(sample_buffer_[it->second]);
    }

    HWCP_NODISCARD std::error_code get_expression_counter_value(hwcpipe_counter counter, counter_sample &sample,
                                                                detail::expression::evaluator eval) const {
        sample = counter_sample(counter, last_collection_timestamp_, eval(*this));
        return {};
    }

    HWCP_NODISCARD double get_counter_value(hwcpipe_counter counter) const override {
        auto it = counter_to_buffer_pos_.find(counter);
        assert(it != counter_to_buffer_pos_.end());
        return static_cast<double>(get_hardware_counter_value(it));
    }

    HWCP_NODISCARD double get_mali_config_ext_bus_byte_size() const override {
        return static_cast<double>(constants_.axi_bus_width) / 8.0;
    }

    HWCP_NODISCARD double get_mali_config_shader_core_count() const override {
        return static_cast<double>(constants_.num_shader_cores);
    }

    HWCP_NODISCARD double get_mali_config_l2_cache_count() const override {
        return static_cast<double>(constants_.l2_slice_size) * static_cast<double>(constants_.num_l2_slices);
    }

    /**
     * Reserves memory for the samples and sets up the various mappings that are
     * needed to convert between counter names & positions in the buffer.
     */
    void build_sample_buffer_mappings(const std::set<sampler_config::registered_counter> &counters) {
        // reserve space
        const auto num_counters = counters.size();
        counter_to_buffer_pos_.reserve(num_counters);
        sample_buffer_.reserve(num_counters);

        // set up the index needed by the sample writer to convert from
        // block/offset to sample buffer position
        for (size_t i = 0; i != static_cast<size_t>(device::hwcnt::block_extents::num_block_types); ++i) {
            counters_by_block_map_[static_cast<block_type>(i)] = {};
        }

        for (const auto &counter : counters) {
            auto buffer_pos = sample_buffer_.size();

            switch (counter.definition.tag) {
            case detail::counter_definition::type::hardware: {
                // build the index needed when the caller reads a counter by name
                counter_to_buffer_pos_[counter.counter] = buffer_pos;
                sample_buffer_.push_back({});

                auto &block_pos_list = counters_by_block_map_[counter.definition.u.address.block_type];
                block_pos_list.emplace_back(counter.definition.u.address.offset, buffer_pos,
                                            counter.definition.u.address.shift);
                break;
            }
            case detail::counter_definition::type::expression: {
                // store the function pointer to run this expression in get_counter_value()
                counter_to_evaluator_.emplace(counter.counter, counter.definition.u.expression.eval);
                break;
            }
            case detail::counter_definition::type::invalid:
            default:
                // ignore (unreachable)
                break;
            }
        }
    }

    /**
     * Reads samples from the kinstr/vinstr reader and collects them into the
     * sample buffer. Templated because the void* we get from the reader is
     * either uint32_t* or uint64_t*, depending on the GPU.
     */
    template <typename values_type_t>
    void fill_sample_buffer(sample_type &backend_sample) {
        // loop over the counter blocks returned by the reader and fetch any
        // samples that were requested
        for (auto &block : backend_sample.blocks()) {
            const auto &counters_in_block = counters_by_block_map_[block.type];
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            const auto *block_buffer = reinterpret_cast<const values_type_t *>(block.values);

            // collect any counters that the user requested from this block
            for (const auto &mapping : counters_in_block) {
                auto &sample = sample_buffer_[mapping.buffer_pos];
                sample += block_buffer[mapping.block_offset] << mapping.shift;
            }
        }
    }
};

} // namespace hwcpipe
