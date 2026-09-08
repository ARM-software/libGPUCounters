# Device

## Overview

The device library provides low level access to the hardware counters data. The library hides the complexity of
different versions of the Mali KMD and abstracts away hardware specifics.

The library provides access to:

- Kernel side hardware counters samplers (manual and periodic)
- Device dependent constants, e.g. number of shader cores, cache slices, cache size, etc.

The library also supports error reporting and logging in order to extend its debugging features.

## Usage

Minimal example:

```
namespace dev = hwcpipe::device;

// Create Mali device handle.
auto hndl = dev::handle::create();
assert(hndl && !"Failed to create Mali device handle.");

// Create Mali device instance and return error code.
std::error_code ec;
auto instance = dev::instance::create(*hndl, ec);
assert(instance && !"Failed to create Mali device.");

// Enable all counter masks and prepare configuration data.
dev::hwcnt::sampler::configuration::enable_map_type enable_map;
enable_map.set();
dev::hwcnt::sampler::configuration configs[] = {
	{ dev::hwcnt::block_type::fe, dev::hwcnt::prfcnt_set::primary, enable_map },
};
size_t config_len = std::extent<decltype(configs)>::value;

// Create manual hardware counters sampler.
dev::hwcnt::sampler::manual manual(*instance, configs, config_len);

// Start accumulating counters in the hardware registers.
manual.accumulation_start();

// Request hardware counter samples.
manual.request_sample(0);

// Wait for the sample completion and hand the sample buffer to the user space.
std::error_code ec;
dev::hwcnt::sample sample{manual.get_reader(), ec};

for (const auto block : sample.blocks())
	process_counter_values(block.get_metadata().values);

// Stop counters accumulation.
manual.accumulation_stop(0);
```

## Logging {#logging}

HWCPipe provides an interface to integrate an external logger to redirect info and error messages.

To use the logger, a header file has to be provided containing the logging macros which when compiled in will enable the
logging feature.

This section explains how to implement and enable the logger for HWCPipe.

### Supported logging functions {#supported-logging-functions}

HWCPipe supports the following logging functions:

- `HWCPIPE_LOG_INFO(...)`: which logs general info messages.
- `HWCPIPE_LOG_ERROR(...)`: which logs error messages.

### Implement the logger

In a new header file, implement the variable args macros `HWCPIPE_LOG_INFO` and `HWCPIPE_LOG_ERROR` which takes
arguments in printf format.

An example of a simple logger printing to stdout and stderr:

```
/**
 * @file logger_external.hpp
 *
 * Macros implementing hwcpipe logger.
 */
#pragma once

#include <stdio.h>

#define HWCPIPE_LOG_INFO(...)    fprintf(stdout, __VA_ARGS__)
#define HWCPIPE_LOG_ERROR(...)   fprintf(stderr, __VA_ARGS__)
```

### Enable the logger

In order to enable the logger in HWCPipe, the library has to be built with the path to the logger header defined using
the `HWCPIPE_LOGGER_HEADER` build option.

Example:

```
cmake -S ./ -B ./out -DHWCPIPE_TOOLCHAIN=${TOOLCHAIN} -DHWCPIPE_TARGET_ARCH=${ARCH} -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} -DHWCPIPE_LOGGER_HEADER=path/to/logger_external.hpp
```

In case of using the build script `cmake.py`, you need to pass the path to the logger header using the option
`--logger-header` .

Example:

```
./build/cmake.py generate --os=${OS} --toolchain=${TOOLCHAIN} --build-type=${BUILDTYPE} --arch=${ARCH} --logger-header=path/to/logger_external.hpp
```

## Error reporting

HWCpipe provides support for extended error reporting using std::error_code, a list of the library's error codes and
messages is defined in include/device/error.hpp and src/device/error.cpp.

To get the error message of an error code, use the function `hwcpipe::device::get_error_message(std::error_code)`.

ex:

```
std::error_code ec;
auto instance = dev::instance::create(*hndl, ec);
if (ec)
	fprintf(stderr, "Instance failed to create with error code %i, message: %s\n,
			ec.value(), hwcpipe::device::get_error_message(ec).c_str());
```

Error codes and messages will be logged if [logging](#logging) is enabled and
[HWCPIPE_LOG_ERROR](#supported-logging-functions) is defined.
