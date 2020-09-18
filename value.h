/*
 * Copyright (c) 2019 ARM Limited.
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

#include <limits>

namespace hwcpipe
{
typedef long long IntType;
typedef double    DoubleType;

class Value
{
  public:
	Value() :
	    is_int_(true),
	    int_(0),
	    double_(0.0f)
	{}
	Value(IntType value) :
	    is_int_(true),
	    int_(value)
	{}
	Value(DoubleType value) :
	    is_int_(false),
	    double_(value)
	{}

	bool valid() const
	{
		return is_int_ ? int_ == InvalidInt : double_ == InvalidDouble;
	}

	template <typename T>
	T get() const
	{
		return is_int_ ? static_cast<T>(int_) : static_cast<T>(double_);
	}

	void set(IntType value)
	{
		int_    = value;
		is_int_ = true;
	}

	void set(DoubleType value)
	{
		double_ = value;
		is_int_ = false;
	}

	static constexpr IntType    InvalidInt    = std::numeric_limits<IntType>::max();
	static constexpr DoubleType InvalidDouble = std::numeric_limits<DoubleType>::max();

  private:
	bool       is_int_;
	IntType    int_{0};
	DoubleType double_{0.0};
};
}        // namespace hwcpipe
