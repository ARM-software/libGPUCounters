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
 * Mali Device Driver handle header.
 */

#pragma once

#include <device/api.hpp>

#include <memory>

namespace hwcpipe {

/** Device library namespace */
namespace device {
/**
 * @defgroup device Mali Device Driver
 *
 * The device library interfaces with Mali kernel driver to query:
 *  - Device constants
 *  - Hardware counters
 */

/**
 * Mali device driver handle.
 *
 * The device handle is the main entry point to the device library API.
 * The class controls the lifetime of a Mali device driver file descriptor.
 *
 * The file descriptor is either created internally by calling `open`
 * or provided externally. While opened descriptors are closed automatically
 * at the destruction time, the external ones will remain open.
 *
 * @par Example
 * @code
 * namespace dev = hwcpipe::device;
 * // Create a handle for /dev/mali
 * auto hdnl = dev::handle::create();
 * @endcode
 *
 * To call device functions one should create an @ref instance.
 */
class HWCPIPE_DEVICE_API handle {
  protected:
    /** Default constructor. */
    handle() = default;

  public:
    /** Handle pointer. */
    using handle_ptr = std::unique_ptr<handle>;

    /** Destructor. */
    virtual ~handle();

    /**
     * Create handle using the default device path.
     *
     * Opens `/dev/mali${instance_number}` device.
     * The device descriptor is owned by the handle and will be
     * closed at the destruction time.
     *
     * @param[in] instance_number    The device instance number.
     * @return Pointer to the handle created, `nullptr` if failed.
     */
    static handle_ptr create(uint32_t instance_number = 0);

    /**
     * Create handle using full path to the device.
     *
     * The device descriptor is owned by the handle and will be
     * closed at the destruction time.
     *
     * @param[in] device_path    Path to the device file to open.
     * @return Pointer to the handle created, `nullptr` if failed.
     */
    static handle_ptr create(const char *device_path);

    /**
     * Create handle using external device file descriptor.
     *
     * The descriptor is _not_ owned by the handle, and therefore,
     * will _not_ be closed at the destruction time. The descriptor
     * MUST be valid. It is a programming error to pass an invalid
     * descriptor.
     *
     * @param[in] fd    Valid device file descriptor.
     * @return Pointer to the handle created, `nullptr` if failed.
     */
    static handle_ptr from_external_fd(int fd);
};

} // namespace device
} // namespace hwcpipe
