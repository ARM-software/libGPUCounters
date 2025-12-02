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
This module contains tests for the database module.

These tests aim to sense check the implementation of the Python code, and
do not check the validity of the data in the hardware layout database.
'''

import sys
import unittest

from .database import CounterDatabase


class DatabaseTestSuite(unittest.TestCase):
    '''
    Unit tests for the database module.
    '''

    def test_database_gpu_list(self):
        '''
        Test the database can list GPUs.
        '''
        gpu_count = 0
        gpus = CounterDatabase.get_supported_gpus()
        gpu_count = len(gpus)

        print(f'Test loaded {gpu_count} products')
        self.assertGreater(gpu_count, 0)

    def test_database_architecture_info_smoke(self):
        '''
        Test we can load all architecture info.
        '''
        gpus = CounterDatabase.get_supported_gpus()
        for gpu in gpus:
            p_info = CounterDatabase.get_product_info_for(gpu)
            a_info = CounterDatabase.get_architecture_info_for(gpu)
            self.assertEqual(p_info.architecture, a_info.name)

    def test_database_indexed_view_smoke(self):
        '''
        Test we can load all indexed views.
        '''
        counter_count = 0

        gpus = CounterDatabase.get_supported_gpus()
        for gpu in gpus:
            view = CounterDatabase.get_indexed_view_for(gpu)
            counter_count += len(list(view))

        print(f'Test loaded {counter_count} indexed counters')
        self.assertGreater(counter_count, 0)

    def test_database_hardware_view_smoke(self):
        '''
        Test we can load all hardware views.
        '''
        b_count = 0
        c_count = 0

        gpus = CounterDatabase.get_supported_gpus()
        for gpu in gpus:
            view = CounterDatabase.get_hardware_view_for(gpu)
            for block in view:
                b_count += 1
                for _ in block:
                    c_count += 1

        print(f'Test loaded {b_count}.{c_count} hardware counters')
        self.assertGreater(b_count, 0)
        self.assertGreater(c_count, 0)

    def test_database_semantic_view_smoke(self):
        '''
        Test we can load all semantic views.
        '''
        s_count = 0
        g_count = 0
        c_count = 0

        gpus = CounterDatabase.get_supported_gpus()
        for gpu in gpus:
            view = CounterDatabase.get_semantic_view_for(gpu)
            for section in view:
                s_count += 1
                for group in section:
                    g_count += 1
                    for _ in group:
                        c_count += 1

        print(f'Test loaded {s_count}.{g_count}.{c_count} semantic counters')
        self.assertGreater(s_count, 0)
        self.assertGreater(g_count, 0)
        self.assertGreater(c_count, 0)


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
