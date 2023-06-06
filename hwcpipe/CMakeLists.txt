#
# Copyright (c) 2023 Arm Limited.
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.13.5)

add_library(hwcpipe
    src/error.cpp
    src/hwcpipe/detail/counter_database.cpp
    src/hwcpipe/all_gpu_counters.cpp
    src/hwcpipe/counter_metadata.cpp
)

if(HWCPIPE_PIC)
    set_target_properties(hwcpipe PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_include_directories(hwcpipe
    PUBLIC include
    PRIVATE src
)

target_link_libraries(hwcpipe
    PUBLIC device
)