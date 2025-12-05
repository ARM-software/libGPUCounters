#
# Copyright (c) 2025 Arm Limited.
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
This module contains a data structure hierarchy that represents the semantic
presentation hierarchy for all performance counters. This can be used to
determine how to structure documentation, or how to group sets of counters for
presentation in a tool.

The semantic hierarchy we use consists of three levels:

    Sections -> Groups -> Counters

Section names and Group names must be globally unique in the entire database,
as both can be used in isolation and must be unambiguous. Counter names must
be unique within the scope of their local group, but are intended to be
short because their parent group name gives the necessary context.

The raw database contains a complete hierarchy that is the union of all
sections, groups and counters from all supported GPUs. The hierarchy can be
filtered by a SemanticView for a single GPU product, hiding the sections,
groups, and counters that are not relevant for that product.

Purpose
=======

The semantic layout is intended to capture recommended presentation ordering
for Arm GPU counters. Groups are analogous to a chart, and Counters are series
on the chart.

Major classes
=============

A SemanticLayout is a container for an ordered list of sections.

A SemanticSectionLayout is a container for an ordered list of sections.
Sections are a purely semantic construct that is only defined in this database.
When compiling a data view for a specific GPU, sections can be associated with
a set of rendered documentation describing the behavior of that section for
that specific GPU.

A SemanticGroupLayout is a container for an ordered list of counters. Groups
are identified by their GroupName, as defined in the main counter database.
When compiling a data view for a specific GPU, groups can be associated with a
set of rendered documentation describing the behavior of that group for that
specific GPU

A SemanticCounterLayout is a container for a single counter. Counters are
identified by their GroupHumanName, as defined in the main counter database.

Source data
===========

