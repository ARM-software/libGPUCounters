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
Convenience exports for the publicly visible classes that users need to use
directly when loading views from the database, including use when adding
Python typing to user scripts.
'''
# pylint: disable=unused-import
from .database import CounterDatabase

from .data.architectureinfo import ArchitectureInfo, ArchitectureInfos

from .data.counterinfo import CounterInfos, CounterInfo, CounterTrend, \
                              CounterVisibility

from .data.productinfo import ProductInfos, ProductInfo, ProductVisibility, \
                              ProductArchitecture

from .data.hardwarelayout import HardwareLayouts, HardwareLayout, \
                                 HardwareBlockType, HardwareLookup

from .data.semanticlayout import SemanticLayout

from .data.semanticinfo import SemanticSectionInfos, SemanticSectionInfo, \
                               SemanticGroupInfos, SemanticGroupInfo

from .view.counterview import CounterView, CounterClockDomain

from .view.hardwareview import HardwareView, HardwareBlockView

from .view.indexedview import IndexedView

from .view.semanticview import SemanticView, SemanticSectionView, \
                               SemanticGroupView
