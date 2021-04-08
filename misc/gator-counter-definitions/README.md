Arm Mali Counter Descriptions
=============================

This directory contains the Arm Streamline events definitions for Mali GPUs used by gatord [https://github.com/ARM-software/gator/].

License
-------

These files are released under the same MIT license used for the rest of the HWCPipe project.

Description
-----------

The files are taken verbatim from Streamline 7.6 release so contain fragments of XML (the enclosing root element, processing instructions etc are missing). 

The files are organised by GPU, and within each file, there are <category> elements which organise counters into logical groups, and within each category, are
<event> elements representing the individual counters. Each event has a title, name, description and units element providing metadata about the event.
* The title attribute groups related events into the same chart (events with the same title will appear as different series within the same chart within 
Streamline).
* The name attribute provides the individual series name.
The event has a counter_set attribute which maps the metadata in the XML back to the original raw counter identifier as per 
https://github.com/ARM-software/gator/blob/master/daemon/mali_userspace/MaliHwCntrNames.h and should be relatable to the same entry within HWCPipe 
https://github.com/ARM-software/HWCPipe/blob/master/vendor/arm/mali/hwc_names.hpp.

A copy of MaliHwCntrNames.h is provided as part of this repository, also under MIT license, providing the required raw counter identifiers for the Mali G68 
and G78 GPUs.

