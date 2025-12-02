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
node documentation, that can be used alongside a semantic layout.

The semantic hierarchy we use consists of three levels:

    Sections -> Groups -> Counters

This module provides the database entry handling for supplemental documentation
for sections and groups. Counter documentation comes from the main CounterInfo
instances.

Major classes
=============

A SemanticSectionInfos is a container for section documentation, stored in a
SectionSectionInfo object.

A SemanticGroupInfos is a container for an ordered list of group documentation,
stored in SemanticGroupInfo object.

GPU customization
=================

Both types of Info object can specify a list of GPUs that they apply to,
allowing customization of documentation for a specific product variant. If no
GPU is an exact match then a search will match the first Info entry with no
GPUs specified, which is assumed to be the default documentation.

Source data
===========

XML database source files are ./database/Mali-SemanticSectionInfo.xml and
./database/Mali-SemanticGroupInfo.xml.
'''

from __future__ import annotations

import pathlib
from typing import Iterator
import xml.dom.minidom as md
import xml.etree.ElementTree as et

from .. import gpuutils as gu
from .. import xmlutils as xu


class SemanticGroupInfo:
    '''
    Semantic group documentation.

    Attributes:
        name: Group name.
        long_description: Group documentation.
        gpu_support: List of GPU database keys that this info applies to.
    '''

    def __init__(self, name: str, long_description: str):
        '''
        Semantic group documentation.

        Attributes:
            name: Group name.
            long_description: Group documentation.
        '''
        self.name = name
        self.long_description = long_description
        self.gpu_support: list[str] = []

    def to_xml(self, parent: et.Element[str]) -> None:
        '''
        Serialize to XML.

        Args:
            parent: Parent XML element to add this node to.
        '''
        node = et.SubElement(parent, 'GroupInfo')

        subnode = et.SubElement(node, 'GroupName')
        subnode.text = self.name

        subnode = et.SubElement(node, 'LongDescription')
        subnode.text = xu.to_pretty_xml(self.long_description, True)

        if self.gpu_support:
            subnode = et.SubElement(node, 'SupportedGPUs')
            for gpu in self.gpu_support:
                gpu_node = et.SubElement(subnode, 'GPU')
                gpu_node.text = gpu

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = True
        self_same &= self.name == other.name
        self_same &= self.long_description == other.long_description
        self_same &= self.gpu_support == other.gpu_support

        return self_same

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    @classmethod
    def from_xml(cls, node: et.Element[str]):
        '''
        Factory method to create a new instance from an XML node.

        Args:
            node: XML containing the counter info.
            source_file: Path of the source file on disk.

        Returns:
            A new counter info node.
        '''
        # Check this is the correct type of XML node
        assert node.tag == 'GroupInfo'

        # Build the core object with the mandatory attributes
        name = xu.get_node_str(node, 'GroupName')

        raw_long_desc = xu.get_node_str(node, 'LongDescription')
        long_desc = xu.from_pretty_xml(raw_long_desc, True)

        info = cls(name, long_desc)

        # Assign supported GPU list
        for child_node in node.findall('SupportedGPUs/GPU'):
            assert child_node.text
            info.gpu_support.append(child_node.text)

        # Maintain sorted lists for ease of maintenance
        info.gpu_support = gu.sort_gpus(info.gpu_support)

        return info


class SemanticGroupInfos:
    '''
    Semantic group documentation container.

    Groups names are are required to be globally unique within a GPU, so you
    cannot have different groups with the same name in different sections.

    Attributes:
        copyright: Copyright message we will emit when writing to file.
        groups: Documentation for each group. Note that there may be
            multiple different documents for each named group, applying to
            different GPUs.
    '''

    def __init__(self, copyright_msg: str):
        '''
        Construct a new group documentation container.

        Args:
            copyright_msg: Copyright message we will emit when writing to file.
        '''
        self.copyright = copyright_msg
        self.groups: dict[str, list[SemanticGroupInfo]] = {}

    def get_info_for(self, key: str, group: str) -> SemanticGroupInfo:
        '''
        Get the semantic info for a specific GPU.

        Args:
            key: The canonical database GPU name.
            group: The group name.

        Returns:
            The info object.

        Raises:
            KeyError if not found.
        '''
        default = None

        group_list = self.groups.get(group, None)
        if not group_list:
            raise KeyError(f'No group for {key}.{group}')

        for info in group_list:
            # Return exact match by preference
            if key in info.gpu_support:
                return info

            # Fall back to default if no exact match
            if not info.gpu_support:
                assert not default
                default = info

        if not default:
            raise KeyError(f'No default for {key}.{group}')

        return default

    def to_xml_str(self, pretty_print: bool = False) -> str:
        '''
        Serialize to XML.

        Args:
            pretty_print: True to pretty print, False otherwise.

        Returns:
           The XML encoded data string.
        '''
        node = et.Element('GroupInfoList')

        for groups in self.groups.values():
            for info in groups:
                node.append(et.Comment(' ' + '=' * 68 + ' '))
                info.to_xml(node)

        document = et.tostring(node, encoding='unicode')

        if pretty_print:
            xml_md = md.parseString(document)
            document = xml_md.toprettyxml(indent='  ')
            document = xu.add_copyright_to_xml_str(document, self.copyright)

        return document

    def to_file(self) -> None:
        '''
        Serialize to disk.
        '''
        # Fetch the data using a script-relative path
        file_path = self._get_file_path()

        with open(file_path, 'w', encoding='utf-8') as handle:
            handle.write(self.to_xml_str(pretty_print=True))

    def __iter__(self) -> Iterator[SemanticGroupInfo]:
        '''
        Iterate all groups in the database.

        Yields:
            Groups in undefined order.
        '''
        for groups in self.groups.values():
            yield from groups

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = len(self.groups) == len(other.groups)
        if not self_same:
            return False

        child_same = all(x == y for x, y in zip(self.groups, other.groups))
        return child_same

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    @classmethod
    def _get_file_path(cls) -> pathlib.Path:
        '''
        Get the path of the database file.
        '''
        # Fetch the data using a script-relative path
        root = pathlib.Path(__file__).parent.parent.parent
        dir_path = root / 'database'
        assert dir_path.exists() and dir_path.is_dir()

        return dir_path / 'Mali-SemanticGroupInfo.xml'

    @classmethod
    def from_xml_str(cls, document: str) -> SemanticGroupInfos:
        '''
        Factory method to create a new instance from an XML database.

        Args:
            document: The XML string to parse.

        Returns:
            The data structure after parsing from XML.
        '''
        # Parse the document
        node = et.fromstring(document)

        # Check this is the correct type of XML node
        assert node.tag == 'GroupInfoList'

        # Get copyright message so we can emit it on file write
        copyright_msg = xu.get_copyright_from_xml_str(document)

        infos = cls(copyright_msg)

        for child in node:
            info = SemanticGroupInfo.from_xml(child)

            if info.name not in infos.groups:
                infos.groups[info.name] = []

            infos.groups[info.name].append(info)

        return infos

    @classmethod
    def from_file(cls) -> SemanticGroupInfos:
        '''
        Factory method to create a new instance from an product XML file.

        Returns:
            The data structure after parsing from XML.
        '''
        file_path = cls._get_file_path()
        with open(file_path, 'r', encoding='utf-8') as handle:
            data = handle.read()

        return cls.from_xml_str(data)


class SemanticSectionInfo:
    '''
    Semantic section documentation.

    Attributes:
        name: Section name.
        long_description: Section documentation.
        gpu_support: List of GPU database keys that this info applies to.
    '''

    def __init__(self, name: str, long_description: str):
        '''
        Semantic section documentation.

        Attributes:
            name: Section name.
            long_description: Section documentation.
        '''
        self.name = name
        self.long_description = long_description
        self.gpu_support: list[str] = []

    def to_xml(self, parent: et.Element[str]) -> None:
        '''
        Serialize to XML.

        Args:
            parent: Parent XML element to add this node to.
        '''
        node = et.SubElement(parent, 'SectionInfo')

        subnode = et.SubElement(node, 'SectionName')
        subnode.text = self.name

        subnode = et.SubElement(node, 'LongDescription')
        subnode.text = xu.to_pretty_xml(self.long_description, True)

        if self.gpu_support:
            subnode = et.SubElement(node, 'SupportedGPUs')
            for gpu in self.gpu_support:
                gpu_node = et.SubElement(subnode, 'GPU')
                gpu_node.text = gpu

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = True
        self_same &= self.name == other.name
        self_same &= self.long_description == other.long_description
        self_same &= self.gpu_support == other.gpu_support

        return self_same

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    @classmethod
    def from_xml(cls, node: et.Element[str]):
        '''
        Factory method to create a new instance from an XML node.

        Args:
            node: XML containing the counter info.
            source_file: Path of the source file on disk.

        Returns:
            A new counter info node.
        '''
        # Check this is the correct type of XML node
        assert node.tag == 'SectionInfo'

        # Build the core object with the mandatory attributes
        name = xu.get_node_str(node, 'SectionName')

        raw_long_desc = xu.get_node_str(node, 'LongDescription')
        long_desc = xu.from_pretty_xml(raw_long_desc, True)

        info = cls(name, long_desc)

        # Assign supported GPU list
        for child_node in node.findall('SupportedGPUs/GPU'):
            assert child_node.text
            info.gpu_support.append(child_node.text)

        # Maintain sorted lists for ease of maintenance
        info.gpu_support = gu.sort_gpus(info.gpu_support)

        return info


class SemanticSectionInfos:
    '''
    Semantic section documentation container.

    Attributes:
        copyright: Copyright message we will emit when writing to file.
        sections: Documentation for each section. Note that there may be
            multiple different documents for each named section, applying to
            different GPUs.
    '''

    def __init__(self, copyright_msg: str):
        '''
        Construct a new section documentation container.

        Args:
            copyright_msg Copyright message we will emit when writing to file.
        '''
        self.copyright = copyright_msg
        self.sections: dict[str, list[SemanticSectionInfo]] = {}

    def get_info_for(self, key: str, section: str) -> SemanticSectionInfo:
        '''
        Get the semantic info for a specific GPU.

        Args:
            key: The canonical database GPU name.
            section: The section name.

        Returns:
            The info object.

        Raises:
            KeyError if not found.
        '''
        default = None

        section_list = self.sections.get(section, None)
        if not section_list:
            raise KeyError(f'No section for {key}.{section}')

        for info in section_list:
            # Return exact match by preference
            if key in info.gpu_support:
                return info

            # Fall back to default if no exact match
            if not info.gpu_support:
                assert not default, f'Two defaults for {section}'
                default = info

        if not default:
            raise KeyError(f'No default for {key}.{section}')

        return default

    def to_xml_str(self, pretty_print: bool = False) -> str:
        '''
        Serialize to XML.

        Args:
            pretty_print: True to pretty print, False otherwise.

        Returns:
           The XML encoded data string.
        '''
        node = et.Element('SectionInfoList')

        for sections in self.sections.values():
            for info in sections:
                node.append(et.Comment(' ' + '=' * 68 + ' '))
                info.to_xml(node)

        document = et.tostring(node, encoding='unicode')

        if pretty_print:
            xml_md = md.parseString(document)
            document = xml_md.toprettyxml(indent='  ')
            document = xu.add_copyright_to_xml_str(document, self.copyright)

        return document

    def to_file(self) -> None:
        '''
        Serialize to disk.
        '''
        # Fetch the data using a script-relative path
        file_path = self._get_file_path()

        with open(file_path, 'w', encoding='utf-8') as handle:
            handle.write(self.to_xml_str(pretty_print=True))

    def __iter__(self) -> Iterator[SemanticSectionInfo]:
        '''
        Iterate all sections in the database.

        Yields:
            Sections in undefined order.
        '''
        for sections in self.sections.values():
            yield from sections

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = len(self.sections) == len(other.sections)
        if not self_same:
            return False

        child_same = all(x == y for x, y in zip(self.sections, other.sections))
        return child_same

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    @classmethod
    def _get_file_path(cls) -> pathlib.Path:
        '''
        Get the path of the database file.
        '''
        # Fetch the data using a script-relative path
        root = pathlib.Path(__file__).parent.parent.parent
        dir_path = root / 'database'
        assert dir_path.exists() and dir_path.is_dir()

        return dir_path / 'Mali-SemanticSectionInfo.xml'

    @classmethod
    def from_xml_str(cls, document: str) -> SemanticSectionInfos:
        '''
        Factory method to create a new instance from an XML database.

        Args:
            document: The XML string to parse.

        Returns:
            The data structure after parsing from XML.
        '''
        # Parse the document
        node = et.fromstring(document)

        # Check this is the correct type of XML node
        assert node.tag == 'SectionInfoList'

        # Get copyright message so we can emit it on file write
        copyright_msg = xu.get_copyright_from_xml_str(document)

        infos = cls(copyright_msg)

        for child in node:
            info = SemanticSectionInfo.from_xml(child)

            if info.name not in infos.sections:
                infos.sections[info.name] = []

            infos.sections[info.name].append(info)

        return infos

    @classmethod
    def from_file(cls) -> SemanticSectionInfos:
        '''
        Factory method to create a new instance from an product XML file.

        Returns:
            The data structure after parsing from XML.
        '''
        file_path = cls._get_file_path()
        with open(file_path, 'r', encoding='utf-8') as handle:
            data = handle.read()

        return cls.from_xml_str(data)
