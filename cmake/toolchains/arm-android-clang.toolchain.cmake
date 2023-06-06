# Copyright (C) 2023 by Arm Limited.
#
# SPDX-License-Identifier: MIT
#

set(ANDROID_TOOLCHAIN_NAME "arm-linux-androideabi-clang-" CACHE STRING "" FORCE)
set(ANDROID_ABI "armeabi-v7a" CACHE STRING "" FORCE)

include("${CMAKE_CURRENT_LIST_DIR}/lib/android.toolchain.cmake")