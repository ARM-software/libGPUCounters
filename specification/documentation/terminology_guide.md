# Performance counter terminology guide

This library has lots of different styles of "names" for each performance
counter, and conventions for what particular terms mean. This section of
the documentation aims to explain these ...

## Counter names

Each counter has multiple names, which can be used to uniquely identify it.

The counter _Machine name_ is a compact variable name which is the primary
key for the counter in the database. It is also used as the enum identifier in
the libGPUCounters C++ interface.

The counter _Human name_ is the fully qualified human-readable name of the
counter, it's unambiguous but can be quite long. These names are globally
unique.

The counter _Group name_ is the name of the group the counter belongs to. Group
names are globally unique, but multiple counters can belong to the same group.
Groups can be used to define a single chart for presentation, with each counter
being a series on the chart.

The counter _Group human name_ is the name of the counter, in the context of
the _Group name_. These names are shorter than the full _Human name_ which
makes them excellent short names to use in tools, but they are not globally
unique and tools need to present them alongside the _Group name_. For example
a tool could use the _Group name_ as a chart title, and then use the
_Group human name_ as the name of each series in the chart legend.

The counter _Stable ID_ is a unique numeric identifier which is guaranteed to
stay stable across releases, as long as the counter semantic meaning is
unchanged, even if we change the name strings for the counter.

## Counting cycles

Many performance counters count the number of cycles when something happened,
but not all counts of cycles count the same thing. We therefore have specific
terms for specific types of measurement.

Counters that measure _Active cycles_ count the number of cycles that a queue
or block contained a workload. These counters are a measure of scheduling
behavior, not functional unit throughput. For example, a block with a 10 stage
pipeline issuing 1 operation every 10 cycles would count as 100% active because
the block was always doing something.

Counters that measure _Issue cycles_ count the number of cycles that a queue or
block issued new work to the head of its pipeline. These counters are a measure
of functional unit throughput. For example, a block with a 10 stage pipeline
issuing 1 operation every 10 cycles would count as only 10% utilized because
most cycles were not actually issuing useful work.

## Counting memory accesses

As a data-plane processor, memory accesses are important to a GPU and bring
their own set of terminology for users to worry about.

A memory _Transaction_ represents a single operation on the memory bus.
Transactions have variable size, and may take variable numbers of data
transfer cycles.

A memory _Beat_ represents a single memory bus data transfer cycle. A
transaction transferring 512 bits on a 128-bit memory bus would take 4 data
beats. Beats have variable size, depending on memory bus hardware
implementation.

A memory _Byte_ represents the actual data bandwidth transferred on the bus.
Most performance counters that count bytes are actually derived counters that
are based on the number of hardware beats, scaled by the bus width, ignoring
partially used bus beats that have lanes masked.

- - -

_Copyright © 2025, Arm Limited._
