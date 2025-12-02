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
This module contains tests for the indexedview module.

These tests aim to sense check the implementation of the Python code, and
do not check the validity of the data in the counter info database.
'''

import sys
import unittest

from ..data.productinfo import ProductInfos
from ..data.counterinfo import CounterInfos
from ..data.counterinfo import CounterVisibility as CVisibility
from ..data.hardwarelayout import HardwareLayouts
from .indexedview import IndexedView


class IndexedViewTestSuite(unittest.TestCase):
    '''
    Unit tests for the indexedview module.
    '''

    def test_smoke(self):
        '''
        Test the IndexedView can compile in all variants.
        '''
        pd_db = ProductInfos.from_file()
        gpus = pd_db.get_gpus()

        hw_db = HardwareLayouts.from_files()
        ct_db = CounterInfos.from_files()

        for gpu in gpus:
            pd_info = pd_db.get_gpu(gpu)
            hw_info = hw_db.get_gpu(pd_info.database_key)

            # Build the view
            view = IndexedView.from_db(gpu, pd_info, hw_info, ct_db)
            self.assertIsNotNone(view)

    def test_equation_resolve(self):
        '''
        Test the IndexedView can resolve equations.
        '''
        pd_db = ProductInfos.from_file()
        gpus = pd_db.get_gpus()

        hw_db = HardwareLayouts.from_files()
        ct_db = CounterInfos.from_files()

        for gpu in gpus:
            pd_info = pd_db.get_gpu(gpu)
            hw_info = hw_db.get_gpu(pd_info.database_key)

            # Build the view
            view = IndexedView.from_db(gpu, pd_info, hw_info, ct_db)
            view.resolve_equations()

            count = len(view.by_stable_id)
            print(f'Test unfiltered {gpu} has {count} counters')

    def test_equation_filter_and_resolve(self):
        '''
        Test the IndexedView can resolve equations.
        '''
        pd_db = ProductInfos.from_file()
        gpus = pd_db.get_gpus()

        hw_db = HardwareLayouts.from_files()
        ct_db = CounterInfos.from_files()

        for gpu in gpus:
            pd_info = pd_db.get_gpu(gpu)
            hw_info = hw_db.get_gpu(pd_info.database_key)

            # Build the view - keeping Advanced counters should mean that
            # everything still resolves because public derivations should never
            # refer to Internal counters, so this is a good sanity check
            view = IndexedView.from_db(gpu, pd_info, hw_info, ct_db)
            filtered_view = view.filter(CVisibility.ADVANCED_SYSTEM, True)
            filtered_view.resolve_equations()

            count = len(filtered_view.by_stable_id)
            print(f'Test filtered {gpu} has {count} counters')


def main() -> int:
    '''
    The main function.

    Returns:
        Process return code.
    '''
    results = unittest.main(exit=False)
    return 0 if results.result.wasSuccessful() else 1


if __name__ == '__main__':
    sys.exit(main())
