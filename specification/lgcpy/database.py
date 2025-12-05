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
This module is a convenience wrapper around all of the sub-databases allowing
easy querying of available products and creation of views without the user
having to manually manage the setup the individual data sources.

The database module caches any created views, making repeated-use much less
expensive. The cache can be reset to release the memory by calling
CounterDatabase.clear_cache().
'''

from __future__ import annotations

from .data.architectureinfo import ArchitectureInfos, ArchitectureInfo
from .data.counterinfo import CounterInfos
from .data.productinfo import ProductInfos, ProductInfo
from .data.hardwarelayout import HardwareLayouts
from .data.semanticinfo import SemanticSectionInfos, SemanticGroupInfos
from .data.semanticlayout import SemanticLayout

from .view.indexedview import IndexedView
from .view.hardwareview import HardwareView
from .view.semanticview import SemanticView


class CounterDatabase:
    '''
    CounterDatabase is a convenience wrapper that exposes simple entrypoints
    to query supported products and create per-product data views.
    '''
    # Component databases loaded from disk
    g_architecture_info_db = ArchitectureInfos.from_file()
    g_counter_info_db = CounterInfos.from_files()
    g_product_info_db = ProductInfos.from_file()
    g_hardware_layout_db = HardwareLayouts.from_files()
    g_semantic_layout_db = SemanticLayout.from_file()
    g_semantic_section_info_db = SemanticSectionInfos.from_file()
    g_semantic_group_info_db = SemanticGroupInfos.from_file()

    # Caches for per-product views
    g_iview_db: dict[str, IndexedView] = {}
    g_hview_db: dict[str, HardwareView] = {}
    g_sview_db: dict[str, SemanticView] = {}

    @classmethod
    def clear_cache(cls) -> None:
        '''
        Clear all generated database views.
        '''
        cls.g_iview_db.clear()
        cls.g_hview_db.clear()
        cls.g_sview_db.clear()

    @classmethod
    def get_indexed_view_for(cls, product_name: str) -> IndexedView:
        '''
        Get the indexed view for a specific GPU.

        Args:
            product_name: GPU to load, can be an alias.

        Returns:
            The created view for the named product.
        '''
        # Cache hit
        if product_name in cls.g_iview_db:
            return cls.g_iview_db[product_name]

        # Cache miss
        pd_info = cls.g_product_info_db.get_gpu(product_name)
        hl_info = cls.g_hardware_layout_db.get_gpu(pd_info.database_key)

        view = IndexedView.from_db(
            product_name, pd_info, hl_info, cls.g_counter_info_db)
        view.resolve_equations()

        # Cache insert
        cls.g_iview_db[product_name] = view

        return view

    @classmethod
    def get_hardware_view_for(cls, product_name: str) -> HardwareView:
        '''
        Get the hardware layout view for a specific GPU.

        Args:
            product_name: GPU to load, can be an alias.

        Returns:
            The created view for the named product.
        '''
        # Cache hit
        if product_name in cls.g_hview_db:
            return cls.g_hview_db[product_name]

        # Cache miss
        pd_info = cls.g_product_info_db.get_gpu(product_name)
        i_view = cls.get_indexed_view_for(product_name)

        hl_info = cls.g_hardware_layout_db.get_gpu(pd_info.database_key)

        view = HardwareView.from_db(hl_info, i_view)

        # Cache insert
        cls.g_hview_db[product_name] = view

        return view

    @classmethod
    def get_semantic_view_for(cls, name: str) -> SemanticView:
        '''
        Get the semantic layout view for a specific GPU.

        Args:
            product_name: GPU to load, can be an alias.

        Returns:
            The created view for the named product.
        '''
        # Cache hit
        if name in cls.g_sview_db:
            return cls.g_sview_db[name]

        # Cache miss
        i_view = cls.get_indexed_view_for(name)
        pd_info = cls.g_product_info_db.get_gpu(name)

        view = SemanticView.from_db(
            pd_info.database_key,
            cls.g_semantic_layout_db,
            cls.g_semantic_section_info_db,
            cls.g_semantic_group_info_db,
            i_view)

        # Cache insert
        cls.g_sview_db[name] = view

        return view

    @classmethod
    def get_supported_gpus(cls) -> list[str]:
        '''
        Get the list of all available GPUs.

        Returns:
            List of all GPUs available in the database, including product
            aliases if the same underlying hardware ships in multiple
            configurations.
        '''
        return cls.g_product_info_db.get_gpus()

    @classmethod
    def get_supported_database_keys(cls) -> list[str]:
        '''
        Get the list of available database keys.

        These keys are deduplicated, with no aliases, and may not be
        consumer-visible product names in their own right. For example
        'Mali G1' is used as the database key for 'Mali G1-Ultra/Premium/Pro',
        but is not a product in its own right.

        This function primarily exists for implementing tests on the underlying
        databases. Application users are expected to use product names.

        Returns:
            List of all supported database keys.
        '''
        keys = []
        for product in cls.g_product_info_db:
            if product.database_key not in keys:
                keys.append(product.database_key)

        return keys

    @classmethod
    def get_architecture_info_for(cls, product_name: str) -> ArchitectureInfo:
        '''
        Get the architecture information for a specific GPU product.

        Args:
            product_name: GPU to load, can be an alias.

        Returns:
            The architecture info of the named product.

        Raises:
            KeyError if not known.
        '''
        product = cls.get_product_info_for(product_name)

        return cls.g_architecture_info_db.get_info_for(
            product.database_key, product.architecture)

    @classmethod
    def get_product_infos(cls) -> ProductInfos:
        '''
        Get all product infos.

        Returns:
            The product infos container for all products.
        '''
        return cls.g_product_info_db

    @classmethod
    def get_product_info_for(cls, product_name: str) -> ProductInfo:
        '''
        Get the product information for a specific GPU product.

        Args:
            product_name: GPU to load, can be an alias.

        Returns:
            The product info of the named product.

        Raises:
            KeyError if not known.
        '''
        return cls.g_product_info_db.get_gpu(product_name)
