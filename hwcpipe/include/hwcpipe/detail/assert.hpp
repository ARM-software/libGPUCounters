/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cassert>
#include <exception>

#if HWCPIPE_TEST
class hwcpipe_assert_exception : public std::exception {
  public:
    const char *what() { return "hwcpipe assert called"; }
};
#define HWCPIPE_ASSERT(expr) ((expr) ? (expr) : throw hwcpipe_assert_exception())
#else
#define HWCPIPE_ASSERT(expr) (assert(expr))
#endif