# Performance counter terminology guide

This library has lots of different styles of "names" for each performance
counter, and conventions for what particular terms mean. This section of
the documentation aims to explain these ...

## Counter naming

Each counter has multiple names that are used to uniquely identify it.

The counter _Machine name_ is a compact variable name which is the primary
key for the counter in the database. It is also used as the enum identifier in
the libGPUCounters C++ interface.

The counter _Human name_ is the fully qualified human-readable name of the
counter, it's unambiguous but might be quite long. These names are globally
unique.

The counter _Group name_ is the name of the group the counter belongs to. Group
names are globally unique, but multiple counters might belong to the same
group. Groups are used to define a single chart for presentation, with each
counter being a series on the chart.

The counter _Group human name_ is the name of the counter, in the context of
the _Group name_. These names are shorter than the full _Human name_ which
makes them excellent short names to use in tools, but they are not globally
unique and tools need to present them alongside the _Group name_. For example
a tool could use the _Group name_ as a chart title, and then use the
_Group human name_ as the name of each series in the chart legend.

The counter _Stable ID_ is a unique numeric identifier which is guaranteed to
stay stable across releases, as long as the counter semantic meaning is
unchanged, even if we update the string names for the counter.

## Cycle count naming

Many performance counters count the number of cycles when something happened,
but not all counts of cycles have the same semantics. We therefore have
specific terms for specific types of measurement.

Counters that measure _Active cycles_ count the number of cycles that a queue
or block contained a workload that was scheduled to run. These counters are a
measure of scheduling behavior, not functional unit throughput. For example, a
block with a 10 stage pipeline that was only issuing 1 operation every 10
cycles would count as 100% active because the block was always doing something,
even though it was not very busy.

Counters that measure _Queued cycles_ count the number of cycles that a queue
contained work that was ready to run, but that could not be started because of
resource contention with another queue. This number will be higher than the
number of _Active cycles_ for the queue.

Counters that measure _Issue cycles_ count the number of cycles that a hardware
processing block issued new work to the head of its pipeline. These counters
are a measure of functional unit throughput. The example above, a block with a
10 stage pipeline issuing 1 operation every 10 cycles, would count as only 10%
utilized because most cycles were not doing useful work.

Counters that measure _Stall cycles_ count the number of cycles that some point
in the pipeline failed to process work. A stall cycle at one point in the
pipeline may not mean an overall loss of performance, because other work may
be buffered in the pipeline. This buffering allows pipelines to absorb stalls
without any loss in performance as long as the buffers don't drain completely
for the critical-path sub-unit.

## Memory bus access terminology

As a data-plane processor, memory accesses are important to a GPU and bring
their own set of terminology for users to worry about.

A memory _Transaction_ represents a single operation on the memory bus.
Transactions have variable size, and may take variable numbers of data
transfer cycles.

A memory _Beat_ represents a single memory bus data transfer cycle. A
transaction transferring a suitably aligned burst of 512 bits on a 128-bit
memory bus would take 4 data beats. Beats have variable size, depending on
memory bus hardware width.

A memory _Byte_ represents the actual data bandwidth transferred on the bus.
Most performance counters that count bytes are actually derived counters that
are based on the number of hardware beats, scaled by the bus width, ignoring
partially used bus beats that have lanes masked.

## Cache access terminology

As a data-plane processor, cache accesses are important to a GPU and bring
their own set of terminology for users to worry about.

A _Cache miss_ represents a cache access cycle that missed in the cache, and
required a data fetch from further out in the memory hierarchy. Not every cache
miss will result in a memory access to the next level of the memory system, as
multiple threads may miss on the same cache line and share loads.

A _Cache hit_ represents a cache access cycle that hit in the cache, and
returned data directly from the cache without a memory access to the next level
of the memory system.

GPUs handle cache misses very differently to a CPU, exploiting the massively
multi-threaded nature of the core to hide the impact of cache misses. For most
data caches, the GPU is able to process a new miss and returning data for an
earlier miss in the same clock cycle. This will often make a miss zero cost in
terms of cycles, although it will reduce energy efficiency.

- - -

_Copyright © 2025, Arm Limited._
