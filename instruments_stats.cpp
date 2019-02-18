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

#include "instruments_stats.h"

#include <algorithm>
#include <numeric>

/** Perform an index sort of a given vector.
 *
 * @param[in] v Vector to sort
 *
 * @return Sorted index vector.
 */
template <typename T>
std::vector<size_t> sort_indices(const std::vector<T> &v)
{
	std::vector<size_t> idx(v.size());
	std::iota(idx.begin(), idx.end(), 0);

	std::sort(idx.begin(), idx.end(),
	          [&v](size_t i1, size_t i2) {
		          return v[i1] < v[i2];
	          });

	return idx;
}

InstrumentsStats::InstrumentsStats(const std::vector<Measurement> &measurements) :
    _min(nullptr),
    _max(nullptr),
    _median(nullptr),
    _mean(measurements.begin()->value().is_floating_point),
    _stddev(0.0)
{
	auto add_measurements = [](Measurement::Value a, const Measurement &b) {
		return a + b.value();
	};

	//Calculate min, max & median values
	auto indices = sort_indices(measurements);
	_median      = &measurements[indices[measurements.size() / 2]];
	_min         = &measurements[indices[0]];
	_max         = &measurements[indices[measurements.size() - 1]];

	Measurement::Value sum_values = std::accumulate(measurements.begin(), measurements.end(), Measurement::Value(_min->value().is_floating_point), add_measurements);

	// Calculate the relative standard deviation
	_mean = sum_values / measurements.size();
	std::vector<Measurement::Value> diff(measurements.size(), _min->value().is_floating_point);
	std::transform(measurements.begin(), measurements.end(), diff.begin(), [&](const Measurement &x) {
		return x.value() - _mean;
	});
	auto sq_sum   = std::inner_product(diff.begin(), diff.end(), diff.begin(), Measurement::Value(_min->value().is_floating_point));
	auto variance = sq_sum / measurements.size();
	_stddev       = Measurement::Value::relative_standard_deviation(variance, _mean);
}
