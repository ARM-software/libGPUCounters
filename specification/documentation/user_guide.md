# User guide

This library provides access to a machine readable specification for Arm GPU
performance counters, providing a way to enumerate the counters and associated
documentation for any of the supported GPUs.

The underlying database is stored as files on disk, representing a combined
view for all supported GPUs. To use the database, you must create an
appropriate per-product view on to the data that is specialized for the product
that you are using.

The main interface to the database is the `CounterDatabase` class, which
provides a number of factory methods that can return a built view for a
specific product, as well as a way to enumerate the supported products.

There are multiple container view types, described below, that each provide a
different way to structure the data. The leaf nodes in all of the container
views are common `CounterView` instances that contain the per-counter
information for the GPU that the view represents.

## Querying supported GPUs

You can query the GPUs that are supported in the database, including any
product name aliases, using the following code:

```py
from lgcpy import CounterDatabase

gpus = CounterDatabase.get_supported_gpus()
for gpu in gpus:
    info = CounterDatabase.get_product_info_for(gpu)
    print(f'Names: {", ".join(info.names)}')
```

## Building an IndexedView

Building an `IndexedView` gives you a way to efficiently random access into the
database, as well as supporting basic iteration through all supported counters.
Note that the order of iteration follows no structure, and could change from
release to release.

```py
from lgcpy import CounterDatabase

view = CounterDatabase.get_indexed_view_for('Mali-G710')

# Random index the GPU Active cycles counter by the various means
counter = view.get_by_stable_id(6)
print(f'Counter: {counter.human_name}')

counter = view.get_by_machine_name('MaliGPUActiveCy')
print(f'Counter: {counter.human_name}')

# Note: Source name indexing is only available for native hardware counters
counter = view.get_by_source_name('GPU_ACTIVE')
print(f'Counter: {counter.human_name}')

counter = view.get_by_human_name('GPU active cycles')
print(f'Counter: {counter.human_name}')

counter = view.get_by_group_names('GPU Cycles', 'GPU active')
print(f'Counter: {counter.human_name}')

# Iterate all CounterView instances in arbitrary order
for counter in view:
    print(f'Counter: {counter.human_name} (Derived: {counter.is_derived()})')
```

## Building a HardwareView

Building a `HardwareView` gives you a way to efficiently iterate though the
counters, grouped by hardware block and in hardware block counter order. This
style of view only exposes hardware counters, with no access to derived
counters.

Note that counters can be laid out sparsely in each hardware memory block, so
you must use `CounterView.block_index` to determine memory location of a
counter, not its index in the enumerated Python sequence.

```py
from lgcpy import CounterDatabase

view = CounterDatabase.get_hardware_view_for('Mali-G710')

# Iterate all CounterView instances in block order, block-by-block
for block in view:
    print(f'Block: {block.btype.name}')
    for counter in block:
        print(f'  [{counter.block_index:03}] {counter.human_name}')

# Iterate all CounterView instances in block order
for counter in view.iter_counters():
    print(f'  [{counter.block_index:03}] {counter.human_name}')
```

## Building a SemanticView

Building a `SemanticView` gives you a way to efficiently iterate though the
counters, arranged into a recommended presentation order. This ordering is a
three level hierarchy of Sections, Groups, and Counters.

* Sections represent overarching topics.
* Groups represent a single chart for presentation.
* Counters represent a single series on a chart.

Documentation is provided for all three hierarchy levels, allowing user
documentation to be built automatically from the content of the database.

```py
from lgcpy import CounterDatabase

view = CounterDatabase.get_semantic_view_for('Mali-G710')

# Iterate all CounterView instances in presentation order, section-by-section
for section in view:
    print(f'Section: {section.name}')
    for group in section:
        print(f'  Group: {group.name}')
        for counter in group:
            print(f'    Counter: {counter.group_human_name}')

# Iterate all CounterView instances in presentation order
# Note that this way of iteration has no access to the top-level section names
for counter in view.iter_counters():
    print(f'  [{counter.group_name}] {counter.group_human_name}')
```

## Filtering a view

Once you have built a view you can filter it to hide counters that are not
needed for your use case. Filtering can hide counters above a certain level
of user complexity and can hide derived counters.

Counter visibility has four levels, defined in the `CounterVisibility` enum.

* `NOVICE`: Counters expected to be useful for application developers without
  needing a lot of background knowledge about GPU hardware.
* `ADVANCED_APPLICATION`: Counters expected to be useful for expert application
  developers who have more background knowledge about GPU hardware and Arm
  GPUs in particular.
* `ADVANCED_SYSTEM`: Counters expected to be useful for expert system
  integrators who have more background knowledge about system hardware and Arm
  GPUs in particular.
* `INTERNAL`: Counters expected to be useful to Arm engineers, but not expected
  to be useful outside of Arm. Internal counters are stripped from public
  releases, and are undocumented.

```py
from lgcpy import CounterDatabase, CounterVisibility

view = CounterDatabase.get_semantic_view_for('Mali-G710')

# Only keep NOVICE and ADVANCED_APPLICATION hardware counters.
filtered_view = view.filter(CounterVisibility.ADVANCED_APPLICATION, False)

for section in filtered_view:
    print(f'Section: {section.name}')
    for group in section:
        print(f'  Group: {group.name}')
        for counter in group:
            print(f'    Counter: {counter.group_human_name}')
```

- - -

_Copyright © 2025, Arm Limited._
