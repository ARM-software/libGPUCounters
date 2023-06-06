# Copyright (C) 2023 by Arm Limited.
#
# SPDX-License-Identifier: MIT
#

set(ANDROID_TOOLCHAIN_NAME "aarch64-linux-android-clang-" CACHE STRING "" FORCE)
set(ANDROID_ABI "arm64-v8a" CACHE STRING "" FORCE)

include("${CMAKE_CURRENT_LIST_DIR}/lib/android.toolchain.cmake")