YAML database source file is ./database/Mali-SemanticLayout.yaml.
'''

from __future__ import annotations

import pathlib
from typing import Iterator
import yaml

from .. import xmlutils as xu


class SemanticCounterLayout:
    '''
    Semantic configuration of a single counter.

    Attributes:
        name: Counter database GroupHumanName.
    '''

    def __init__(self, name: str):
        '''
        Construct a new counter.

        Args:
            name: Counter database GroupHumanName.
        '''
        self.name = name

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = self.name == other.name
        return self_same

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    def __str__(self) -> str:
        return self.name


class SemanticGroupLayout:
    '''
    Semantic configuration of a counter group, with one or more counters.

    Attributes:
        name: Counter database GroupName.
        counters: Ordered map of counters.
    '''

    def __init__(self, name: str):
        '''
        Construct a new group.

        Args:
            name: Counter database GroupHumanName.
        '''
        self.name = name
        self.counters: dict[str, SemanticCounterLayout] = {}

    def append(self, counter: SemanticCounterLayout) -> None:
        '''
        Append a new counter to this group.

        Args:
            counter: Counter to add.
        '''
        assert counter.name not in self.counters, \
               f'Duplicate semantic counter: {counter.name}'
        self.counters[counter.name] = counter

    def __iter__(self) -> Iterator[SemanticCounterLayout]:
        '''
        Iterate all counters in this group.

        Yields:
            Counters in presentation order.
        '''
        yield from self.counters.values()

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = True
        self_same &= self.name == other.name
        self_same &= len(self.counters) == len(other.counters)

        if not self_same:
            return False

        # No need to check keys as they repeat value.name for random access
        x_children = self.counters.values()
        y_children = other.counters.values()
        child_same = all(x == y for x, y in zip(x_children, y_children))
        return child_same

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    def __str__(self) -> str:
        parts = [f'{self.name}']

        for counter in self:
            parts.append(f'  - {counter.name}')

        return '\n'.join(parts)


class SemanticSectionLayout:
    '''
    Semantic configuration of a counter section, with one or more groups.

    Attributes:
        name: Section name.
        groups: Ordered map of groups.
    '''

    def __init__(self, name: str):
        '''
        Construct a new section.

        Args:
            name: Section name.
        '''
        self.name = name
        self.groups: dict[str, SemanticGroupLayout] = {}

    def append(self, group: SemanticGroupLayout) -> None:
        '''
        Append a new group to this section.

        Args:
            group: Group to add.
        '''
        assert group.name not in self.groups, \
               f'Duplicate semantic group: {group.name}'
        self.groups[group.name] = group

    def __iter__(self) -> Iterator[SemanticGroupLayout]:
        '''
        Iterate all groups in this section.

        Yields:
            Counter groups in presentation order.
        '''
        yield from self.groups.values()

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = True
        self_same &= self.name == other.name
        self_same &= len(self.groups) == len(other.groups)

        if not self_same:
            return False

        # No need to check keys as they repeat value.name for random access
        x_children = self.groups.values()
        y_children = other.groups.values()
        child_same = all(x == y for x, y in zip(x_children, y_children))
        return child_same

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    def __str__(self) -> str:
        parts = [f'{self.name}']

        for group in self:
            parts.append(f'  - {group.name}')
            for counter in group:
                parts.append(f'    - {counter.name}')

        return '\n'.join(parts)


class SemanticLayout:
    '''
    Semantic configuration database.

    Attributes:
        copyright: Copyright message we will emit when writing to file.
        sections: Ordered map of sections.
    '''

    def __init__(self, copyright_msg: str):
        '''
        Construct a new database.

        Args:
            copyright_msg: Copyright message we will emit when writing to file.
        '''
        self.copyright = copyright_msg
        self.sections: dict[str, SemanticSectionLayout] = {}

    def append(self, section: SemanticSectionLayout) -> None:
        '''
        Append a new section to this database.

        Args:
            section: Section to add.
        '''
        assert section.name not in self.sections, \
               f'Duplicate semantic section: {section.name}'
        self.sections[section.name] = section

    def iter_sections(self) -> Iterator[SemanticSectionLayout]:
        '''
        Iterate all sections in the layout.

        Yields:
            Sections in presentation order.
        '''
        yield from self.sections.values()

    def iter_groups(self) -> Iterator[SemanticGroupLayout]:
        '''
        Iterate all groups in the layout.

        Yields:
            Groups in presentation order.
        '''
        for section in self.sections.values():
            yield from section.groups.values()

    def to_yaml_str(self) -> str:
        '''
        Serialize to YAML.

        Returns:
           The YAML encoded data.
        '''
        parts = [
            self.copyright,
            '---'
        ]

        for section in self:
            parts.append(f'- {section.name}:')
            for group in section:
                parts.append(f'  - {group.name}:')
                for counter in group:
                    parts.append(f'    - {counter.name}')
            parts.append('')

        string_form = '\n'.join(parts)
        return string_form

    def to_file(self) -> None:
        '''
        Serialize to disk.
        '''
        # Fetch the data using a script-relative path
        file_path = self._get_file_path()

        with open(file_path, 'w', encoding='utf-8') as handle:
            handle.write(self.to_yaml_str())

    def __iter__(self) -> Iterator[SemanticSectionLayout]:
        '''
        Iterate all sections in the database.

        Yields:
            Counter sections in presentation order.
        '''
        yield from self.sections.values()

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = len(self.sections) == len(other.sections)
        if not self_same:
            return False

        # No need to check keys as they repeat value.name for random access
        x_children = self.sections.values()
        y_children = other.sections.values()
        child_same = all(x == y for x, y in zip(x_children, y_children))
        return child_same

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    def __str__(self) -> str:
        parts = [str(x) for x in self.sections]
        return '\n'.join(parts)

    @classmethod
    def _get_file_path(cls) -> pathlib.Path:
        '''
        Get the path of the database file.
        '''
        # Fetch the data using a script-relative path
        root = pathlib.Path(__file__).parent.parent.parent
        dir_path = root / 'database'
        assert dir_path.exists() and dir_path.is_dir()

        return dir_path / 'Mali-SemanticLayout.yaml'

    @classmethod
    def from_yaml_str(cls, document: str) -> SemanticLayout:
        '''
        Factory method to create a new instance from a YAML string.

        Args:
            document: Database string.

        Returns:
            New semantic hierarchy instance.
        '''
        # YAML dicts are not ordered, so we have list of single item dicts
        # storing the hierarchy name and the list of sub-items in them
        root_hierarchy = yaml.safe_load(document)

        copyright_msg = xu.get_copyright_from_yaml_str(document)

        database = cls(copyright_msg)

        for section_entry in root_hierarchy:
            assert len(section_entry) == 1
            section_name = list(section_entry.keys())[0]
            groups = section_entry[section_name]

            section_layout = SemanticSectionLayout(section_name)
            database.append(section_layout)

            for group_entry in groups:
                assert len(group_entry) == 1
                group_name, group_counters = list(group_entry.items())[0]

                group_layout = SemanticGroupLayout(group_name)
                section_layout.append(group_layout)

                for counter_name in group_counters:
                    counter_layout = SemanticCounterLayout(counter_name)
                    group_layout.append(counter_layout)

        return database

    @classmethod
    def from_file(cls) -> SemanticLayout:
        '''
        Factory method to create a new instance from the standard YAML file.

        Returns:
            New semantic hierarchy instance.
        '''
        file_path = cls._get_file_path()
        with open(file_path, encoding='utf-8') as handle:
            yaml_string = handle.read()

        return cls.from_yaml_str(yaml_string)
