#
# Copyright (c) 2021-2022 ARM Limited.
#
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 3.13.5)

project(hwcpipe2)

option(
    HWCPIPE_ENABLE_TESTS
    "If set, build unit tests."
    ON
)
option(HWCPIPE_ENABLE_EXCEPTIONS "If set, c++ exceptions are enabled.")
option(
    HWCPIPE_ENABLE_RTTI
    "If set, c++ rtti is enabled. Note that exceptions are required for catch2."
)
option(HWCPIPE_ENABLE_SYMBOLS_VISIBILITY "If set, all symbols are exported.")
option(
    HWCPIPE_WALL
    "Enable all warnings."
    ON
)
option(
    HWCPIPE_WERROR
    "Treat compile and link warning as error."
    ON
)

set(IS_ARM_BUILD FALSE)
if(CMAKE_SYSTEM_PROCESSOR
   MATCHES
   "^(armv7-a)|(aarch64)$"
)
    set(IS_ARM_BUILD TRUE)
endif()

include(CMakeDependentOption)
cmake_dependent_option(
    HWCPIPE_ENABLE_END_TO_END_TESTS
    "If set, build end-to-end tests."
    ${IS_ARM_BUILD}
    "HWCPIPE_ENABLE_TESTS"
    OFF
)

option(HWCPIPE_SYSCALL_LIBMALI
       "Load syscall function entries from libmali.so for testing."
)

if(CMAKE_BUILD_TYPE
   AND (NOT
        CMAKE_BUILD_TYPE
        STREQUAL
        "Debug"
       )
)
    set(IS_RELEASE_BUILD YES)
endif()

option(
    HWCPIPE_ENABLE_LTO
    "Enable link time optimization. Enabled by default for release builds."
    ${IS_RELEASE_BUILD}
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckIPOSupported)
check_ipo_supported(RESULT IS_LTO_SUPPORTED OUTPUT error)

if(IS_LTO_SUPPORTED AND HWCPIPE_ENABLE_LTO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif()

if(HWCPIPE_ENABLE_EXCEPTIONS)
    add_compile_options("-fexceptions")
else()
    add_compile_options("-fno-exceptions")
endif()

if(HWCPIPE_ENABLE_RTTI)
    add_compile_options("-frtti")
else()
    add_compile_options("-fno-rtti")
endif()

if(HWCPIPE_ENABLE_SYMBOLS_VISIBILITY)
    add_compile_options("-fvisibility=default")
else()
    add_compile_options("-fvisibility=hidden")
endif()

if(HWCPIPE_WALL)
    add_compile_options(
        -Wall
        -Wextra
        -pedantic
        -Wno-abi # Silence GCC 7.1 ABI compatibility warning
        -Wconversion
    )
endif()

if(HWCPIPE_WERROR)
    add_compile_options(
        -Werror
        -pedantic-errors
        -Wno-abi # Silence GCC 7.1 ABI compatibility warning
    )
    add_link_options(-Werror)
endif()

add_subdirectory(device)

if(HWCPIPE_ENABLE_TESTS)
    enable_testing()

    add_subdirectory(test)
    add_subdirectory(third_party/catch2)
endif()
