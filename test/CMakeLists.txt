#
# Copyright (c) 2023-2025 Arm Limited.
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.13.5)

# define the Catch2 entrypoint so that we don't have to recompile the implementation for each test group
add_library(catch-main STATIC main.cpp)
target_link_libraries(catch-main PUBLIC catch2)

function(add_test_target)
    cmake_parse_arguments(ADD_TEST_TARGET "" "TARGET" "SOURCES;LIBRARIES" ${ARGN})
    string(REPLACE ";" " " SOURCE_STR "${ADD_TEST_TARGET_SOURCES}")
    add_executable(${ADD_TEST_TARGET_TARGET} ${SOURCE_STR})

    string(REPLACE ";" " " LIBS_STR "${ADD_TEST_TARGET_LIBRARIES}")
    target_link_libraries(${ADD_TEST_TARGET_TARGET} ${LIBS_STR} hwcpipe catch-main)

    target_compile_options(${ADD_TEST_TARGET_TARGET}
        PRIVATE
        -Werror
        -Wswitch-default
        -Wswitch-enum
        -fsanitize=address
        -fsanitize=leak
    )

    # Enable AddressSanitizer and LeakSanitizer at link time
    target_link_options(${ADD_TEST_TARGET_TARGET} PRIVATE
        -fsanitize=address
        -fsanitize=leak
    )

    catch_discover_tests(${ADD_TEST_TARGET_TARGET}
        REPORTER junit
        OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
        OUTPUT_SUFFIX ".result.xml"
    )
endfunction()

add_test_target(TARGET counter-enumeration-test
    SOURCES hwcpipe/counter_enumeration.cpp
)

add_test_target(TARGET counter-sampler-test
    SOURCES counter-sampler.cpp
)

add_test_target(TARGET gpu-instance-test
    SOURCES hwcpipe/gpu_instance.cpp
)

add_test_target(TARGET hwcpipe-double-test
    SOURCES hwcpipe/hwcpipe_double.cpp
)

