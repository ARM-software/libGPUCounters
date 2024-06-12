/*
 * Copyright (c) 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hwcpipe/error.hpp"
#include "hwcpipe/types.hpp"

#include <device/handle.hpp>
#include <device/instance.hpp>
#include <device/product_id.hpp>

#include <iostream>
#include <iterator>
#include <system_error>

namespace hwcpipe {

/**
 * @brief The gpu class represents a physical GPU device in the system. It can
 * be used to query various hardware features (e.g. core count).
 */
class gpu {
  public:
    /** Default constructor for an invalid device. */
    gpu()
        : device_number_(-1)
        , valid_(false) {}

    /**
     * Construct a GPU instance for a device number. Probes the device to
     * fetch its hardware features. If the requested device was not found then
     * the gpu instance will be invalid. Callers should use the valid() method,
     * or cast to bool, to check that the GPU is valid for use.
     *
     * @param [in] device_number  The device instance number (e.g. 0 for /dev/mali0)
     */
    gpu(int device_number)
        : device_number_(device_number) {
        auto handle = device::handle::create(device_number);
        if (!handle) {
            valid_ = false;
        } else {
            valid_ = true;
            fetch_device_info(*handle);
        }

        auto result = device::product_id_from_raw_gpu_id(constants_.gpu_id);

        if (result.first) {
            valid_ = false;
        }

        id_ = result.second;
    }

    HWCP_NODISCARD int get_device_number() const { return device_number_; }

    HWCP_NODISCARD uint64_t num_shader_cores() const { return constants_.num_shader_cores; }

    HWCP_NODISCARD uint64_t num_execution_engines() const { return constants_.num_exec_engines; }

    HWCP_NODISCARD uint64_t bus_width() const { return constants_.axi_bus_width; }

    HWCP_NODISCARD device::product_id get_product_id() const { return id_; }

    HWCP_NODISCARD device::gpu_family get_gpu_family() const { return device::get_gpu_family(id_); }

    HWCP_NODISCARD device::constants get_constants() const { return constants_; }

    HWCP_NODISCARD bool valid() const { return valid_; }
    operator bool() const { return valid(); }

  private:
    int device_number_;
    bool valid_{};
    device::constants constants_{};
    device::product_id id_{};

    void fetch_device_info(device::handle &handle) {
        auto instance = device::instance::create(handle);

        if (!instance) {
            valid_ = false;
            return;
        }

        constants_ = instance->get_constants();
    }
};

/**
 * @brief A class which provdides an enumerable view of the GPU devices attached
 * to the system.
 *
 * @par
 * @code
 * for (const auto & gpu : find_gpus()) {
 *     std::cout << "Found device " << gpu.get_device_number() << std::endl;
 * }
 * @endcode
 */
class find_gpus {
  private:
    /**
     * @brief An iterator which probes for GPUs.
     */
    class iterator {
      public:
        using value_type = gpu;
        using reference = gpu &;
        using pointer = gpu *;
        using difference_type = ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        /**
         * @brief Default constructor that represents the end of iteration.
         */
        iterator()
            : device_number_(max_device_number)
            , gpu_{} {}

        /**
         * @brief Constructs an iterator which starts probing from the specified
         * device number.
         */
        iterator(int start_from_device)
            : device_number_(start_from_device) {
            find_next_device();
        }

        reference operator*() { return gpu_; }
        pointer operator->() { return &gpu_; }

        iterator &operator++() {
            find_next_device();
            return *this;
        }

        iterator operator++(int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        friend bool operator==(const iterator &lhs, const iterator &rhs) {
            return lhs.device_number_ == rhs.device_number_;
        }

        friend bool operator!=(const iterator &lhs, const iterator &rhs) {
            return lhs.device_number_ != rhs.device_number_;
        }

      private:
        constexpr static int max_device_number = 32;

        int device_number_;
        gpu gpu_;

        /**
         * @brief Scan for the next valid device number by attempting to create
         * a hwcpipe instance.
         */
        void find_next_device() {
            while (device_number_ < max_device_number) {
                gpu_ = gpu(device_number_);
                device_number_++;

                if (gpu_.valid()) {
                    break;
                }
            }
        }
    };

  public:
    find_gpus() = default;

    auto begin() const { return iterator{0}; }

    auto end() const { return iterator{}; }
};

} // namespace hwcpipe

inline std::ostream &operator<<(std::ostream &os, const hwcpipe::device::gpu_family &family) {
    switch (family) {
    case hwcpipe::device::gpu_family::midgard:
        return os << "midgard";
    case hwcpipe::device::gpu_family::bifrost:
        return os << "bifrost";
    case hwcpipe::device::gpu_family::valhall:
        return os << "valhall";
    case hwcpipe::device::gpu_family::fifthgen:
        return os << "Arm 5th Gen";
    default:
        return os << "unknown";
    }
}

inline std::ostream &operator<<(std::ostream &os, const hwcpipe::device::gpu_frontend &frontend) {
    switch (frontend) {
    case hwcpipe::device::gpu_frontend::jm:
        return os << "jm";
    case hwcpipe::device::gpu_frontend::csf:
        return os << "csf";
    default:
        return os << "unknown";
    }
}