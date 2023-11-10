/*
 * Copyright (c) 2022-2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file
 *
 * Product id class header.
 */

#pragma once

#include <cassert>
#include <cstdint>
#include <tuple>

namespace hwcpipe {
namespace device {

/**
 * Product ID class.
 *
 * Product ID is a number that can be used to uniquely identify
 * a particular Mali GPU model.
 */
class product_id {
  public:
    /** GPU Family. */
    enum class gpu_family {
        /** Midgard GPU family. */
        midgard,
        /** Bifrost GPU Family. */
        bifrost,
        /** Valhall GPU Family. */
        valhall,
    };

    /** GPU front-end type. */
    enum class gpu_frontend {
        /** Job manager GPUs. */
        jm,
        /** Command stream front-end GPUs. */
        csf,
    };

    /** Product id version style. */
    enum class version_style {
        /** Legacy style: product id is 0x6956 */
        legacy_t60x,
        /** Legacy style: product id is an arbitrary constant < 0x1000. */
        legacy_txxx,
        /** product id is an arch and product major versions pair. */
        arch_product_major,
    };

    /**
     * Default-construct a product id.
     */
    explicit constexpr product_id()
        : product_id_{} {}
    /**
     * Construct product id from a GPU id.
     *
     * @param[in] raw_product_id Raw unmasked product id.
     */
    explicit constexpr product_id(uint32_t raw_product_id)
        : product_id_(from_raw_product_id(raw_product_id)) {}

    /**
     * Construct product id using product/arch major versions pair.
     *
     * @param[in] arch_major       Arch major version.
     * @param[in] product_major    Product major version.
     */
    explicit constexpr product_id(uint32_t arch_major, uint32_t product_major)
        : product_id_(from_versions(arch_major, product_major)) {}

    /**
     * Construct product id from the raw GPU id value.
     *
     * @param gpu_id Raw gpu id.
     * @return Product id.
     */
    static constexpr product_id from_raw_gpu_id(uint64_t gpu_id) {
        constexpr uint64_t product_id_shift{16};
        constexpr uint64_t product_id_mask{0xFFFF};

        return product_id{static_cast<uint32_t>((gpu_id >> product_id_shift) & product_id_mask)};
    }

    /** @return Version style of the product ID. */
    constexpr version_style get_version_style() const { return get_version_style(product_id_); }

    /**
     * Get product major version.
     *
     * @warning It is a programming error to call this function with a legacy version style.
     *
     * @return Product major version.
     */
    constexpr uint32_t get_product_major() const {
        assert(get_version_style() == version_style::arch_product_major);
        return product_id_ & version_mask;
    }

    /**
     * Get arch major version.
     *
     * @warning It is a programming error to call this function with a legacy version style.
     *
     * @return Arch major version.
     */
    constexpr uint32_t get_arch_major() const {
        assert(get_version_style() == version_style::arch_product_major);
        return (product_id_ >> arch_major_shift) & version_mask;
    }

    /** @return GPU family of the product ID. */
    constexpr gpu_family get_gpu_family() const {
        /** First Valhall GPU product id ever released. */
        constexpr uint32_t valhall_arch_major = 9U;

        if (get_version_style() != version_style::arch_product_major)
            return gpu_family::midgard;

        if (get_arch_major() < valhall_arch_major)
            return gpu_family::bifrost;

        return gpu_family::valhall;
    }

    /** @return job front-end type of the product. */
    constexpr gpu_frontend get_gpu_frontend() const {
        constexpr uint32_t csf_arch_major = 0xaU;

        if (get_version_style() != version_style::arch_product_major)
            return gpu_frontend::jm;

        if (get_arch_major() < csf_arch_major)
            return gpu_frontend::jm;

        return gpu_frontend::csf;
    }

    /** @return Product id as unsigned integer. */
    constexpr operator uint32_t() const { return product_id_; }

  private:
    /**
     * Get version style of a gpu_id or product_id value.
     *
     * @param[in] value    Value to get the style from.
     * @return Version style of @p value.
     */
    constexpr static version_style get_version_style(uint32_t value) {
        if (value == product_id_t60x)
            return version_style::legacy_t60x;
        else if (value < 0x1000U)
            return version_style::legacy_txxx;
        else
            return version_style::arch_product_major;
    }

    /**
     * Get product id from a raw unmasked value.
     *
     * @param[in] raw_product_id Raw unmasked product id.
     * @return Product id value.
     */
    static constexpr uint32_t from_raw_product_id(uint32_t raw_product_id) {
        switch (get_version_style(raw_product_id)) {
        case version_style::legacy_t60x:
        case version_style::legacy_txxx:
            return raw_product_id & product_id_mask_legacy;
        case version_style::arch_product_major:
        default:
            return raw_product_id & product_id_mask_modern;
        }
    }

    /**
     * Get product id using product/arch major versions pair.
     *
     * @param[in] arch_major       Arch major version.
     * @param[in] product_major    Product major version.
     * @return Product id value.
     */
    static constexpr uint32_t from_versions(uint32_t arch_major, uint32_t product_major) {
        assert((product_major & version_mask) == product_major);
        assert((arch_major & version_mask) == arch_major);

        return (arch_major << arch_major_shift) | product_major;
    }

    /** Mask to apply to GPU id to read legacy product id.*/
    static constexpr uint32_t product_id_mask_legacy{0xffffU};
    /** Mask to apply to GPU id to read modern product id.*/
    static constexpr uint32_t product_id_mask_modern{0xf00fU};
    /** t60x GPU product GPU id. */
    static constexpr uint32_t product_id_t60x{0x6956U};

    /** Arch major version shift. */
    static constexpr uint32_t arch_major_shift{12UL};
    /** Major version mask. */
    static constexpr uint32_t version_mask{0xfU};

    /** Product id number. */
    uint32_t product_id_;
};

} // namespace device
} // namespace hwcpipe
