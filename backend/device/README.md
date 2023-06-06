# Device

## Overview

The device library provides low level access to the hardware counters data.
The library hides the complexity of different versions of the Mali KMD and
abstracts away hardware specifics.

The library provides access to:

- Kernel side hardware counters samplers (manual and periodic)
- Device dependent constants, e.g. number of shader cores, cache slices, cache size, etc.

## Usage

Minimal example:

```
namespace dev = hwcpipe::device;

// Create Mali device handle.
auto hndl = dev::handle::create();
assert(hndl && !"Failed to create Mali device handle.");

// Create Mali device instance.
auto instance = dev::instance::create(*hndl);
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
