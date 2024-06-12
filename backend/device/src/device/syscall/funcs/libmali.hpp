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
 * @file libmali.hpp libmali system call functions.
 */

#pragma once

#include <cstdio>
#include <exception>

#include <dlfcn.h>

namespace hwcpipe {
namespace device {
namespace syscall {
namespace funcs {

/** libmali syscall functions class. */
class libmali {
    /** Helper class for loading fake system call symbols from libmali.so */
    class loader {
      private:
        loader() {
            handle_ = dlopen("libmali.so", RTLD_NOW);

            if (!handle_) {
                const char *error_str = dlerror();
                error_str = error_str != nullptr ? error_str : "<unknown>";

                fprintf(stderr, "Failed to load libmali.so. Error: %s\n", error_str);
                std::terminate();
            }

            open = load_sym<open_type>("mali_open");
            close = load_sym<close_type>("mali_close");
            ioctl = load_sym<ioctl_type>("mali_ioctl");
            mmap = load_sym<mmap_type>("mali_mmap");
            munmap = load_sym<munmap_type>("mali_munmap");
        }
        ~loader() { dlclose(handle_); }

      public:
        /** @return The library instance. */
        static loader &instance() {
            static loader lib;
            return lib;
        }

        /** Open function type. */
        using open_type = int (*)(const char *file_name, int mode);
        /** Close function type. */
        using close_type = int (*)(int fd);
        /** Ioctl function type. */
        using ioctl_type = int (*)(int fd, int request, ...);
        /** Mmap function type. */
        using mmap_type = void *(*)(void *addr, size_t len, int prot, int flags, int fd, off_t off);
        /** Munmap function type. */
        using munmap_type = int (*)(void *addr, size_t len);

        /** Open function from the library. */
        open_type open;
        /** Close function from the library. */
        close_type close;
        /** Ioctl function from the library. */
        ioctl_type ioctl;
        /** Mmap function from the library. */
        mmap_type mmap;
        /** Munmap function from the library. */
        munmap_type munmap;

      private:
        template <typename func_t>
        func_t load_sym(const char *funcname) {
            void *sym = dlsym(handle_, funcname);
            if (!sym) {
                const char *error_str = dlerror();
                error_str = error_str != nullptr ? error_str : "<unknown>";

                fprintf(stderr, "Failed to load %s from libmali.so. Error: %s\n", funcname, error_str);
                std::terminate();
            }
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<func_t>(sym);
        }

        /** Library handle type. */
        using lib_handle_type = void *;
        /** Library handle. */
        lib_handle_type handle_;
    };

  public:
    template <typename... args_t>
    static auto open(args_t &&...args) {
        return loader::instance().open(std::forward<args_t>(args)...);
    }

    template <typename... args_t>
    static auto close(args_t &&...args) {
        return loader::instance().close(std::forward<args_t>(args)...);
    }

    template <typename request_t, typename... args_t>
    static auto ioctl(int fildes, request_t request, args_t &&...args) {
        return loader::instance().ioctl(fildes, static_cast<int>(request), std::forward<args_t>(args)...);
    }

    template <typename... args_t>
    static auto mmap(args_t &&...args) {
        return loader::instance().mmap(std::forward<args_t>(args)...);
    }

    template <typename... args_t>
    static auto munmap(args_t &&...args) {
        return loader::instance().munmap(std::forward<args_t>(args)...);
    }
};

} // namespace funcs
} // namespace syscall
} // namespace device
} // namespace hwcpipe
