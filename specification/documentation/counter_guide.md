# Performance counter guide

Performance counters provide developers with insights about what is happening
inside the hardware. However, building tooling that is working with performance
counters means that you do need to understand some of the underlying hardware
behaviors so that you correctly interpret the counter data.

Note that users of libGPUCounters as a sampling library will have had these
issues handled by the library for the counters that the library provides
directly, but may run into some of these issues if they use the library to
compute their own derived expressions manually.

## Counter hardware

Arm GPUs consist of multiple structural hardware blocks that implement aspects
of the GPU functionality. These blocks include:

* _GPU front-end_ which implements the scheduler for work submitted by the
  graphics driver.
* _Tiler_ which implements the pipeline that coordinates the geometry binning
  phase for render passes.
* _Shader cores_ which implements the programmable core that processes all
   types of application shader workload. There may be multiple shader core
   block instances in a design.
* _L2 cache slices_ which implement the GPU L2 cache and MMU. There is only a
  single MMU, which is reported in block instance zero, but may be multiple
  cache slice instances in a design.

For blocks that allow multiple instances, the instance count will vary
depending on the performance needs of the specific vendor's hardware
implementation. Each instance will return a block of counters to the driver. In
most developer tooling, hardware counters are accumulated across all blocks to
give a per-GPU total.

## Counter types

The machine readable specification exposes three types of counter.

_Basic hardware counters_ are counters that are sampled from the hardware and
return values that can be presented directly.

```py
CounterView.is_derived() == False
CounterView.scale_multiplier() == 1
```

_Scaled hardware counters_ are counters that are sampled from the hardware
but stored by the hardware at a coarse granularity, incrementing by 1 for every
N event instances that occur. Counter values must be scaled by the consumer
to create the value that matches the description.

```py
CounterView.is_derived() == False
CounterView.scale_multiplier() > 1
```

_Derived counters_ are software-defined counters that are specified using an
textual source code equation that can be parsed into a machine-usable AST.
Computing the value returned by this equation, feeding in appropriate hardware
counters as source data, will give the specified counter value.

```py
CounterView.is_derived() == True
CounterView.scale_multiplier() == 1
```

## Derived counters

Derived counters are defined using textual source equations that can be
parsed and manipulated programmatically. An equation can contain references to
other counters, of any type, as well as literal values, and symbolic constants.

Symbolic constants are placeholders for the target system configuration that
must be resolved to a concrete value by the tooling at run-time when the
target device and measurement methodology is known. These constants are:

* `MALI_CONFIG_SHADER_CORE_COUNT`: Number of shader cores.
* `MALI_CONFIG_L2_CACHE_COUNT`: Number of L2 cache slices.
* `MALI_CONFIG_EXT_BUS_BYTE_SIZE`: Width of GPU external bus interface in bits.
* `MALI_CONFIG_TIME_SPAN`: Time span represented by a counter sample.

Derived counters in the database can be defined using references to other
derived counters. These references may be ambiguous when viewed globally
because two GPUs may compute a counter that is semantically the same very
differently. Resolving derived counters to an expression that contains only
references to hardware counters must therefore be done on a per-GPU basis.

### Counter cardinality

Arm GPU counter samples contain counters for every implemented block. In a
design with multiple block instance of a block type, each instance would emit
counters. For example, in a product with 6 shader cores there will be 6 copies
of each shader core counter emitted by the hardware when a sample is taken.

When computing derived expressions with counters from multiple block types,
beware that summation means that care needs to be taken to get the correct
result. For example, it may seem like you can compute shader core busyness by
dividing "Shader core clock cycles" by "GPU clock cycles" to get a percentage:

```
Shader core utilization = (Shader core cy / GPU cy) * 100
```

However, because there might be multiple shader core instances the two counters
are not counting the same thing. You must divide by the shader core count to
scale the two counters to the same normalized per instance view:

```
Shader core utilization = (Shader core cys / (Shader core count * GPU cy) * 100
```

Expressions defined in the database have already been defined with correction
factors for any block cardinality differences, by including any necessary
scaling of parts of each expression. However, user applications computing their
own derived expressions by hand must be aware of block cardinality differences
and apply any scaling that is needed.


### Counter clock domains

Arm GPUs can have multiple clock domains, with blocks of each type running at a
different frequency to other blocks types. For example, in larger GPU
configurations it is common to run the shader cores at a lower clock frequency
than the GPU front end and the L2 memory system. This allows the shader cores
to run at lower voltage, maximizing energy efficiency.

The clock domains are:

* The _GPU front-end_ and _Tiler_ block types are in the top-level clock
  domain.
* The _Shader core_ block type is in the shader code clock domain.

When computing derived expressions with counters from multiple block types,
beware that clock scaling may mean that results are hard to interpret. Taking
our previous example, we wanted to compute shader core busyness by
dividing "Shader core clock cycles" by "GPU clock cycles" to get a percentage:

```
Shader core utilization = (Shader core cys / (Shader core count * GPU cy) * 100
```

This only gives a value where 100% means shader core fully utilized if the
shader core and the GPU front-end are running at the same frequency. In reality
this expression may return a value significantly lower than 100%, even if the
shader core is fully loaded, because the shader is running at a lower clock
frequency.

Clock-frequencies are implementation-defined and can change regularly at
run-time due to dynamic voltage and frequency scaling (DVFS) power management.
There is therefore no way to effectively correct for this effect using static
scaling factors.

- - -

_Copyright © 2025, Arm Limited._
