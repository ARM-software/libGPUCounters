#
# Copyright (c) 2019-2025 Arm Limited.
#
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
#
'''
This module contains a data structure hierarchy that represents the
counter definitions for all supported GPUs. Counter definitions include five
types of counter:

* Hardware counters that are used direct.
* Hardware counters that are used after scaling by factor in hardwarelayout.
* Derived counters that are computed based on any other counter.
* Symbolic constants that resolve to an opaque value, where the actual value
  must be provided by the implementation at run-time. This is used to allow
  equations to refer to core count, cache count, etc.
* Symbolic literals that resolve to a literal number, which may be a float.

Counter definitions are scoped to a set of products to which they apply. There
may be multiple definitions for the same counter, with different descriptions
or derivations for different GPUs.

For derived counters, the resolution of any symbolic references must be done
per GPU. It is valid that a derived counter definition refers to another
named counter that has different definitions depending on product.

Entries in the database use a de-duplicated database key, rather than the
raw product name. This comes from the ProductInfo database for each product.

Editing helpers
===============

The XML loader is tolerant of some errors. For example, it will allow counters
with no assigned StableID even though released counters are required to have
one. This allows the validator tooling to propose and inject an unassigned
StableID automatically, rather than a human needing to manually track which IDs
can be used.

Published databases must have passed validation, and can assume that mandatory
fields are actually present, even though mypy typing says that they may not.

Major classes
=============

CounterVisibility is an enum class storing the visibility definition of a
counter. For non-confidential releases any internal counters are stripped
before release.

CounterTrend is an enum class storing the preferred direction of a counter,
showing if making it higher or lower indicates the application is becoming
more efficient.

CounterInfo is the class containing the parsed database entry for a single
counter. These entries are the raw multi-GPU database entries, and must be
compiled into a per-GPU CounterView if you need to resolve counters or
determine hardware memory layout.

CounterInfos is the overall container class containing all of the parsed
CounterInfo instances. It stores counters in database file order, allowing
counters to be serialized back to disk without introducing many file diffs.
File order does not imply any semantics, and counters can move around as we
manually maintain the XML files.

Source data
===========

XML database source files are ./database/counterinfo/*.xml.

To make maintaining and editing the XML files easier, the database is stored
split into multiple source files. This has no semantic meaning, and all files
are concatenated on load into a single flat database.
'''

from __future__ import annotations

import enum
import pathlib
from typing import Any, Optional
import xml.dom.minidom as md
import xml.etree.ElementTree as et

from .. import equationutils as eu
from .. import gpuutils as gu
from .. import xmlutils as xu


class CounterVisibility(enum.Enum):
    '''
    Counter visibility rules.

    NOVICE counters are generally usable by any application developer.

    ADVANCED_APPLICATION counters are useful to application developers, but
    require more developer knowledge about GPUs to use and interpret.

    ADVANCED_SYSTEM counters are useful for system integrators, as well as
    Arm engineers, but assume a lot of knowledge about Arm GPUs and systems
    to use and interpret.

    INTERNAL counters are useful inside of Arm, and assume a lot of Arm GPU
    knowledge. These counters are stripped from public releases, as they are
    not really useful to third-party developers.
    '''
    NOVICE = 1
    ADVANCED_APPLICATION = 2
    ADVANCED_SYSTEM = 3
    INTERNAL = 4

    @classmethod
    def from_xml(cls, value: str) -> CounterVisibility:
        '''
        Convert an XML string into an enum value.

        Args:
            value: The XML string value.

        Returns:
            The enum value.
        '''
        if value == 'Novice':
            return cls.NOVICE

        if value == 'Advanced application':
            return cls.ADVANCED_APPLICATION

        if value == 'Advanced system':
            return cls.ADVANCED_SYSTEM

        if value == 'Internal':
            return cls.INTERNAL

        assert False, f'Unknown enumeration string {value}'

    def to_xml(self) -> str:
        '''
        Serialize to XML.

        Args:
            value: The enum value.

        Returns:
            The XML string value.
        '''
        if self == self.NOVICE:
            return 'Novice'

        if self == self.ADVANCED_APPLICATION:
            return 'Advanced application'

        if self == self.ADVANCED_SYSTEM:
            return 'Advanced system'

        if self == self.INTERNAL:
            return 'Internal'

        assert False, f'Unknown enumeration value {self.value}'

    def __str__(self) -> str:
        return self.to_xml()


