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

#pragma once

#include <fmt/format.h>

#define HWCPIPE_TAG "HWCPipe"

namespace hwcpipe
{
// The severity of a log
enum class LogSeverity
{
	Verbose,
	Info,
	Debug,
	Warn,
	Error,
	Fatal
};

// Using the DebugLogCallback allows a project using HWCPipe to define custom logging behaviour
typedef void (*DebugLogCallback)(LogSeverity severity, const char *message);

void set_logger(DebugLogCallback callback);

DebugLogCallback get_logger();

template <typename... Args>
void log(LogSeverity severity, const std::string &format, Args... args)
{
	auto logger = get_logger();
	if (logger != nullptr)
	{
		logger(severity, fmt::format(format, args...).c_str());
	}
}
}        // namespace hwcpipe
