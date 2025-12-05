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
This module contains tests for the productinfo module.

These tests aim to sense check the implementation of the Python code, and
do not check the validity of the data in the product info database.
'''

import sys
import unittest

from .productinfo import ProductVisibility, ProductInfos


class ProductInfoTestSuite(unittest.TestCase):
    '''
    Unit tests for the productinfo module.
    '''

    def test_visibility_enum(self):
        '''
        Test the ProductVisibility enum helper.
        '''
        valid_xml_strings = [
            'Public',
            'Confidential'
        ]

        for xml_string in valid_xml_strings:
            enum = ProductVisibility.from_xml(xml_string)
            self.assertEqual(xml_string, enum.to_xml())

    def test_smoke(self):
        '''
        Test the ProductInfos can parse all database files.
        '''
        # Deserialize it ...
        deserialized_original = ProductInfos.from_file()
        self.assertIsNotNone(deserialized_original)

        # Serialize it
        serialized = deserialized_original.to_xml_str()

        # Deserialize it
        deserialized = ProductInfos.from_xml_str(serialized)
        self.assertEqual(deserialized_original, deserialized)

    def test_get_gpus(self):
        '''
        Test the ProductInfos can parse all database files.
        '''
        infos = ProductInfos.from_file()
        gpus = infos.get_gpus()
        self.assertGreater(len(gpus), 0)


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