class CounterTrend(enum.Enum):
    '''
    Counter trend rules.

    HIGHER_BETTER counters indicate that performance or efficiency would
    improve if the number can be increased. This commonly applies to
    percentages that indicate efficiency or utilization.

    LOWER_BETTER counters indicate that performance or efficiency would
    improve if the number can be reduced. This commonly applies to absolute
    measures of work.

    INFORMATIVE counters indicate a neutral stance, so the counter conveys
    useful information but it's unclear if higher or lower would be an
    improvement due to use case dependencies the counter cannot show.
    '''
    HIGHER_BETTER = 1
    INFORMATIVE = 2
    LOWER_BETTER = 3

    @classmethod
    def from_xml(cls, value: str) -> CounterTrend:
        '''
        Convert an XML string into an enum value.

        Args:
            value: The XML string value.

        Returns:
            The enum value.
        '''
        if value == 'Higher better':
            return cls.HIGHER_BETTER

        if value == 'Lower better':
            return cls.LOWER_BETTER

        if value == 'Informative':
            return cls.INFORMATIVE

        assert False, f'Unknown enumeration string {value}'

    def to_xml(self) -> str:
        '''
        Serialize to XML.

        Args:
            value: The enum value.

        Returns:
            The XML string value.
        '''
        if self == self.LOWER_BETTER:
            return 'Lower better'

        if self == self.HIGHER_BETTER:
            return 'Higher better'

        if self == self.INFORMATIVE:
            return 'Informative'

        assert False, f'Unknown enumeration value {self.value}'

    def __str__(self) -> str:
        return self.to_xml()


