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
This module contains the classes to manage per-architecture documentation.

Major classes
=============

An ArchitectureInfos contains the full database, containing documentation
entries for all known architectures and architecture variants.

An ArchitectureInfo contains documentation for a single architecture variant.
Beware that this is not a simple 1:1 mapping with the architecture name, as
we can specialize documentation for different products even if the architecture
name is the same.

Source data
===========

XML database source file is ./database/Mali-ArchitectureInfo.xml
'''

from __future__ import annotations

import pathlib
from typing import Iterator
import xml.dom.minidom as md
import xml.etree.ElementTree as et

from .productinfo import ProductArchitecture as ProdArch
from .. import gpuutils as gu
from .. import xmlutils as xu


class ArchitectureInfo:
    '''
    Architecture documentation.

    Attributes:
        name: Architecture name.
        long_description: Architecture documentation.
        gpu_support: List of GPU database keys that this info applies to.
    '''

    def __init__(self, name: ProdArch, long_description: str):
        '''
        Architecture documentation.

        Attributes:
            name: Architecture name.
            long_description: Architecture documentation.
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
        node = et.SubElement(parent, 'ArchitectureInfo')

        subnode = et.SubElement(node, 'Name')
        subnode.text = self.name.to_xml()

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

        Returns:
            A new architecture info node.
        '''
        # Check this is the correct type of XML node
        assert node.tag == 'ArchitectureInfo'

        # Build the core object with the mandatory attributes
        name = ProdArch.from_xml(xu.get_node_str(node, 'Name'))

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


class ArchitectureInfos:
    '''
    Architecture documentation container.

    Note that there may be multiple different documents for each named
    architecture, applying to different GPUs.

    Attributes:
        copyright: Copyright message we will emit when writing to file.
        architectures: Documentation for each architecture.
    '''

    def __init__(self, copyright_msg: str):
        '''
        Construct a new architecture documentation container.

        Args:
            copyright_msg: Copyright message we will emit when writing to file.
        '''
        self.copyright = copyright_msg
        self.architectures: dict[ProdArch, list[ArchitectureInfo]] = {}

    def get_info_for(self, key: str,
                     architecture: ProdArch) -> ArchitectureInfo:
        '''
        Get the info for a specific GPU.

        Args:
            key: The canonical database GPU name.
            architecture: The product architecture name.

        Returns:
            The info object.

        Raises:
            KeyError if not found.
        '''
        default = None

        info_list = self.architectures.get(architecture, None)
        if not info_list:
            raise KeyError(f'No architecture for {architecture}')

        for info in info_list:
            # Return exact match by preference
            if key in info.gpu_support:
                return info

            # Fall back to default if no exact match
            if not info.gpu_support:
                assert not default, f'Two defaults for {architecture}'
                default = info

        if default is None:
            raise KeyError(f'No default for {key}')

        return default

    def to_xml_str(self, pretty_print: bool = False) -> str:
        '''
        Serialize to XML.

        Args:
            pretty_print: True to pretty print, False otherwise.

        Returns:
           The XML encoded data string.
        '''
        node = et.Element('ArchitectureInfoList')

        for sections in self.architectures.values():
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

    def __iter__(self) -> Iterator[ArchitectureInfo]:
        '''
        Iterate all architectures in the database.

        Yields:
            Architectures in undefined order.
        '''
        for architecture in self.architectures.values():
            yield from architecture

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = len(self.architectures) == len(other.architectures)
        if not self_same:
            return False

        pairs = zip(self.architectures, other.architectures)
        child_same = all(x == y for x, y in pairs)
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

        return dir_path / 'Mali-ArchitectureInfo.xml'

    @classmethod
    def from_xml_str(cls, document: str) -> ArchitectureInfos:
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
        assert node.tag == 'ArchitectureInfoList'

        # Get copyright message so we can emit it on file write
        copyright_msg = xu.get_copyright_from_xml_str(document)

        infos = cls(copyright_msg)

        for child in node:
            info = ArchitectureInfo.from_xml(child)

            if info.name not in infos.architectures:
                infos.architectures[info.name] = []

            infos.architectures[info.name].append(info)

        return infos

    @classmethod
    def from_file(cls) -> ArchitectureInfos:
        '''
        Factory method to create a new instance from an architecture XML file.

        Returns:
            The data structure after parsing from XML.
        '''
        file_path = cls._get_file_path()
        with open(file_path, 'r', encoding='utf-8') as handle:
            data = handle.read()

        return cls.from_xml_str(data)
