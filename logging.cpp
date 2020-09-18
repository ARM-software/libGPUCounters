/*
 * Copyright (c) 2020 ARM Limited.
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

#include "logging.h"

#if defined(__ANDROID__)
#	include <android/log.h>
#endif

namespace hwcpipe
{
void default_logger(LogSeverity severity, const char *message)
{
#if defined(__ANDROID__)
	android_LogPriority priority;

	switch (severity)
	{
		case LogSeverity::Verbose:
			priority = ANDROID_LOG_VERBOSE;
			break;
		case LogSeverity::Info:
			priority = ANDROID_LOG_INFO;
			break;
		case LogSeverity::Debug:
			priority = ANDROID_LOG_DEBUG;
			break;
		case LogSeverity::Warn:
			priority = ANDROID_LOG_WARN;
			break;
		case LogSeverity::Error:
			priority = ANDROID_LOG_ERROR;
			break;
		case LogSeverity::Fatal:
			priority = ANDROID_LOG_FATAL;
			break;
		default:
			priority = ANDROID_LOG_DEFAULT;
	}

	__android_log_write(priority, HWCPIPE_TAG, message);
#else
	const char *severity_str;

	switch (severity)
	{
		case LogSeverity::Verbose:
			severity_str = "Verbose";
			break;
		case LogSeverity::Info:
			severity_str = "Info";
			break;
		case LogSeverity::Debug:
			severity_str = "Debug";
			break;
		case LogSeverity::Warn:
			severity_str = "Warn";
			break;
		case LogSeverity::Error:
			severity_str = "Error";
			break;
		case LogSeverity::Fatal:
			severity_str = "Fatal";
			break;
		default:
			severity_str = "";
	}

	fmt::print("{} [{}]: {}\n", HWCPIPE_TAG, severity_str, message);
#endif
}

DebugLogCallback logger = &default_logger;

void set_logger(DebugLogCallback callback)
{
	logger = callback;
}

DebugLogCallback get_logger()
{
	return logger;
}
}        // namespace hwcpipe