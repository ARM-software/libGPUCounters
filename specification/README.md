# libGPUCounters Machine Readable Specification

This project provides a programmatic way for third-parties to build tooling and
documentation that use information about Arm GPU performance counters. It is a
machine readable specification that defines the counters available on any
supported GPU, and developer documentation for each of them.

The database of information about the counters is stored as plain XML and YAML
text files on disk. Users access the database using Python wrappers, which
handle loading and cross-referencing the data sources ready for use.

## Python  wrapper: lgcpy

The Python wrapper classes come in two types: data classes and view classes.

The data classes can load XML or YAML in to an in-memory form, and serialize
back to disk if requested. However, they are not designed for direct use by
users because database entries can be shared by multiple GPUs and therefore can
only contain ambiguous symbolic references to other counters. These symbolic
references must be resolved for a specific GPU, based on the counters and
configuration of that product.

The view classes, which are designed for developer use, provide a structured
way to access the data for a single GPU product. Views give three benefits to
users.

* `CounterView` instances contain the resolved counter information for that
  specific GPU, giving the accurate names and counter derivations for that
  product after removing aliases and resolving symbolic references.
* Higher level View types provide a structured way to iterate the counters,
  for example allowing iteration based on hardware memory layout order or
  recommended presentation semantic order.
* Views can be filtered to restrict what is returned, for example hiding
  counters above a certain complexity level or hiding derived counters.

The Python library provides three views:

* An `IndexedView` provides an view that allows unstructured iteration and
  random access, providing a variety of options for indexing the data without
  imposing any higher-level structure.
* A `HardwareView` provides a structured view that allows users to iterate
  through counter blocks and counters in hardware memory layout order. This
  view only allows iteration of hardware counters, and will not contain any
  derived counters.
* A `SemanticView` provides a structured view that allows users to iterate
  through counters in a recommended reading order. This type of view arranges
  counters into a three level hierarchy of sections, groups, and counters.
  Sections represent a collection of related groups. Groups represent a
  set of counters that could be presented as a single chart. Counters
  represent single data series in that group chart.

## IP Support

This specification supports Mali-G71 (Bifrost architecture) or later.

## License

This public release of this specification and associated Python tooling is
MIT licensed. Not all performance counters are included in the public release.

## Developer Documentation

The following programmer guides are provided for users and maintainers of the
library.

* [Python library user guide](documentation/user_guide.md)
* [Performance counter technology guide](documentation/counter_guide.md)
* [Performance counter terminology guide](documentation/terminology_guide.md)

- - -

_Copyright © 2019-2025, Arm Limited._
