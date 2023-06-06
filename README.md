# `HWCPipe 2.0`

## Introduction

Currently access to Hardware Counters is exposed via ioctl interfaces. That requires some knowledge
about driver internals and sometimes about hardware itself. Software that needs to access Mali
counters needs to interface with internals somwehow.

HWCPipe 2.0 is a second version of a currently existing open source solution maintained by Arm.
Its aim is to support an up-to-date way to interface with the latest drivers. The library is
intended to be more modular than its predecessor and support different use-cases while still
exposing a simple API to access GPU counters.

## Building the Library

The library is designed to be embedded into your existing CMake build process. Clone the GitHub
repository and add it as a subdirectory to your CMakeLists.txt. A single library target, called
`hwcpipe`, is made available for you to link to your project.

For example, your CMakeLists.txt could contain:
```
add_subdirectory(external/hwcpipe)
target_link_libraries(my_project hwcpipe)
```

## Building the Examples

A short example of the API usage is provided in the `examples` folder. Enable the
HWCPIPE_BUILD_EXAMPLES CMake option to build the example program with the library.

Run CMake from the root of the repository:

```cmake -DHWCPIPE_BUILD_EXAMPLES=ON -B build .```

