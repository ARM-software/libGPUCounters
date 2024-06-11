#
# Copyright (c) 2021-2024 ARM Limited.
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

add_library(
    device STATIC
    src/device/handle.cpp
    src/device/hwcnt/backend_type.cpp
    src/device/hwcnt/reader.cpp
    src/device/hwcnt/sampler/detail/backend.cpp
    src/device/hwcnt/sampler/kinstr_prfcnt/block_index_remap.cpp
    src/device/hwcnt/sampler/kinstr_prfcnt/enum_info_parser.cpp
    src/device/hwcnt/sampler/kinstr_prfcnt/metadata_parser.cpp
    src/device/instance.cpp
    src/device/num_exec_engines.cpp
    src/device/product_id.cpp
)

target_include_directories(device PUBLIC "include")

target_compile_definitions(device PRIVATE -DHWCPIPE_DEVICE_BUILDING=1)

if(HWCPIPE_SYSCALL_LIBMALI)
    target_compile_definitions(device PUBLIC -DHWCPIPE_SYSCALL_LIBMALI=1)
    target_link_libraries(device PUBLIC ${CMAKE_DL_LIBS})
endif()

add_library(device_private INTERFACE)
target_include_directories(device_private INTERFACE "src")

target_link_libraries(
    device
    PRIVATE device_private
)

if(HWCPIPE_ENABLE_TESTS)
    add_subdirectory(test)
endif()
