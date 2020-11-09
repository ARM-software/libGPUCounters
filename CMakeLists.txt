# Copyright (c) 2019, Arm Limited and Contributors
#
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge,
# to any person obtaining a copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

cmake_minimum_required(VERSION 3.6)

project(hwcpipe LANGUAGES C CXX)

set(PROJECT_FILES
    hwcpipe.h
    cpu_profiler.h
    gpu_profiler.h
    value.h

    hwcpipe.cpp)

if(UNIX AND NOT APPLE)
    list(APPEND PROJECT_FILES
        vendor/arm/mali/hwc.hpp
        vendor/arm/mali/hwc_names.hpp
        vendor/arm/mali/mali_profiler.h
        vendor/arm/mali/mali_profiler.cpp)

    list(APPEND PROJECT_FILES
        vendor/arm/pmu/pmu_counter.h
        vendor/arm/pmu/pmu_profiler.h
        vendor/arm/pmu/pmu_counter.cpp
        vendor/arm/pmu/pmu_profiler.cpp)
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

source_group("\\" FILES ${PROJECT_FILES})

add_library(${PROJECT_NAME} SHARED ${PROJECT_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC third_party)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
