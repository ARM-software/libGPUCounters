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

install(PROGRAMS $<TARGET_FILE:api-example>
    DESTINATION bin
    RENAME hwcpipe-api-example
)

target_compile_options(api-example
    PRIVATE -Werror
            -Wswitch-default
            -Wswitch-enum
)