class CounterInfo():
    '''
    Container class for counter info.

    Attributes:
        source_file: Name of the file containing this counter on disk.
        machine_name: Canonical machine readable name.
        stable_id: Canonical stable ID.
        source_name: Name of counter in the architecture, or None if derived.
        source_name_aliases: Other source names if architecture name changed.
        human_name: Fully qualified human readable name.
        group_name: Fully qualified human readable group name.
        group_human_name: Group qualified human readable name.
        visibility: Visibility level.
        short_description: Short description used for tooltips.
        long_description: Long description used for documentation.
        unit: Units of measurement.
        trend: Desirable trend of measurement.
        equation_text: Original source equation text, or None if not derived.
        equation_ast: Lark AST for the equation, or None if not derived.
        equation_ast_error: Lark error if AST parsing failed.
        gpu_support: List of GPU database keys that support this counter.
    '''
    # pylint: disable=too-many-instance-attributes

    def __init__(self, source_file: str, machine_name: str, human_name: str,
                 group_name: str, group_human_name: str,
                 short_description: str, long_description: str,
                 units: str, trend: CounterTrend,
                 visibility: CounterVisibility):
        '''
        Construct a new counter info.

        Args:
            source_file: Name of the file containing this counter on disk.
            machine_name: Canonical machine readable name.
            human_name: Fully qualified human readable name.
            group_name: Fully qualified human readable group name.
            group_human_name: Group qualified human readable name.
            short_description: Short description used for tooltips.
            long_description: Long description used for documentation.
            unit: Units of measurement.
            trend: Desirable trend of measurement.
            visibility: Visibility level.
        '''
        # pylint: disable=too-many-arguments,too-many-positional-arguments

        self.source_file = source_file
        self.machine_name = machine_name
        self.human_name = human_name
        self.group_name = group_name
        self.group_human_name = group_human_name

        self.short_description = short_description
        self.long_description = long_description

        self.units = units
        self.trend = trend
        self.visibility = visibility

        # Must be present in releases, but optional for pre-release
        self.stable_id: Optional[int] = None

        # Must have either a source_name or an equation, but not both
        self.source_name: Optional[str] = None

        self.equation_text: Optional[str] = None
        self.equation_ast: Optional[Any] = None
        self.equation_ast_error: Optional[str] = None

        self.gpu_support: list[str] = []
        self.source_name_aliases: list[str] = []

    def supports_gpu(self, key: str) -> bool:
        '''
        Is this counter supported by the given GPU?

        Args:
            key: The canonical database GPU name.

        Returns:
            True if supported, False otherwise.
        '''
        return key in self.gpu_support

    def to_xml(self, parent: et.Element[str]) -> None:
        '''
        Serialize to XML.

        Args:
            parent: The parent XML element to add this node to.
        '''
        node = et.SubElement(parent, 'CounterInfo')

        subnode = et.SubElement(node, 'MachineName')
        subnode.text = self.machine_name

        if self.source_name:
            subnode = et.SubElement(node, 'SourceName')
            subnode.text = self.source_name

        for alias in self.source_name_aliases:
            assert self.source_name
            if alias == self.source_name:
                continue
            subnode = et.SubElement(node, 'SourceAlias')
            subnode.text = alias

        subnode = et.SubElement(node, 'StableID')
        subnode.text = f'{self.stable_id}'

        subnode = et.SubElement(node, 'HumanName')
        subnode.text = self.human_name

        subnode = et.SubElement(node, 'GroupName')
        subnode.text = self.group_name

        subnode = et.SubElement(node, 'GroupHumanName')
        subnode.text = self.group_human_name

        subnode = et.SubElement(node, 'Units')
        subnode.text = self.units

        subnode = et.SubElement(node, 'Trend')
        subnode.text = self.trend.to_xml()

        subnode = et.SubElement(node, 'Visibility')
        subnode.text = self.visibility.to_xml()

        # Word wrap and format long fields so they are readable
        subnode = et.SubElement(node, 'ShortDescription')
        subnode.text = xu.to_pretty_xml(self.short_description, True)

        subnode = et.SubElement(node, 'LongDescription')
        subnode.text = xu.to_pretty_xml(self.long_description, True)

        if self.equation_ast:
            subnode = et.SubElement(node, 'Equation')
            equation_text = eu.equation_ast_to_string(self.equation_ast)
            subnode.text = xu.to_pretty_xml(equation_text)

        subnode = et.SubElement(node, 'SupportedGPUs')
        for gpu in self.gpu_support:
            gpu_node = et.SubElement(subnode, 'GPU')
            gpu_node.text = gpu

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = True
        self_same &= self.source_file == other.source_file
        self_same &= self.machine_name == other.machine_name
        self_same &= self.human_name == other.human_name
        self_same &= self.group_name == other.group_name
        self_same &= self.group_human_name == other.group_human_name
        self_same &= self.short_description == other.short_description
        self_same &= self.long_description == other.long_description
        self_same &= self.units == other.units
        self_same &= self.trend == other.trend
        self_same &= self.visibility == other.visibility
        self_same &= self.stable_id == other.stable_id
        self_same &= self.source_name == other.source_name
        self_same &= self.equation_ast == other.equation_ast
        self_same &= self.gpu_support == other.gpu_support
        self_same &= self.source_name_aliases == other.source_name_aliases

        return self_same

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    @classmethod
    def from_xml(cls, node: et.Element[str], source_file: str) -> CounterInfo:
        '''
        Factory method to create a new instance from an XML node.

        Args:
            node: XML containing the counter info.
            source_file: Path of the source file on disk.

        Returns:
            A new counter info node.
        '''
        # pylint: disable=too-many-locals

        # Check this is the correct type of XML node
        assert node.tag == 'CounterInfo'

        # Build the core object with the mandatory attributes
        machine_name = xu.get_node_str(node, 'MachineName')
        human_name = xu.get_node_str(node, 'HumanName')
        group_name = xu.get_node_str(node, 'GroupName')
        group_human_name = xu.get_node_str(node, 'GroupHumanName')

        raw_short_desc = xu.get_node_str(node, 'ShortDescription')
        short_desc = xu.from_pretty_xml(raw_short_desc, True)

        raw_long_desc = xu.get_node_str(node, 'LongDescription')
        long_desc = xu.from_pretty_xml(raw_long_desc, True)

        units = xu.get_node_str(node, 'Units')

        trend_raw = xu.get_node_str(node, 'Trend')
        trend = CounterTrend.from_xml(trend_raw)

        visibility_raw = xu.get_node_str(node, 'Visibility')
        visibility = CounterVisibility.from_xml(visibility_raw)

        info = cls(source_file, machine_name, human_name,
                   group_name, group_human_name,
                   short_desc, long_desc,
                   units, trend, visibility)

        # Assign any non-mandatory attributes piece-wise

        # Assign stable ID, if one exists
        info.stable_id = xu.get_node_opt_int(node, 'StableID')

        # Assign either source_name or equation
        source_name = xu.get_node_opt_str(node, 'SourceName')
        raw_equation = xu.get_node_opt_str(node, 'Equation')

        if source_name:
            assert not raw_equation
            info.source_name = source_name
            info.source_name_aliases.append(source_name)
        else:
            assert raw_equation
            assert not source_name
            equation_text = xu.from_pretty_xml(raw_equation)
            info.equation_text = equation_text

            parse_result = eu.equation_string_to_ast(equation_text)
            info.equation_ast = parse_result[0]
            info.equation_ast_error = parse_result[1]

        # Assign supported GPU list
        for child_node in node.findall('SupportedGPUs/GPU'):
            assert child_node.text
            info.gpu_support.append(child_node.text)

        # Assign source_name aliases
        for child_node in node.findall('SourceAlias'):
            assert child_node.text
            info.source_name_aliases.append(child_node.text)

        # Maintain sorted lists for ease of maintenance
        info.gpu_support = gu.sort_gpus(info.gpu_support)
        info.source_name_aliases.sort()

        return info


