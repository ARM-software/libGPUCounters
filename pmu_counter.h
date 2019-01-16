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

#include "instrument.h"
#include "pmu.h"

namespace vkb
{
/** Implementation of an instrument to count CPU cycles. */
class PMUCounter : public Instrument {
  public:
	/// @brief Construct a PMU counter.
	PMUCounter(){};

	std::string     id() const override;
	void            start() override;
	void            stop() override;
	MeasurementsMap measurements() const override;

  private:
	PMU       _pmu_cycles{PERF_COUNT_HW_CPU_CYCLES};
	PMU       _pmu_instructions{PERF_COUNT_HW_INSTRUCTIONS};
	PMU       _pmu_cache_references{PERF_COUNT_HW_CACHE_REFERENCES};
	PMU       _pmu_cache_misses{PERF_COUNT_HW_CACHE_MISSES};
	PMU       _pmu_branch_instructions{PERF_COUNT_HW_BRANCH_INSTRUCTIONS};
	PMU       _pmu_branch_misses{PERF_COUNT_HW_BRANCH_MISSES};
	long long _cycles{0};
	long long _instructions{0};
	long long _cache_references{0};
	long long _cache_misses{0};
	long long _branch_instructions{0};
	long long _branch_misses{0};
};
}        // namespace vkb
