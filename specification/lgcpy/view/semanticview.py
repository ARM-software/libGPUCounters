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
This module contains the SemanticView, a compiled view of the counters for
a specific GPU that can be iterated based on semantic reading layout.

The semantic hierarchy we use consists of three levels:

    Sections -> Groups -> Counters

This layout is intended to capture a recommended presentation ordering.
Sections are analogous to topical areas, Groups are analogous to a chart, and
Counters can be presented as data series on that chart.
'''
from __future__ import annotations

import string
from typing import Iterator

from ..data.counterinfo import CounterVisibility as CVisibility
from ..data.semanticlayout import SemanticLayout as SemLayout
from ..data.semanticlayout import SemanticSectionLayout as SemSecLayout
from ..data.semanticlayout import SemanticGroupLayout as SemGrpLayout
from ..data.semanticinfo import SemanticSectionInfos as SemSecInfos
from ..data.semanticinfo import SemanticGroupInfos as SemGrpInfos

from .counterview import CounterView
from .indexedview import IndexedView


class SemanticGroupView:
    '''
    Semantic group view, consisting of 1 or more counters.

    Attributes:
        name: Counter database GroupName.
        long_description: Group documentation.
        counters: Ordered map of counters.
    '''

    def __init__(self, name: str, long_description: str):
        '''
        Construct a new empty SemanticGroupView.

        Expected to be populated via a factory function.

        Args:
            name: Group name.
            long_description: Group documentation.
        '''
        self.name = name
        self.long_description = long_description
        self.counters: dict[str, CounterView] = {}

    def __iter__(self) -> Iterator[CounterView]:
        '''
        Iterate all counters in this group.

        Yields:
            Counters in presentation order.
        '''
        yield from self.counters.values()

    def get_anchor(self) -> str:
        '''
        Get a stable HTML anchor name for this group.

        Stability means that we return same value unless the group name
        changes.

        Returns:
            A stable anchor name.
        '''
        valid_chars = string.ascii_lowercase + string.digits

        anchor = self.name.lower()
        anchor = ''.join(x for x in anchor if x in valid_chars)
        return f'g_{anchor}'

    @classmethod
    def from_db(cls, gpu: str, sem_db: SemGrpLayout, grp_db: SemGrpInfos,
                i_view: IndexedView) -> SemanticGroupView:
        '''
        Create a semantic counter memory layout for a single GPU.

        Args:
            gpu: GPU database key to generate the view for.
            sem_db: The unfiltered semantic layout to use as a data source.
            grp_db: The unfiltered group into to use as a data source.
            i_view: Pre-compiled index view we can load counters from.

        Raises:
            KeyError if GPU or group is not found.
        '''
        info = grp_db.get_info_for(gpu, sem_db.name)

        sem_view = cls(sem_db.name, info.long_description)

        for counter in sem_db:
            child_view = i_view.get_by_group_names(
                sem_view.name, counter.name)

            # Only keep counters that exist!
            if child_view:
                sem_view.counters[counter.name] = child_view

        return sem_view


class SemanticSectionView:
    '''
    Semantic section view, consisting of 1 or more groups.

    Attributes:
        name: Name of the section name.
        long_description: Section documentation.
        groups: Ordered map of groups.
    '''

    def __init__(self, name: str, long_description: str):
        '''
        Construct a new empty SemanticSectionView.

        Expected to be populated via a factory function.

        Args:
            name: Name of the section.
            long_description: Section documentation.
        '''
        self.name = name
        self.long_description = long_description
        self.groups: dict[str, SemanticGroupView] = {}

    def __iter__(self) -> Iterator[SemanticGroupView]:
        '''
        Iterate all groups in this section.

        Yields:
            Counter groups in presentation order.
        '''
        yield from self.groups.values()

    def get_anchor(self) -> str:
        '''
        Get a stable HTML anchor name for this section.

        Stability means that we return same value unless the section name
        changes.

        Returns:
            A stable anchor name.
        '''
        valid_chars = string.ascii_lowercase + string.digits

        anchor = self.name.lower()
        anchor = ''.join(x for x in anchor if x in valid_chars)
        return f's_{anchor}'

    @classmethod
    def from_db(cls, key: str, sem_db: SemSecLayout,
                sec_db: SemSecInfos, grp_db: SemGrpInfos,
                i_view: IndexedView) -> SemanticSectionView:
        '''
        Create a semantic counter memory layout for a single GPU.

        Args:
            key: The canonical database GPU name.
            sem_db: The unfiltered semantic layout to use as a template.
            sec_db: Section information database.
            grp_db: Group information database.
            i_view: Pre-compiled index view we can load counters from.

        Raises:
            KeyError if GPU is not found.
        '''
        info = sec_db.get_info_for(key, sem_db.name)

        sem_view = cls(sem_db.name, info.long_description)

        # Parse the groups and associated counters
        for group in sem_db:
            try:
                child_view = SemanticGroupView.from_db(
                    key, group, grp_db, i_view)

                # Only keep groups that have counters for this GPU
                if len(child_view.counters):
                    sem_view.groups[group.name] = child_view

            # Group had no match for this GPU
            except KeyError:
                continue

        return sem_view


class SemanticView:
    '''
    Container for blocks for a single GPU.

    Attributes:
        sections: Ordered map of sections.
    '''

    def __init__(self):
        '''
        Construct a new empty SemanticView.

        Expected to be populated via a factory function.
        '''
        self.sections: dict[str, SemanticSectionView] = {}

    def iter_sections(self) -> Iterator[SemanticSectionView]:
        '''
        Iterate all sections in the view.

        Yields:
            Sections in presentation order.
        '''
        yield from self.sections.values()

    def iter_groups(self) -> Iterator[SemanticGroupView]:
        '''
        Iterate all groups in the view.

        Yields:
            Groups in presentation order.
        '''
        for section in self.sections.values():
            yield from section.groups.values()

    def iter_counters(self) -> Iterator[CounterView]:
        '''
        Iterate all counters in the view.

        Yields:
            Counters in presentation order.
        '''
        for section in self.sections.values():
            for group in section.groups.values():
                yield from group.counters.values()

    def filter(self, max_visibility: CVisibility,
               derived: bool) -> SemanticView:
        '''
        Create a filtered copy of this view to hide counters.

        Args:
            max_visibility: Show up to this visibility level.
            derived: Show derived counters.
        '''
        flt_view = SemanticView()

        for sec_name, sec in self.sections.items():
            flt_sec = SemanticSectionView(sec_name, sec.long_description)

            for grp_name, grp in sec.groups.items():
                flt_grp = SemanticGroupView(grp_name, grp.long_description)

                for cnt_name, cnt in grp.counters.items():
                    if not cnt.is_visible(max_visibility):
                        continue

                    if cnt.is_derived() and not derived:
                        continue

                    # Keep counter if it passed
                    flt_grp.counters[cnt_name] = cnt

                # Keep group if it contains any counters after filtering
                if flt_grp.counters:
                    flt_sec.groups[grp_name] = flt_grp

            # Keep section if it contains any groups after filtering
            if flt_sec.groups:
                flt_view.sections[sec_name] = flt_sec

        return flt_view

    def __iter__(self) -> Iterator[SemanticSectionView]:
        '''
        Iterate all sections in the view.

        Yields:
            Counter sections in presentation order.
        '''
        yield from self.sections.values()

    @classmethod
    def from_db(cls, key: str, sem_db: SemLayout, sec_db: SemSecInfos,
                grp_db: SemGrpInfos, i_view: IndexedView) -> SemanticView:
        '''
        Create a semantic counter memory layout for a single GPU.

        Args:
            key: The canonical database GPU name.
            sem_db: Unfiltered semantic layout to use as a template.
            sec_db: Section documentation database.
            grp_db: Group documentation database.
            i_view: Pre-compiled index view we can load counters from.
        '''
        sem_view = cls()

        # Parse the sections and associated groups
        for section in sem_db:
            try:
                child_view = SemanticSectionView.from_db(
                    key, section, sec_db, grp_db, i_view)

                # Only keep sections that have groups for this GPU
                if len(child_view.groups):
                    sem_view.sections[section.name] = child_view

            # Section had no match for this GPU
            except KeyError:
                continue

        return sem_view
