/*
 * Copyright (c) 2026 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "logger.hpp"

#ifndef HWCPIPE_LOG_INFO
#define HWCPIPE_LOG_INFO(...)  hwcpipe::log_item(LOG_INFO, "[HWCPIPE] " __VA_ARGS__)
#endif

#ifndef HWCPIPE_LOG_ERROR
#define HWCPIPE_LOG_ERROR(...)  hwcpipe::log_item(LOG_ERROR, "[HWCPIPE] " __VA_ARGS__)
#endif