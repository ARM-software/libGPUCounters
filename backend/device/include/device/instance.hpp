/*
 * Copyright (c) 2021-2023 Arm Limited.
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
 * Mali device driver instance header.
 */

#pragma once

#include <device/api.hpp>
#include <device/constants.hpp>
#include <device/hwcnt/block_extents.hpp>

#include <memory>

namespace hwcpipe {
namespace device {

class handle;

/**
 * Mali device driver instance.
 *
 * @par Example
 * @code
 * namespace dev = hwcpipe::device;
 * // Create an instance for /dev/mali
 * auto hndl = dev::handle::create();
 * // Create a device to serve this instance.
 * auto instance = dev::instance::create(*hndl);
 * // Print GPU id
 * const dev::constants c = instance->get_constants();
 * printf("GPU id = %lu\n", c.gpu_id);
 * @endcode
 */
class HWCPIPE_DEVICE_API instance {
  public:
    /** Instance pointer type. */
    using instance_ptr = std::unique_ptr<instance>;

    /** Destructor */
    virtual ~instance();

    /**
     * Get GPU device constants.
     *
     * @return GPU device constants.
     */
    virtual constants get_constants() const = 0;

    /**
     * Get hardware counters block's extents.
     *
     * @return The block_extents structure instance.
     */
    virtual hwcnt::block_extents get_hwcnt_block_extents() const = 0;

    /**
     * Create device instance.
     *
     * The @p hndl object must outlive the instance created.
     *
     * @param[in] hndl    Device handle.
     * @return The device instance created, nullptr if failed.
     */
    static instance_ptr create(handle &hndl);
};

} // namespace device
} // namespace hwcpipe
