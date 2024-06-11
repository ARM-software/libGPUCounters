#
# Copyright (c) 2021-2023 Arm Limited.
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.13.5)

project(hwcpipe VERSION 2.3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
option(
    HWCPIPE_PIC
    "Generate position independent code."
    ON
)
# Hwcpipe is designed for integration into game engines so RTTI and exceptions should not be used in this library.
# They are, however, useful for unit tests so the compiler flags may be conditionally enabled for that purpose.
option(
    HWCPIPE_ENABLE_RTTI
    "If set, C++ RTTI is enabled. Note that exceptions are required for Catch2."
    OFF
)
option(
    HWCPIPE_ENABLE_EXCEPTIONS
    "If set, C++ exceptions are enabled."
    OFF
)
option(
    HWCPIPE_FRONTEND_ENABLE_TESTS
    "Build the hwcpipe unit & integration tests. Note that RTTI and exceptions are required to build the tests."
    OFF
)
option(
    HWCPIPE_BUILD_EXAMPLES
    "Build the example programs."
    OFF
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

if (HWCPIPE_ENABLE_EXCEPTIONS)
    add_compile_options(-fexceptions)
else()
    add_compile_options(-fno-exceptions)
endif()

if (HWCPIPE_ENABLE_RTTI)
    add_compile_options(-frtti)
else()
    add_compile_options(-fno-rtti)
endif()

if(HWCPIPE_WERROR)
    add_compile_options(
        -Werror
        -pedantic-errors
        -Wno-abi # Silence GCC 7.1 ABI compatibility warning
    )
    add_link_options(-Werror)
endif()

add_subdirectory(backend/device)

if(HWCPIPE_PIC)
    set_target_properties(device PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

add_subdirectory(hwcpipe)

if(HWCPIPE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(HWCPIPE_FRONTEND_ENABLE_TESTS)
    if (NOT HWCPIPE_ENABLE_EXCEPTIONS)
        message(FATAL_ERROR "HWCPIPE_FRONTEND_ENABLE_TESTS=ON requires HWCPIPE_ENABLE_EXCEPTIONS=ON.")
    endif()
    if (NOT HWCPIPE_ENABLE_RTTI)
        message(FATAL_ERROR "HWCPIPE_FRONTEND_ENABLE_TESTS=ON requires HWCPIPE_ENABLE_RTTI=ON.")
    endif()

    enable_testing()
    include(CTest)
    include(cmake/Catch.cmake)

    add_compile_definitions(HWCPIPE_TEST=1)

    add_subdirectory(test)
    add_subdirectory(third_party/catch2)
endif()