class CounterInfos(list[CounterInfo]):
    '''
    Container class for the CounterInfo database.

    The counter database is a common data store for all supported Arm GPUs,
    and must be built into a view for a specific product for use.

    Attributes:
        copyrights: Copyright messages we will emit when writing to file,
            stored per file as they may have different dates.
    '''

    def __init__(self):
        '''
        Construct a new counter infos container.
        '''
        super().__init__()
        self.copyrights: dict[str, str] = {}

    def to_xml_str(self, pretty_print: bool = False) -> dict[str, str]:
        '''
        Serialize to XML.

        Args:
            pretty_print: True to pretty print, False otherwise.

        Returns:
            Returns a mapping of source file to XML string.
        '''
        # Assemble the overall XML
        xml_by_file = {}
        for counter in self:
            # Create the root node for each unique file
            source_file = counter.source_file
            if source_file not in xml_by_file:
                xml_by_file[source_file] = et.Element('CounterInfoList')

            # Append counters file-wise with comment separator between entries
            node = xml_by_file[counter.source_file]
            node.append(et.Comment(' ' + '=' * 68 + ' '))
            counter.to_xml(node)

        # Serialize and format each XML, file by file
        str_by_file: dict[str, str] = {}
        for source_file, node in xml_by_file.items():
            document = et.tostring(node, encoding='unicode')

            if pretty_print:
                md_xml = md.parseString(document)
                document = md_xml.toprettyxml(indent='  ')

                copyright_msg = self.copyrights[source_file]
                document = xu.add_copyright_to_xml_str(document, copyright_msg)

            str_by_file[source_file] = document

        return str_by_file

    def to_files(self) -> None:
        '''
        Serialize to disk.
        '''
        new_files = self.to_xml_str(pretty_print=True)
        for file_path, data in new_files.items():
            with open(file_path, 'w', encoding='utf-8') as handle:
                handle.write(data)

    def _load_file(self, source_file: str, data: Optional[str] = None) -> None:
        '''
        Load database entries from a single file.

        Args:
            source_file: File path of the data on disk.
            data: Data payload it has been preloaded.
        '''
        if not data:
            with open(source_file, 'r', encoding='utf-8') as handle:
                data = handle.read()

        node = et.fromstring(data)

        assert node.tag == 'CounterInfoList'

        # Get copyright message so we can emit it on file write
        copyright_msg = xu.get_copyright_from_xml_str(data)
        self.copyrights[source_file] = copyright_msg

        # Load and append counters to the local store
        for child in node:
            info = CounterInfo.from_xml(child, source_file)
            self.append(info)

    @classmethod
    def from_xml_str(cls, file_map: dict[str, str]) -> CounterInfos:
        '''
        Factory method to load the entire database from in-memory XML.

        Args:
            file_map: Data to load, key is file name, value is file payload.

        Returns:
            A new CounterInfos instance.
        '''
        counter_infos = cls()

        for source_file, xml_data in file_map.items():
            counter_infos._load_file(source_file, xml_data)

        return counter_infos

    @classmethod
    def from_files(cls) -> CounterInfos:
        '''
        Factory method to load all counter info from XML files.

        Returns:
            A new CounterInfos instance.
        '''
        # Fetch the data using a script-relative path
        root = pathlib.Path(__file__).parent.parent.parent
        dir_path = root / 'database' / 'counterinfo'
        assert dir_path.exists() and dir_path.is_dir()

        # Ensure we load from XML files in alphabetical order for stability
        database_files = list(dir_path.glob('Mali-CounterInfo-*.xml'))
        database_files.sort()

        counter_infos = cls()
        for file_path in database_files:
            counter_infos._load_file(str(file_path))

        return counter_infos
