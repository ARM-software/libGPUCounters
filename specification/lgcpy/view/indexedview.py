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
This module contains the IndexView, a compiled view that combines information
from multiple underlying databases into a view of the counters for a specific
target GPU.

This view allows a caller to efficiently randomly select counters based on a
counter identifier, such as its Stable ID or Machine Name. Iteration over all
counters is possible, but is unstructured.
'''

from __future__ import annotations

from typing import Iterator, Optional

from ..data.productinfo import ProductInfo as PInfo
from ..data.counterinfo import CounterVisibility as CVisibility
from ..data.counterinfo import CounterInfos as CInfos
from ..data.hardwarelayout import HardwareLayout as HWLayout

from .counterview import CounterView


class IndexedView:
    '''
    Container class for the compiled counter database for a specific GPU.

    This view provides indexed lookups and unstructured iteration. Structured
    views are accessible via the hardwareview or semanticview wrappers.

    Attributes:
        gpu: The product GPU name, which can be an alias.
        key: The canonical database key GPU name.
        by_stable_id: Map of counters indexed by Stable ID.
        by_machine_name: Map of counters indexed by Machine Name.
        by_source_name: Map of counters indexed by Source Name. This map only
            contains hardware counters, as derived counters have no source.
        by_human_name: Map of counters indexed by Human Name.
        by_groups_names: Map of counters indexed by Group Name and Group
            Human Name.
    '''

    def __init__(self, gpu: str, key: str):
        '''
        Construct a new empty IndexedView.

        Expected to be populated via a factory function.

        Args:
            gpu: The product name, which can be an alias.
            key: The canonical database GPU name.
        '''
        self.gpu = gpu
        self.key = key

        # Indexes to allow random access lookups
        self.by_stable_id: dict[int, CounterView] = {}
        self.by_machine_name: dict[str, CounterView] = {}
        self.by_source_name: dict[str, CounterView] = {}
        self.by_human_name: dict[str, CounterView] = {}
        self.by_group_names: dict[str, CounterView] = {}

    def resolve_equations(self) -> None:
        '''
        Eagerly resolve all resolved equations ahead of time.
        '''
        for counter in self.by_stable_id.values():
            counter.resolve_equation(self)

    def get_by_stable_id(self, key: int) -> Optional[CounterView]:
        '''
        Get a counter by database stable ID.

        Args:
            key: Stable identifier index.

        Returns:
            Counter view if found, None otherwise.
        '''
        return self.by_stable_id.get(key, None)

    def get_by_machine_name(self, key: str) -> Optional[CounterView]:
        '''
        Get a counter by database machine name.

        Args:
            key: Machine name index.

        Returns:
            Counter view if found, None otherwise.
        '''
        return self.by_machine_name.get(key.lower(), None)

    def get_by_source_name(self, key: str) -> Optional[CounterView]:
        '''
        Get a counter by database source name.

        Note that only native hardware counters are accessible via this
        mechanism, derived counters do not have a source name.

        Args:
            key: Source name index.

        Returns:
            Counter view if found, None otherwise.
        '''
        return self.by_source_name.get(key.lower(), None)

    def get_by_human_name(self, key: str) -> Optional[CounterView]:
        '''
        Get a counter by database human name.

        Args:
            key: Human name index.

        Returns:
            Counter view if found, None otherwise.
        '''
        return self.by_human_name.get(key.lower(), None)

    def get_by_group_names(self, group_name: str,
                           group_human_name: str) -> Optional[CounterView]:
        '''
        Get a counter by group name.

        Args:
            group: Counter group name.
            counter: Counter group human name.

        Returns:
            Counter view if found, None otherwise.
        '''
        group_index = f'{group_name}|{group_human_name}'
        return self.by_group_names.get(group_index.lower(), None)

    def __iter__(self) -> Iterator[CounterView]:
        '''
        Iterate all counters for this GPU in an unstructured way.

        Yields:
            Counter series in arbitrary order.
        '''
        yield from self.by_machine_name.values()

    def filter(self, max_visibility: CVisibility,
               derived: bool) -> IndexedView:
        '''
        Create a filtered copy of this view to hide counters.

        Args:
            max_visibility: Show up to this visibility level.
            derived: Show derived counters.
        '''
        filtered_view = IndexedView(self.gpu, self.key)

        # Iterate by Stable ID as all counters are in that map
        for counter in self.by_stable_id.values():
            if not counter.is_visible(max_visibility):
                continue

            if counter.is_derived() and not derived:
                continue

            # Add the indexes
            filtered_view.by_stable_id[counter.stable_id] = counter

            key = counter.machine_name.lower()
            filtered_view.by_machine_name[key] = counter

            if counter.source_name:
                key = counter.source_name.lower()
                filtered_view.by_source_name[key] = counter

            key = counter.human_name.lower()
            filtered_view.by_human_name[key] = counter

            group_index = f'{counter.group_name}|{counter.group_human_name}'
            key = group_index.lower()
            filtered_view.by_group_names[key] = counter

        return filtered_view

    @classmethod
    def from_db(cls, product: str, pi: PInfo, hw_db: HWLayout,
                ct_db: CInfos) -> IndexedView:
        '''
        Create an indexed counter memory layout for a single GPU.

        Args:
            product: Product name for this view, which can be an alias.
            pi: Product info for this GPU.
            hw_db: Hardware layout database.
            ct_db: Counter database.
        '''
        view = cls(product, pi.database_key)

        for counter in ct_db:
            # Skip counters that don't apply to this GPU
            if not counter.supports_gpu(view.key):
                continue

            # Find the matching hardware layout entries which could be using
            # a name alias
            hw_bl = None
            hw_cl = None
            hw_name = None

            # TODO: Store the alias per GPU in the CounterInfo XML, avoiding
            # potential ambiguity and the need to search here
            if counter.source_name:
                source_names = [counter.source_name]
                source_names.extend(counter.source_name_aliases)
                for name in source_names:
                    hw_data = hw_db.get_counter_by_name(name)
                    if hw_data:
                        hw_name = name
                        hw_bl, hw_cl = hw_data
                        break
                else:
                    # This is an error, but picked up by validator
                    hw_name = counter.source_name
                    hw_bl = None

            # Build the view
            ct_view = CounterView(pi, hw_name, hw_bl, hw_cl, counter)

            # Add the indexes
            view.by_stable_id[ct_view.stable_id] = ct_view

            key = ct_view.machine_name.lower()
            view.by_machine_name[key] = ct_view

            if ct_view.source_name:
                key = ct_view.source_name.lower()
                view.by_source_name[key] = ct_view

            key = ct_view.human_name.lower()
            view.by_human_name[key] = ct_view

            group_index = f'{ct_view.group_name}|{ct_view.group_human_name}'
            key = group_index.lower()
            view.by_group_names[key] = ct_view

        return view
