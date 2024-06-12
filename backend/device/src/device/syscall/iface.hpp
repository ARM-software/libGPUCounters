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
 * @file iface.hpp Syscall interface header.
 */

#pragma once

#include "funcs/libmali.hpp"
#include "funcs/unix.hpp"

#include <device/detail/is_empty_class.hpp>

#include <system_error>

#include <errno.h>
#include <poll.h>
#include <sys/stat.h>

namespace hwcpipe {
namespace device {
namespace syscall {

namespace detail {
/**
 * Wrapper around all system calls used by the library.
 *
 * The class aims to abstract away system calls details and
 * ease unit testing. For the product code, it is an empty class
 * with a set of syscall static functions. For the unit tests, one
 * MAY use a stateful class to provide a fake implementations and/or
 * track functions called.
 *
 * @par Example
 * @code
 * template<typename syscall_ifcace_t>
 * class my_class : private syscall_iface_t {
 *     template<typename... args_t>
 *     my_class(args_t&&... args) : syscall_iface_t(std::forward<args_t>(args)...) {}
 *
 *     syscall_iface_t &get_syscall_iface() { return *this; }
 *     void func() {
 *         std::tie(ec, fd) = get_syscall_iface().open("/a/b/c/d.txt", O_RDONLY);
 *     }
 * };
 * @endcode
 *
 * @par Stateful syscall interface mock example
 * @code
 * class syscall_iface_mock {
 * public:
 *     syscall_iface_mock(unsigned &open_call_count) : open_call_count_(open_call_count)
 *     std::pair<std::error_code, int> open(const char *path, int mode) {
 *         ++open_call_count_; // record that open was called
 *         assert(mode == O_RDONLY); //
 *         return std::make_pair(std::error_code{}, 42);
 *     }
 *     unsigned &open_call_count_;
 * }
 *
 * void test() {
 *    unsigned open_call_count = 0;
 *    my_class<syscall_iface_mock> obj(m, open_call_count);
 *    assert(open_call_count == 1); // if `open` was called, the counter should be updated.
 * }
 * @endcode
 */
template <typename syscall_funcs_t>
class iface {
  private:
    using syscall_funcs_type = syscall_funcs_t;

  public:
    /**
     * open wrapper function.
     *
     * @param[in] name      File name to open.
     * @param[in] oflags    File open flags.
     * @return A pair of std::error_code and the file descriptor.
     */
    static std::pair<std::error_code, int> open(const char *name, int oflags) {
        const int result = syscall_funcs_type::open(name, oflags);
        std::error_code ec;

        if (result < 0)
            ec = errno_error_code();

        return std::make_pair(ec, result);
    }

    /**
     * Check if a file descriptor corresponds to a char device.
     *
     * @param[in] fd    File descriptor to check.
     * @return A pair of std::error_code and bool which is true if @p fd is a char device.
     */
    static std::pair<std::error_code, bool> is_char_device(int fd) {
        struct stat s {};
        const int result = fstat(fd, &s);

        if (result < 0)
            return std::make_pair(errno_error_code(), false);

        return std::make_pair(std::error_code{}, S_ISCHR(s.st_mode) != 0);
    }

    /**
     * close wrapper function.
     *
     * @param[in] fd    File descriptor to close.
     * @return std::error_code instance.
     */
    static std::error_code close(int fd) {
        const int result = syscall_funcs_type::close(fd);

        std::error_code ec;

        if (result < 0)
            ec = errno_error_code();

        return ec;
    }

    /**
     * mmap wrapper function.
     *
     * @param[in] addr      Hint address where to map the memory.
     * @param[in] len       Mapping length.
     * @param[in] prot      Protection flags.
     * @param[in] flags     Mapping flags.
     * @param[in] fd        File descriptor.
     * @param[in] off       File offset.
     * @return A pair of std::error_code and the address mapped.
     */
    static std::pair<std::error_code, void *> mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
        void *const result = syscall_funcs_type::mmap(addr, len, prot, flags, fd, off);

        std::error_code ec;

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        if (MAP_FAILED == result)
            ec = errno_error_code();

        return std::make_pair(ec, result);
    }

    /**
     * munmap wrapper function.
     *
     * @param[in] addr    Address to unmap.
     * @param[in] len     Memory region length to unmap.
     * @return std::error_code instance.
     */
    static std::error_code munmap(void *addr, size_t len) {
        const int result = syscall_funcs_type::munmap(addr, len);

        if (result < 0)
            return errno_error_code();

        return {};
    }

    /**
     * ioctl wrapper function.
     *
     * @param[in]     fd      Valid file descriptor.
     * @param[in]     command I/O control command.
     * @param[in,out] args    Command arguments.
     * @return A pair of std:error_code and `ioctl` return value.
     */
    template <typename command_t, typename... args_t>
    static std::pair<std::error_code, int> ioctl(int fd, command_t command, args_t &&...args) {
        const int result = syscall_funcs_type::ioctl(fd, command, std::forward<args_t>(args)...);

        std::error_code ec;

        if (result < 0)
            ec = errno_error_code();

        return std::make_pair(ec, result);
    }

    /**
     * poll wrapper function.
     *
     * @param[in] fds         The set of file descriptors to be monitored.
     * @param[in] nfds        The number of items in the fds array in nfds.
     * @param[in] timeout     The timeout in milliseconds that poll() should block
     *                        waiting for a file descriptor to become ready.
     * @return A pair of std:error_code and `poll` return value.
     */
    static std::pair<std::error_code, int> poll(struct pollfd *fds, nfds_t nfds, int timeout) {
        const int result = ::poll(fds, nfds, timeout);

        std::error_code ec;

        if (result < 0)
            ec = errno_error_code();

        return std::make_pair(ec, result);
    }

  private:
    /** @return std::error_code created out of the errno value. */
    static std::error_code errno_error_code() { return {errno, std::generic_category()}; }
};

} // namespace detail

#if defined(HWCPIPE_SYSCALL_LIBMALI)
using iface = detail::iface<funcs::libmali>;
#else
using iface = detail::iface<funcs::unix>;
#endif

} // namespace syscall

static_assert(detail::is_empty_class<syscall::iface>::value, "syscall::iface must be an empty class.");

} // namespace device
} // namespace hwcpipe
