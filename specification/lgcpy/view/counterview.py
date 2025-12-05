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
This module contains the CounterView, a compiled view of a single counter that
combines information from multiple underlying databases into a consistent view
that represents the state of the counter for a specific target GPU.
'''

from __future__ import annotations

import enum
from typing import Any, Optional, TYPE_CHECKING

from .. import equationutils as eu

from ..data.productinfo import ProductInfo as PInfo
from ..data.counterinfo import CounterInfo as CInfo
from ..data.counterinfo import CounterVisibility as CVisibility
from ..data.hardwarelayout import HardwareBlockType as HBType
from ..data.hardwarelayout import HardwareBlockLayout as HBLayout
from ..data.hardwarelayout import HardwareCounterLayout as HCLayout

if TYPE_CHECKING:
    from .indexedview import IndexedView


class CounterClockDomain(enum.Enum):
    '''
    Hardware counter clock domain.
    '''
    GPU = 1
    SHADER_CORE = 2


class CounterView:
    '''
    A view onto the counter info, compiled for a specific GPU.

    Attributes:
        block_type: Hardware block type, or None if derived.
        block_index: Hardware block index, or 0 if derived.
        scale_multiplier: Hardware reconstruct multiplier, or 1 if derived.
        machine_name: Canonical machine readable name.
        stable_id: Canonical stable ID.
        source_name: Name of counter in the architecture, or None if derived.
        human_name: Fully qualified human readable name.
        group_name: Fully qualified human readable group name.
        group_human_name: Group qualified human readable name.
        visibility: Visibility level.
        short_description: Short description used for tooltips.
        long_description: Long description used for documentation.
        unit: Units of measurement.
        trend: Desirable trend of measurement.
        equation_text: Textual equation.
        equation_ast: AST equation.
        equation_ast_resolved: AST equation that has been resolved to remove
            any symbolic references to other derived counters.
        equation_ast_resolved_error: Any error that occurrent when trying to
            resolve an AST. This should never happen in release builds.
    '''
    # pylint: disable=too-many-instance-attributes

    def __init__(self, pi: PInfo, hw_name: Optional[str],
                 hw_bl: Optional[HBLayout], hw_cl: Optional[HCLayout],
                 ci: CInfo):
        # ----
        # Counter info from the hardware database
        self.block_type: Optional[HBType] = None
        self.block_index = 0
        self.scale_multiplier = 1

        # These should both be set if hw_name is set but might not if there is
        # a database editing error. This will be detected by the validator, so
        # this should not occur in normal use
        if hw_bl:
            self.block_type = hw_bl.btype

        if hw_cl:
            self.block_index = hw_cl.index
            self.scale_multiplier = 1 << hw_cl.shift

        # ----
        # Determine clock domain

        # Derived counter, so no clock domain
        if not self.block_type:
            self.clock_domain = None
        # Hardware counter on GPU with only 1 clock domain
        elif not pi.has_feature('async_clock'):
            self.clock_domain = CounterClockDomain.GPU
        # Shader counter on GPU with async clock domain
        elif self.block_type == HBType.SHADER_CORE:
            self.clock_domain = CounterClockDomain.SHADER_CORE
        # Any other counter on GPU with async clock domain
        else:
            self.clock_domain = CounterClockDomain.GPU

        # ----
        # Counter info from the counter database
        self.machine_name = ci.machine_name

        # Disallow propagating unassigned stable DIs into a view, even if the
        # underlying database allows it pre-release for easier manual editing
        assert ci.stable_id is not None
        self.stable_id: int = ci.stable_id

        self.human_name = ci.human_name
        self.group_name = ci.group_name
        self.group_human_name = ci.group_human_name
        self.visibility = ci.visibility
        self.short_description = ci.short_description
        self.long_description = ci.long_description
        self.unit = ci.units
        self.trend = ci.trend

        # Handle equations, but we cannot resolve yet so do that later
        self.equation_ast = ci.equation_ast
        self.equation_ast_resolved: Optional[Any] = None
        self.equation_ast_resolved_error: Optional[str] = None

        # Use the passed name which is the specific alias for this GPU
        self.source_name = hw_name

    def resolve_equation(self, index_view: IndexedView) -> None:
        '''
        Resolve the compiled equation to remove derived references.

        Args:
            index_view: The compiled index view for this GPU.
        '''
        if not self.equation_ast:
            return

        result = eu.equation_ast_to_resolved_ast(self.equation_ast, index_view)
        self.equation_ast_resolved = result[0]
        self.equation_ast_resolved_error = result[1]

    def get_anchor(self) -> str:
        '''
        Get a stable HTML anchor name for this counter.

        Stability means that we return same value unless the counter stable ID
        changes, which it should not unless the semantic meaning changes.

        Returns:
            A stable anchor name.
        '''
        return f'c_{self.stable_id}'

    def is_derived(self) -> bool:
        '''
        Is this a derived counter?

        Returns:
            True if derived, False otherwise.
        '''
        return not self.source_name

    def is_visible(self, max_visibility: CVisibility) -> bool:
        '''
        Is this counter visible?

        Args:
            max_visibility: The maximum visibility level allowed.

        Returns:
            True if visible, False otherwise.
        '''
        return self.visibility.value <= max_visibility.value

    def is_novice(self) -> bool:
        '''
        Is this a novice counter?

        Returns:
            True if novice, False otherwise.
        '''
        return self.visibility == CVisibility.NOVICE

    def is_advanced_application(self) -> bool:
        '''
        Is this an advanced counter for application developers?

        Returns:
            True if advanced for application developers, False otherwise.
        '''
        return self.visibility == CVisibility.ADVANCED_APPLICATION

    def is_advanced_system(self) -> bool:
        '''
        Is this an advanced counter for system developers?

        Returns:
            True if advanced for system developers, False otherwise.
        '''
        return self.visibility == CVisibility.ADVANCED_SYSTEM

    def is_internal(self) -> bool:
        '''
        Is this an internal counter for Arm developers?

        Returns:
            True if internal, False otherwise.
        '''
        return self.visibility == CVisibility.INTERNAL
