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
This module contains tests for the semanticview module.

These tests aim to sense check the implementation of the Python code, and
do not check the validity of the data in the counter info database.
'''

import sys
import unittest

from ..data.productinfo import ProductInfos
from ..data.counterinfo import CounterInfos
from ..data.counterinfo import CounterVisibility as CVisibility
from ..data.hardwarelayout import HardwareLayouts
from ..data.semanticlayout import SemanticLayout
from ..data.semanticinfo import SemanticSectionInfos, SemanticGroupInfos

from .indexedview import IndexedView
from .semanticview import SemanticView


class SemanticViewTestSuite(unittest.TestCase):
    '''
    Unit tests for the semanticview module.
    '''

    REF_GPU = 'Mali-G710'

    def get_sem_view(self) -> SemanticView:
        '''
        Get a standard SemView we can use for testing.
        '''
        gpu = self.REF_GPU

        pd_db = ProductInfos.from_file()
        pd_info = pd_db.get_gpu(gpu)

        hw_db = HardwareLayouts.from_files()
        hw_info = hw_db.get_gpu(pd_info.database_key)

        ct_db = CounterInfos.from_files()

        sem_db = SemanticLayout.from_file()
        sem_is_db = SemanticSectionInfos.from_file()
        sem_ig_db = SemanticGroupInfos.from_file()

        # Build the base view
        ct_view = IndexedView.from_db(gpu, pd_info, hw_info, ct_db)
        self.assertIsNotNone(ct_view)

        # Build the structured view
        sem_view = SemanticView.from_db(
            gpu, sem_db, sem_is_db, sem_ig_db, ct_view)
        self.assertIsNotNone(sem_view)

        return sem_view

    def test_smoke(self):
        '''
        Test the SemanticView can compile in all variants.
        '''
        pd_db = ProductInfos.from_file()
        gpus = pd_db.get_gpus()

        hw_db = HardwareLayouts.from_files()

        ct_db = CounterInfos.from_files()

        sem_db = SemanticLayout.from_file()
        sem_is_db = SemanticSectionInfos.from_file()
        sem_ig_db = SemanticGroupInfos.from_file()

        for gpu in gpus:
            pd_info = pd_db.get_gpu(gpu)
            hw_info = hw_db.get_gpu(pd_info.database_key)

            # Build the base view
            ct_view = IndexedView.from_db(gpu, pd_info, hw_info, ct_db)
            self.assertIsNotNone(ct_view)

            # Build the structured view
            sem_view = SemanticView.from_db(
                gpu, sem_db, sem_is_db, sem_ig_db, ct_view)
            self.assertIsNotNone(sem_view)

    def test_filtering_1(self):
        '''
        Test the SemanticView can be filtered.
        '''
        view = self.get_sem_view()
        filtered_view = view.filter(CVisibility.NOVICE, False)

        for counter in filtered_view.iter_counters():
            self.assertEqual(counter.visibility, CVisibility.NOVICE)
            self.assertFalse(counter.is_derived())

    def test_filtering_2(self):
        '''
        Test the SemanticView can be filtered.
        '''
        view = self.get_sem_view()
        filtered_view = view.filter(CVisibility.ADVANCED_SYSTEM, True)

        for counter in filtered_view.iter_counters():
            self.assertNotEqual(counter.visibility, CVisibility.INTERNAL)


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
