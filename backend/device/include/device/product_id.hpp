/*
 * Copyright (c) 2022-2024 Arm Limited.
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
 * Functions to obtain a GPU product ID and query product information.
 */

#pragma once

#include <cstdint>
#include <system_error>
#include <utility>

namespace hwcpipe {
namespace device {

/**
 * List of all GPU product IDs
 */
enum class product_id {
    /* Midgard */
    t60x,
    t62x,
    t720,
    t760,
    t820,
    t830,
    t860,
    t880,
    /* Bifrost */
    g31,
    g51,
    g52,
    g71,
    g72,
    g76,
    /* Valhall */
    g57,
    g57_2,
    g68,
    g77,
    g78,
    g78ae,
    g310,
    g510,
    g610,
    g615,
    g710,
    g715,
    /* 5th Gen */
    g720,
    g620,
    g725,
    g625,
};

/** GPU Family. */
enum class gpu_family {
    /** Midgard GPU family. */
    midgard,
    /** Bifrost GPU Family. */
    bifrost,
    /** Valhall GPU Family. */
    valhall,
    /** 5th Generation GPU Family */
    fifthgen,
};

/** GPU front-end type. */
enum class gpu_frontend {
    /** Job manager GPUs. */
    jm,
    /** Command stream front-end GPUs. */
    csf,
};

/**
 * Get the product ID from a raw GPU ID.
 *
 * @param[in] raw_gpu_id Raw GPU ID as an integer value.
 * @return A pair of error code and product ID.
 */
std::pair<std::error_code, product_id> product_id_from_raw_gpu_id(uint64_t raw_gpu_id);

/**
 * Get the GPU family to which a product belongs.
 *
 * @param[in] pid  Product ID.
 * @return GPU family of the product.
 */
gpu_family get_gpu_family(product_id pid);

/**
 * Get the type of job front-end for a product.
 *
 * @param[in] pid  Product ID.
 * @return job front-end type of the product.
 */
gpu_frontend get_gpu_frontend(product_id pid);

} // namespace device
} // namespace hwcpipe
