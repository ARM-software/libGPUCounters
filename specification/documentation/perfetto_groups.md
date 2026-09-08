# Perfetto grouping guide

The Android protocol for capturing performance counters through Perfetto
`traced` allows each counter to be associated with a set of groups selected
from a fixed list defined in the Protobuf protocol. Counters can be associated
with multiple groups where it makes sense to present the same information in
multiple places.

In our semantic layout machine readable specification, we define a three level
hierarchy of sections, groups, and counters. Our sections define coarse
categories of related counters, and our groups define relatively small sets of
counters that can be coherently plotted on a single chart.

Perfetto only defines a two level hierarchy of groups and counters. The
Perfetto groups are therefore semantically closer to our use of sections. A
Perfetto group will contain a large number of counters which cannot be
coherently plotted on a single chart, both due to number of counters and also
because counters will often be measuring fundamentally different things.

## Perfetto groups

The set of supported groups in the current protobuf protocol is:

```
enum GpuCounterGroup {
  UNCLASSIFIED = 0;
  SYSTEM = 1;
  VERTICES = 2;
  FRAGMENTS = 3;
  PRIMITIVES = 4;
  MEMORY = 5;
  COMPUTE = 6;
  RAY_TRACING = 7;
}
```

## Our use of Perfetto groups

We have applied the following approach to mapping Arm GPU counters to Perfetto
groups.

### Vertices group

The `VERTICES` group is used for all counters that can be clearly attributed to
geometry processing, and that are not related to handling of primitives. This
includes front-end queue scheduling, application-centric tiler counters, and
vertex shading of workloads scheduled in the binning phase of render passes.

Low-level tiler counters that do not map to application-usable feedback are
marked as `UNCLASSIFIED`.

#### Caveats

Arm GPU shader cores counters only distinguish two types of thread:
non-fragment and fragment. The non-fragment counter is included in this group,
but may not indicate vertex shading.

Arm GPUs with deferred vertex shading (DVS), which was introduced in the
Immortalis-G720 series hardware, do not process all vertex shading during the
binning phase of a render pass. Vertex shading that is deferred and processed
during the main phase will be counted in the `FRAGMENTS` group because it
cannot be separated from fragment shading.

### Primitives group

The `PRIMITIVES` group is used for all counters that can be clearly attributed
to count primitives (points, lines, triangles). This includes general primitive
counters, and primitive culling counters.

### Fragments group

The `FRAGMENTS` group is used for all counters that can be clearly attributed
to fragment shading, as well as any other workload that runs in the main phase
of a render pass. This includes front-end queue scheduling, shader core
scheduling, rasterization, ZS testing, and all main phase shading workloads.

Low-level fragment counters that do not map to application-usable feedback are
marked as `UNCLASSIFIED`.

#### Caveats

Arm GPUs with deferred vertex shading (DVS), which was introduced in the
Immortalis-G720 series hardware, do not only process fragment shading during
the main phase of a render pass. Vertex shading that is deferred and processed
during the main phase will be counted in this group because it cannot be
separated from fragment shading.

### Compute group

The `COMPUTE` group is used for all counters that ca be clearly attributed
to compute shading. This includes front-end queue scheduling, shader
core scheduling, and non-fragment shading workloads.

Low-level compute counters that do not map to application-usable feedback are
marked as `UNCLASSIFIED`.

#### Caveats

Arm GPUs use the hardware compute workload to implement many different logical
workloads. This includes geometry shaders, tessellation shaders, ray tracing
pipeline, and some transfers, in addition to compute shaders. The GPU scheduler
does not disambiguate between them, and they cannot be separated in the
counters, so compute queue scheduling counters may include work that is not
literal API compute dispatches.

Arm GPU shader cores counters only distinguish two types of thread:
non-fragment and fragment. The non-fragment counter is included in this group,
but may not indicate literal compute shading.

### Ray tracing group

The `RAY_TRACING` group is used for all counters that can be clearly attributed
to ray tracing. This includes front-end queue scheduling, shader
core scheduling, and shader core ray tracing unit behavior.

#### Caveats

Arm GPUs implement ray tracing pipeline using the compute queue in the
hardware. The scheduling hardware does not disambiguate different types of
compute workload, so scheduling counters in this group may include work that is
not related to ray tracing pipeline workloads.

Arm GPU shader cores counters only distinguish two types of thread:
non-fragment and fragment. The non-fragment counter is included in this group,
but may not indicate ray tracing pipeline ray generation shaders.

### Memory group

The `MEMORY` group is used for all counters related to the shader core L1
caches, the GPU L2 cache, the GPU MMU, the internal memory bandwidth within the
GPU, and behavior on the external memory interface with the rest of the system.

### System group

The `SYSTEM` group is currently used for GPU counters that are usable by
application developers and system integrators that cannot sensibly be
attributed to a more specific group because they exist on a common data path
that is shared by multiple groups.

This group is primarily used for counters that are part of the GPU front-end
scheduler that are not tied to a specific workload queue, shader processing
in the shader core which is workload agnostic.

Low-level counters that do not map to application-usable feedback are marked as
`UNCLASSIFIED`.

### Unclassified group

The `UNCLASSIFIED` group is used for counters that do not fall into the
remit of one of the above groups, or that have been determined to be low value
for application developers. It is also used for configuration constants, which
are exposed as pseudo-counters in the database.

It is also used for new hardware data paths, such as the Mali neural
accelerator used to accelerate the Vulkan data graph API, when there is no
suitable more specific Perfetto group yet.

## Our database encoding of Perfetto groups

Our database allows counters to be associated with zero or more Perfetto groups
where the group name is a string matching the Perfetto enumeration name.

We expect the Perfetto group names to evolve over time, and want to be able to
build for multiple versions of Android from a single version of the database.
To allow for this our Perfetto groups are stored as an ordered list of group
lists:

```xml
<Perfetto>
  <Groups>
    <GroupName>SHADER_CORE</GroupName>
    <GroupName>FRAGMENTS</GroupName>
  </Groups>
  <Groups>
    <GroupName>SYSTEM</GroupName>
    <GroupName>FRAGMENTS</GroupName>
  </Groups>
</Perfetto>
```

The first group list that contains entries that are all supported by the target
Android version will be selected. For example, with the definition above the
current Android version would match the second entry because `SHADER_CORE`
doesn't exist in the current Perfetto enumeration, but the first would be
selected for a future Android that adds this (hypothetical) group enum value to
improve the usability.

If no group list completely intersects the available groups, no counter
group is returned. Exporters should assigned the counter to the `UNCLASSIFIED`
group if this occurs, although it should not happen in a production database
for released versions of the Perfetto data protocol.

- - -

_Copyright © 2026, Arm Limited._
