# About

libGPUCounters (formerly called HWCPipe) is a utility that allows applications to sample performance
counters from Arm® Immortalis™ and Arm Mali™ GPUs. This information allows developers to profile and
optimize their application workload using their existing in-house performance tooling, and gives the
ability to display performance data live in the application user interface.

The 2.x release series is a major rewrite of the library, capable of exposing all of the public
performance counters accessible in the Arm Streamline profiler. This version of the library
is not API compatible with the 1.x series, and no longer supports Arm CPU performance counters.

## Supported devices

This library aims to support all Arm GPU products from the Mali-T700 series onwards, ensuring
developers have coverage of the vast majority of smartphones with Arm GPUs that are in use today.
If you find a device with an Arm GPU which does not work, or gives inaccurate results, please open
an Issue on the GitHub issue tracker.

This library only supports devices using the Arm commercial driver.

## License

This project is licensed under the MIT license. By downloading any component from this repository
you acknowledge that you accept terms specified in the [LICENSE.md](LICENSE.md) file.

# Building

The HWCPipe library is designed to be embedded into existing CMake build process. Clone the GitHub
repository and add it as a subdirectory to your CMakeLists.txt. A single library target, called
`hwcpipe`, is made available for you to link to your project.

For example, your `CMakeLists.txt` could contain:

```
add_subdirectory(external/hwcpipe)
target_link_libraries(my_project hwcpipe)
```

## Building the example

A small example demonstrating the API usage is provided in the `examples` folder. To build the
example enable the `HWCPIPE_BUILD_EXAMPLES` CMake build option.

```
cmake -DHWCPIPE_BUILD_EXAMPLES=ON -B build .
```

# Support

If you have issues with the library itself, please raise them in the project's GitHub issue tracker.

If you have any questions about Arm GPUs, application development for Arm GPUs, or general mobile
graphics development or technology please submit them on the [Arm Community graphics forums][1].


- - -

_Copyright © 2023-2024, Arm Limited and contributors. All rights reserved._

[1]: https://community.arm.com/support-forums/f/graphics-gaming-and-vr-forum/
