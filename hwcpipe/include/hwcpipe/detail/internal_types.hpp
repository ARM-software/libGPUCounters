/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hwcpipe/hwcpipe_counter.h"
#include "hwcpipe/types.hpp"

#include <device/handle.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sample.hpp>
#include <device/hwcnt/sampler/manual.hpp>
#include <device/instance.hpp>

#include <cstdint>
#include <initializer_list>
#include <system_error>
namespace hwcpipe {
namespace detail {

namespace expression {
/**
 * The expression evaluation context provides an abstraction over some block of
 * hardware counter storage. It breaks an otherwise cyclic dependency between
 * the sampler class & the expression evaluator function type.
 */
class context {
  public:
    /**
     * @brief Returns the value a hardware counter from the sampler to be used
     * in the expression evaluator
     *
     * @param counter Hardware counter to get the value for.
     * @return the counter value
     */
    HWCP_NODISCARD virtual double get_counter_value(hwcpipe_counter counter) const = 0;

    /**
     * @brief Returns the AXI bus width in bytes
     */
    HWCP_NODISCARD virtual double get_mali_config_ext_bus_byte_size() const = 0;

    /**
     * @brief Returns the number of shader core (constant) to be used
     * in evaluator function
     */
    HWCP_NODISCARD virtual double get_mali_config_shader_core_count() const = 0;

    /**
     * @brief
     */
    HWCP_NODISCARD virtual double get_mali_config_l2_cache_count() const = 0;
};

// Signature for generated evaluation functions.
using evaluator = double (*)(const context &);
/**
 * Holds information about the expression that the sampler will need when
 * registering the counters and evaluating.
 */
struct expression_definition {
    /**
     * Pointer to the function that will evaluate the expression and return the
     * calculated result.
     */
    evaluator eval;
    /**
     * The list of counters that this expression depends on. These will need to
     * be implicitly registered with the sampler so that they can be collected
     * when it is polled.
     */
    const std::initializer_list<hwcpipe_counter> dependencies;
};

} // namespace expression
/**
 * Structure representing the block type/offset address and shift
 * scaling of a counter within a particular GPU's PMU data.
 */
struct block_offset {
    uint32_t offset;
    uint32_t shift;
    hwcpipe::device::hwcnt::block_type block_type;
};

/**
 * Counters can either be raw hardware counters, or they are expressions based
 * on some combination of hardware counters, constants or other expressions.
 *
 * For hardware counters, we need to know their block/offset addresses so that
 * we can read them as we iterate over the block types. For expression counters
 * we need to call the function that will evaluate the formula.
 */
struct counter_definition {
    enum class type { invalid, hardware, expression };
    union u {
        block_offset address{};
        expression::expression_definition expression;
        explicit u(expression::expression_definition expression)
            : expression(expression) {}
        explicit u(block_offset address)
            : address(address) {}
    } u;
    type tag;

    counter_definition()
        : tag(type::invalid)
        , u(block_offset{0, 0, device::hwcnt::block_type::fe}) {}
    explicit counter_definition(expression::expression_definition expression)
        : tag(type::expression)
        , u(expression) {}
    explicit counter_definition(block_offset address)
        : tag(type::hardware)
        , u(address) {}
};

struct hwcpipe_backend_policy {
    using handle_type = device::handle;
    using instance_type = device::instance;
    using sampler_type = device::hwcnt::sampler::manual;
    using sample_type = device::hwcnt::sample;
};

/**
 * This class should **only** be used in the derived expressions (derived_functions.cpp).
 *
 * Used to override the result of 0.0 / 0.0 to return 0.0. Operators for other types included
 * to resolve ambiguity.
 */
class hwcpipe_double {
  public:
    double value;

    hwcpipe_double(double value)
        : value(value) {}

    hwcpipe_double operator+(const hwcpipe_double &other) const { return hwcpipe_double(value + other.value); }

    hwcpipe_double operator-(const hwcpipe_double &other) const { return hwcpipe_double(value - other.value); }

    hwcpipe_double operator*(const hwcpipe_double &other) const { return hwcpipe_double(value * other.value); }

    hwcpipe_double operator/(const hwcpipe_double &other) const {
        if (value == 0.0 && other.value == 0.0) {
            return hwcpipe_double(0.0);
        }
        return hwcpipe_double(value / other.value);
    }

    hwcpipe_double operator+(const int &other) const { return hwcpipe_double(value + other); }

    hwcpipe_double operator-(const int &other) const { return hwcpipe_double(value - other); }

    hwcpipe_double operator*(const int &other) const { return hwcpipe_double(value * other); }

    hwcpipe_double operator/(const int &other) const {
        if (value == 0.0 && other == 0) {
            return hwcpipe_double(0.0);
        }
        return hwcpipe_double(value / other);
    }

    hwcpipe_double operator+(const double &other) const { return hwcpipe_double(value + other); }

    hwcpipe_double operator-(const double &other) const { return hwcpipe_double(value - other); }

    hwcpipe_double operator*(const double &other) const { return hwcpipe_double(value * other); }

    hwcpipe_double operator/(const double &other) const {
        if (value == 0.0 && other == 0.0) {
            return hwcpipe_double(0.0);
        }
        return hwcpipe_double(value / other);
    }

    bool operator<(const hwcpipe_double &other) const { return value < other.value; }

    bool operator<=(const hwcpipe_double &other) const { return value <= other.value; }

    bool operator>(const hwcpipe_double &other) const { return value > other.value; }

    bool operator>=(const hwcpipe_double &other) const { return value >= other.value; }

    bool operator==(const hwcpipe_double &other) const { return value == other.value; }

    operator double() const { return value; }
};

} // namespace detail
} // namespace hwcpipe
