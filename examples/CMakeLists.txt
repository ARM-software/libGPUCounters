#
# Copyright (c) 2021-2023 Arm Limited.
#
# SPDX-License-Identifier: MIT
#

add_executable(api-example
    api_example.cpp
)

target_link_libraries(api-example
    PRIVATE hwcpipe
)

target_compile_options(api-example
    PRIVATE -Werror
            -Wswitch-default
            -Wswitch-enum
)