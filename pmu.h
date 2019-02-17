/*
 * Copyright (c) 2017-2019 ARM Limited.
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

#include <cstdint>
#include <errno.h>
#include <linux/perf_event.h>
#include <stdexcept>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>

#define HWCPIPE_TAG "HWCPipe"

#if defined(__ANDROID__)
#	include <android/log.h>

#	define HWCPIPE_LOG(...) __android_log_print(ANDROID_LOG_VERBOSE, HWCPIPE_TAG, __VA_ARGS__)
#else
#	define HWCPIPE_LOG(...)                              \
		{                                                 \
			fprintf(stdout, "%s [INFO] : ", HWCPIPE_TAG); \
			fprintf(stdout, __VA_ARGS__);                 \
			fprintf(stdout, "\n");                        \
		}
#endif

/** Class provides access to CPU hardware counters. */
class PMU
{
  public:
	/** Default constructor. */
	PMU();

	/** Create PMU with specified counter.
     *
     * This constructor automatically calls @ref open with the default
     * configuration.
     *
     * @param[in] config Counter identifier.
     */
	explicit PMU(uint64_t config);

	/** Default destructor. */
	~PMU();

	/** Get the counter value.
     *
     * @return Counter value casted to the specified type. */
	template <typename T>
	T get_value() const;

	/** Open the specified counter based on the default configuration.
     *
     * @param[in] config The default configuration.
     */
	void open(uint64_t config);

	/** Open the specified configuration.
     *
     * @param[in] perf_config The specified configuration.
     */
	void open(const perf_event_attr &perf_config);

	/** Close the currently open counter. */
	void close();

	/** Reset counter. */
	void reset();

	/** Print counter config ID. */
	std::string config_to_str(const perf_event_attr &perf_config);

  private:
	perf_event_attr _perf_config;
	long            _fd{-1};
};

template <typename T>
T PMU::get_value() const
{
	T             value{};
	const ssize_t result = read(_fd, &value, sizeof(T));

	if (result == -1)
	{
		throw std::runtime_error("Can't get PMU counter value: " + std::to_string(errno));
	}

	return value;
}
