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
This module contains a data structure hierarchy that contains information
abut GPU product configuration.

Major classes
=============

A ProductInfos contains product information for all of the known products.

A ProductInfo contains product information for a single product. Products are
identified by a numeric GPU ID and human readable name. Beware that this is not
a simple 1:1 mapping, as often the same GPU core can be used in multiple
products with different core counts.

Source data
===========

XML database source file is ./database/Mali-ProductInfo.xml.
'''

from __future__ import annotations

import enum
import pathlib
from typing import Iterator, Optional
import xml.dom.minidom as md
import xml.etree.ElementTree as et

import lgcpy.xmlutils as xu


class ProductVisibility(enum.Enum):
    '''
    Product visibility rules.
    '''
    PUBLIC = 1
    CONFIDENTIAL = 2

    @classmethod
    def from_xml(cls, value: str) -> ProductVisibility:
        '''
        Convert an XML string into an enum value.

        Args:
            value: The XML string value.

        Returns:
            The enum value.
        '''
        if value == 'Public':
            return cls.PUBLIC

        if value == 'Confidential':
            return cls.CONFIDENTIAL

        assert False, f'Unknown enumeration string {value}'

    def to_xml(self) -> str:
        '''
        Serialize to XML.

        Args:
            value: The enum value.

        Returns:
            The XML string value.
        '''
        if self == self.PUBLIC:
            return 'Public'

        if self == self.CONFIDENTIAL:
            return 'Confidential'

        assert False, f'Unknown enumeration value {self.value}'

    def __str__(self) -> str:
        return self.to_xml()


class ProductArchitecture(enum.Enum):
    '''
    Product architecture.
    '''
    BIFROST = 1
    VALHALL = 2
    FIFTH_GENERATION = 3

    @classmethod
    def from_xml(cls, value: str) -> ProductArchitecture:
        '''
        Convert an XML string into an enum value.

        Args:
            value: The XML string value.

        Returns:
            The enum value.
        '''
        if value == 'Bifrost':
            return cls.BIFROST

        if value == 'Valhall':
            return cls.VALHALL

        if value == '5th Generation':
            return cls.FIFTH_GENERATION

        assert False, f'Unknown enumeration string {value}'

    def to_xml(self) -> str:
        '''
        Serialize to XML.

        Args:
            value: The enum value.

        Returns:
            The XML string value.
        '''
        if self == self.BIFROST:
            return 'Bifrost'

        if self == self.VALHALL:
            return 'Valhall'

        if self == self.FIFTH_GENERATION:
            return '5th Generation'

        assert False, f'Unknown enumeration value {self.value}'

    def __str__(self) -> str:
        return self.to_xml()


class ProductInfo():
    '''
    Class representing architecture metadata for a single GPU.

    Attributes:
        ids: List of applicable numeric IDs for this product.
        names: List of applicable string names for this product.
        release_year: Year announced publicly by Arm.
        architecture: Name of the architecture.
        visibility: Is this public or confidential?
        document_name: String name to use in documents, or None if this GPU
          reuses a document for a related GPU
        document_name_indirect: String name to use in documents if this GPU
          reuses a document for another GPU, None if document_name is set.
        database_key: String name to use for the database lookups.
        features: Set of features implemented in this GPU.
        engineering_name: The engineering name of the GPU.
        project_name: The project name of the GPU.
        architecture_branch: The architecture branch to use for specifications.
    '''

    def __init__(self, ids: list[int], names: list[str], release_year: int,
                 architecture: ProductArchitecture,
                 visibility: ProductVisibility):
        '''
        Class representing the configuration of a single GPU.

        Args:
            ids: List of applicable numeric IDs for this product.
            names: List of applicable string names for this product.
            release_year: Year announced publicly by Arm.
            architecture: Name of the architecture.
            visibility: Is this public or confidential?
        '''
        assert len(ids) > 0

        # Mandatory information
        self.ids = ids
        self.names = names
        self.release_year = release_year
        self.architecture = architecture
        self.visibility = visibility

        # Defaults that can be overridden
        self.document_name: Optional[str] = names[0]
        self.document_name_indirect: Optional[str] = None
        self.database_key = names[0]

        # Optional information which may not be in the XML
        self.features: list[str] = []
        self.engineering_name: Optional[str] = None
        self.project_name: Optional[str] = None
        self.architecture_branch: Optional[str] = None

    def is_public(self) -> bool:
        '''
        Test if this product is publicly announced.

        Returns:
            True if public, False if confidential.
        '''
        return self.visibility == ProductVisibility.PUBLIC

    def has_feature(self, feature: str) -> bool:
        '''
        Test if a given feature is available

        Args:
            feature: The feature to query.

        Returns:
            True if feature is implemented, False otherwise.
        '''
        return feature in self.features

    def to_xml(self, parent: et.Element[str]) -> None:
        '''
        Serialize to XML.

        Args:
            parent: Parent XML element to add this node to.
        '''
        node = et.SubElement(parent, 'ProductInfo')

        for gpu_id in self.ids:
            subnode = et.SubElement(node, 'Id')
            subnode.text = f'0x{gpu_id:04x}'

        for name in self.names:
            subnode = et.SubElement(node, 'Name')
            subnode.text = name

        if self.engineering_name:
            subnode = et.SubElement(node, 'EngineeringName')
            subnode.text = self.engineering_name

        if self.project_name:
            subnode = et.SubElement(node, 'ProjectName')
            subnode.text = self.engineering_name

        subnode = et.SubElement(node, 'Architecture')
        subnode.text = self.architecture.to_xml()

        if self.architecture_branch:
            subnode = et.SubElement(node, 'ArchitectureBranch')
            subnode.text = self.architecture_branch

        if self.database_key:
            subnode = et.SubElement(node, 'DatabaseKey')
            subnode.text = self.database_key

        subnode = et.SubElement(node, 'Visibility')
        subnode.text = self.visibility.to_xml()

        subnode = et.SubElement(node, 'ReleaseYear')
        subnode.text = str(self.release_year)

        if self.document_name != self.names[0]:
            subnode = et.SubElement(node, 'DocumentName')
            if not self.document_name:
                subnode.text = 'False'
            else:
                subnode.text = self.document_name

        if self.features:
            featurenode = et.SubElement(node, 'Features')
            for feature in self.features:
                subnode = et.SubElement(featurenode, 'Feature')
                subnode.text = feature

    def get_document_name(self, allow_indirect=False) -> Optional[str]:
        '''
        Get the name of the GPU to use in documentation.

        Args:
            allow_indirect: If True, return name of another GPU that triggered
                the document generation, else only return if this triggered
                the document generation.

        Returns:
            The name of the GPU to use in document generation, or None if
            indirect evaluations were not allowed and this product is not
            a primary document source.
        '''
        if self.document_name:
            return self.document_name

        if allow_indirect:
            return self.document_name_indirect

        return None

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = True
        self_same &= self.ids == other.ids
        self_same &= self.names == other.names
        self_same &= self.architecture == other.architecture
        self_same &= self.visibility == other.visibility
        self_same &= self.document_name == other.document_name
        self_same &= self.database_key == other.database_key
        self_same &= self.features == other.features
        self_same &= self.engineering_name == other.engineering_name
        self_same &= self.project_name == other.project_name
        self_same &= self.architecture_branch == other.architecture_branch

        return self_same

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    @classmethod
    def from_xml(cls, node: et.Element[str]) -> ProductInfo:
        '''
        Factory method to create a new instance from an XML database.

        Args:
            node: The XML root node to parse.

        Returns:
            The data structure after parsing from XML.
        '''
        # Check this is the correct type of XML node
        assert node.tag == 'ProductInfo'

        # Mandatory fields
        ids: list[int] = []
        for subnode in node.findall('Id'):
            assert subnode.text is not None
            ids.append(int(subnode.text, 16))

        names: list[str] = []
        for subnode in node.findall('Name'):
            assert subnode.text is not None
            names.append(subnode.text)

        raw_year = xu.get_node_str(node, 'ReleaseYear')
        year = int(raw_year)

        raw_architecture = xu.get_node_str(node, 'Architecture')
        architecture = ProductArchitecture.from_xml(raw_architecture)

        raw_visibility = xu.get_node_str(node, 'Visibility')
        visibility = ProductVisibility.from_xml(raw_visibility)

        # Build the core info object
        info = ProductInfo(ids, names, year, architecture, visibility)

        # Optional fields that may not exist
        raw_string = xu.get_node_opt_str(node, 'EngineeringName')
        info.engineering_name = raw_string

        raw_string = xu.get_node_opt_str(node, 'ProjectName')
        info.project_name = raw_string

        raw_string = xu.get_node_opt_str(node, 'ArchitectureBranch')
        info.architecture_branch = raw_string

        raw_string = xu.get_node_opt_str(node, 'DatabaseKey')
        if raw_string:
            info.database_key = raw_string

        raw_string = xu.get_node_opt_str(node, 'DocumentName')
        if raw_string:
            if raw_string == 'False':
                info.document_name = None
            else:
                info.document_name = raw_string

        for feature in node.findall('Features/Feature'):
            assert feature.text is not None
            info.features.append(feature.text)

        return info


class ProductInfos():
    '''
    Configuration information for all known products.

    Attributes:
        copyright: Copyright message we will emit when writing to file.
        products: Mapping of all known products.
    '''

    def __init__(self, copyright_msg: str):
        '''
        Construct a new product info container.

        Args:
            copyright_msg: Copyright message we will emit when writing to file.
        '''
        self.copyright = copyright_msg
        self.products: dict[str, ProductInfo] = {}

    def get_gpus(self) -> list[str]:
        '''
        Return list of supported GPUs.

        This function can return product entries for public-facing aliases of
        the same hardware configuration. For example, Mali-G725 and
        Immortalis-G925 are the same underlying product with the same GPU ID,
        and differ only on implementation core count.

        This functions cannot return product entries for database aliases
        which are not public product named. For example, 'Mali G1-Ultra' uses a
        'Mali G1' database key, which is not a publicly visible name.

        Returns:
            List of supported GPU names.
        '''
        return list(self.products.keys())

    def get_gpu(self, name: str) -> ProductInfo:
        '''
        Return a specific product's info.

        This can lookup based on both product name and database key.

        Args:
            name: Name of the product to find.

        Returns:
            The product info.

        Raises:
            KeyError is not found.
        '''
        # Direct lookup
        product = self.products.get(name, None)
        if product:
            return product

        # Indirect for a database key
        for product in self.products.values():
            if product.database_key == name:
                return product

        raise KeyError(f'Unknown GPU product {name}')

    def get_gpu_documentation_primary(self, name: str) -> ProductInfo:
        '''
        Return the product info that is the documentation primary for a
        product. This may be a different product info than get_gpu() returns
        because GPUs can share documents.

        This can lookup based on both product name and database key.

        Args:
            name: Name of the product to find.

        Returns:
            The product info.

        Raises:
            KeyError is not found.
        '''
        # Get the root product and return that if it is a primary source
        product = self.get_gpu(name)
        if product.get_document_name():
            return product

        # Else find the indirect source (there must be one!)
        for indirect_product in self.products.values():
            # Not an alias
            if product.database_key != indirect_product.database_key:
                continue

            if indirect_product.get_document_name():
                return indirect_product

        raise KeyError(f'Unknown GPU product documentation primary {name}')

    def get_aliases_for(self, gpu: str) -> list[str]:
        '''
        Get all known aliases for a GPU product.

        This function finds both marketing aliases for products with the same
        GPU ID and technical aliases for products built from same base
        architecture with different GPU IDs.

        Args:
            gpu: The base name to match.

        Returns:
           List of matching names.
        '''
        base = self.get_gpu(gpu)
        key = base.database_key

        key_matches = [x for x in self if x.database_key == key]

        names = []
        for product in key_matches:
            names.extend(product.names)

        return names

    def to_xml_str(self, pretty_print: bool = False) -> str:
        '''
        Serialize to XML.

        Args:
            pretty_print: True to pretty print, False otherwise.

        Returns:
           The XML encoded data string.
        '''
        node = et.Element('ProductInfoList')

        for name, info in self.products.items():
            # Only serialize the primary name, not additional marketing names
            if name == info.names[0]:
                info.to_xml(node)

        document = et.tostring(node, encoding='unicode')

        if pretty_print:
            xml_md = md.parseString(document)
            document = xml_md.toprettyxml(indent='  ')
            document = xu.add_copyright_to_xml_str(document, self.copyright)

        return document

    def __iter__(self) -> Iterator[ProductInfo]:
        '''
        Iterate all product info instances.

        Yields:
            Product info instances in product order.
        '''
        yield from self.products.values()

    def __eq__(self, other) -> bool:
        if not isinstance(other, self.__class__):
            return False

        self_same = len(self.products) == len(other.products)
        if not self_same:
            return False

        child_same = all(x == y for x, y in zip(self.products, other.products))
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

        return dir_path / 'Mali-ProductInfo.xml'

    @classmethod
    def from_xml_str(cls, document: str) -> ProductInfos:
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
        assert node.tag == 'ProductInfoList'

        # Get copyright message so we can emit it on file write
        copyright_msg = xu.get_copyright_from_xml_str(document)

        infos = cls(copyright_msg)

        # Populate the database entries
        for child in node:
            info = ProductInfo.from_xml(child)
            for name in info.names:
                infos.products[name] = info

        # Setup any indirect document names
        for info in infos.products.values():
            if info.document_name is not None:
                continue

            doc_source = infos.get_gpu_documentation_primary(info.names[0])
            assert doc_source

            info.document_name_indirect = doc_source.document_name

        return infos

    @classmethod
    def from_file(cls) -> ProductInfos:
        '''
        Factory method to create a new instance from an product XML file.

        Returns:
            The data structure after parsing from XML.
        '''
        file_path = cls._get_file_path()
        with open(file_path, 'r', encoding='utf-8') as handle:
            data = handle.read()

        return cls.from_xml_str(data)
