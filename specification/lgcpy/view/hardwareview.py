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
This module contains the HardwareView, a compiled view of the counters for
a specific GPU that can be iterated based on hardware block memory layout.
'''

from __future__ import annotations

from typing import Iterator

from ..data.counterinfo import CounterVisibility as CVisibility
from ..data.hardwarelayout import HardwareBlockType as HBType
from ..data.hardwarelayout import HardwareBlockLayout as HBLayout
from ..data.hardwarelayout import HardwareLayout as HWLayout

from .counterview import CounterView
from .indexedview import IndexedView


class HardwareBlockView(list[CounterView]):
    '''
    Container for counters in a single memory block for a single GPU.

    Attributes:
        btype: Hardware counter block type.
    '''

    def __init__(self, btype: HBType):
        '''
        Construct a new empty HardwareBlockView.

        Expected to be populated via a factory function.

        Args:
            btype: Hardware block type.
        '''
        super().__init__()
        self.btype = btype

    @classmethod
    def from_db(cls, hw_db: HBLayout,
                i_view: IndexedView) -> HardwareBlockView:
        '''
        Create a hardware memory layout counter view for a single GPU.

        Args:
            hw_db: The hardware layout to use as a template.
            i_view: Pre-compiled index view we can load counters from.

        Returns:
            The compiled view of the block.
        '''
        view = cls(hw_db.btype)

        for hw_cnt in hw_db:
            child_view = i_view.get_by_source_name(hw_cnt.name)
            assert child_view
            view.append(child_view)

        return view


class HardwareView(list[HardwareBlockView]):
    '''
    Container for blocks for a single GPU.
    '''

    def iter_counters(self) -> Iterator[CounterView]:
        '''
        Iterate all counters in the view.

        Yields:
            Counters in hardware order.
        '''
        for block in self:
            yield from block

    def filter(self, max_visibility: CVisibility,
               derived: bool) -> HardwareView:
        '''
        Create a filtered copy of this view to hide counters.

        Args:
            max_visibility: Show up to this visibility level.
            derived: Show derived counters.
        '''
        filtered_view = HardwareView()

        for block in self:
            filtered_block = HardwareBlockView(block.btype)

            for counter in block:
                if not counter.is_visible(max_visibility):
                    continue

                if counter.is_derived() and not derived:
                    continue

                filtered_block.append(counter)

            if filtered_block:
                filtered_view.append(filtered_block)

        return filtered_view

    @classmethod
    def from_db(cls, hw_db: HWLayout, i_view: IndexedView) -> HardwareView:
        '''
        Create a hardware memory layout counter view for a single GPU.

        Args:
            hw_db: The hardware layout to use as a template.
            i_view: Pre-compiled index view we can load counters from.
        '''
        view = cls()

        for hw_bl in hw_db:
            if hw_bl.bank != 0:
                continue

            child_view = HardwareBlockView.from_db(hw_bl, i_view)
            view.append(child_view)

        return view
