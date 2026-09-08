/*
 * Copyright (c) 2024-2025 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * @file logger.hpp
 *
 * Macros implementing the logger.
 *
 * Logger macros are:
 * - HWCPIPE_LOG_INFO(...).
 * - HWCPIPE_LOG_ERROR(...).
 *
 * These macros takes the parameters in printf format and logs the info and
 * error messages to their corresponding channels.
 *
 * Error codes are logged automatically using HWCPIPE_LOG_ERROR when an
 * error code is created using HWCPIPE_MAKE_ERROR_CODE.
 *
 * To enable the logger:
 * - Define these macros in a header file.
 * - Set the HWCPIPE_LOGGER_HEADER to point at the header file at
 *   build time.
 */

#pragma once

#include <inttypes.h>

/**
 * HWCPIPE_LOGGER_HEADER
 *
 * This macro is defined by the integrator of the library during build time,
 * it should point at a header file containing the definitions of the logging
 * macros:
 * - HWCPIPE_LOG_INFO.
 * - HWCPIPE_LOG_ERROR.
 *
 * This definition will enable logging across the library, and If it is not
 * defined all logging will be disabled.
 */
#ifdef HWCPIPE_LOGGER_HEADER

#include HWCPIPE_LOGGER_HEADER

#if !defined(HWCPIPE_LOG_INFO) || !defined(HWCPIPE_LOG_ERROR)
#error "Both macros HWCPIPE_LOG_INFO and HWCPIPE_LOG_ERROR needs to be defined in the logger header file, refer to \
the file documentation for guidance."
#endif

#else /* HWCPIPE_LOGGER_HEADER */

/**
 * Base macro for logging info messages.
 *
 * If this macro is not defined by the integrator, it is disabled here.
 *
 * @param[in] ...   Variable args in printf format.
 */
#ifndef HWCPIPE_LOG_INFO
#define HWCPIPE_LOG_INFO(...)
#endif

/**
 * Base macro for logging error messages.
 *
 * If this macro is not defined by the integrator, it is disabled here.
 *
 * @param[in] ...   Variable args in printf format.
 */
#ifndef HWCPIPE_LOG_ERROR
#define HWCPIPE_LOG_ERROR(...)
#endif

#endif /* HWCPIPE_LOGGER_HEADER */
