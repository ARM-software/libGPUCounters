# HWCPipe


## Introduction

HWCPipe is an interface to the Arm Hardware Counters, designed to allow for easily interfacing with and reading the hardware counters built into Arm hardware.


## License

The software is provided under an MIT license. Contributions to this project are accepted under the same license.


## Building

To use HWCPipe, build it as a shared library in your Android Project, to do this it must be integrated into your project with CMake.


## Using

### Performance data

In order for CPU performance data to be displayed, profiling needs to be enabled on the device.
Some devices may disable it by default.

Profiling can be enabled via adb:

```
adb shell setprop security.perf_harden 0
```

GPU performance data should be collected regardless of this setting.

#### Enabling a Counter:

To enable a counter, create either a PMU or Mali counter and then call its start function.

```
Instrument instrument_ = PMUCounter();
instrument_.start();
```

#### Reading a Counter:

To read a counter, first stop it and then call its measurements function to store results in the MeasurementsMap variable which can then be read from.

```
instrument_.stop();
MeasurementsMap measurements = instrument_.measurements();
```


## Counters

The counters are separated into two categories: PMU and Mali counters, the available counters are:

#### PMU

 - CPU cycles
 - CPU instructions
 - Cache miss ratio
 - Branch miss ratio

#### Mali

 - Timespan
 - GPU cycles
 - Fragment Jobs
 - Vertex/compute jobs
 - L2 cache read lookups
 - L2 cache external reads
 - L2 cache write lookups
 - L2 cache external writes

For more information regarding these counters, see [Mali Performance Counters](https://community.arm.com/graphics/b/blog/posts/mali-bifrost-family-performance-counters).
