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
This module contains tests for the hardwarelayout module.

These tests aim to sense check the implementation of the Python code, and
do not check the validity of the data in the hardware layout database.
'''

import pathlib
import sys
import unittest

from .hardwarelayout import HardwareBlockType, HardwareLayout, HardwareLayouts


class HardwareLayoutTestSuite(unittest.TestCase):
    '''
    Unit tests for the hardwarelayout module.
    '''

    def test_blocktype_enum(self):
        '''
        Test the HardwareBlockType enum helper.
        '''
        valid_xml_strings = [
            'GPU Front-end',
            'Shader Core',
            'Memory System',
            'Tiler',
        ]

        for xml_string in valid_xml_strings:
            enum = HardwareBlockType.from_xml(xml_string)
            encoded_enum = enum.to_xml()
            self.assertEqual(xml_string, encoded_enum)

    def test_layout_smoke(self):
        '''
        Test the HardwareLayout can parse all database files.
        '''
        # Fetch the data using a script-relative path
        root = pathlib.Path(__file__).parent.parent.parent
        dir_path = root / 'database' / 'hardwarelayout'
        assert dir_path.exists() and dir_path.is_dir()

        database_files = dir_path.glob('*.xml')

        for file_path in database_files:
            with open(file_path, 'r', encoding='utf-8') as handle:
                data = handle.read()

            # Deserialize it ...
            deserialized_orig = HardwareLayout.from_xml_str(data, file_path)
            self.assertIsNotNone(deserialized_orig)

            # Serialize it without pretty printing ...
            serialized = deserialized_orig.to_xml_str()
            self.assertTrue(serialized)

            # Deserialize it again and check it matches ...
            deserialized = HardwareLayout.from_xml_str(serialized, file_path)
            self.assertEqual(deserialized_orig, deserialized)

            # Serialize it with pretty printing ...
            serialized = deserialized.to_xml_str(pretty_print=True)
            self.assertTrue(serialized)

            # Deserialize it again and check it matches ...
            deserialized = HardwareLayout.from_xml_str(serialized, file_path)
            self.assertEqual(deserialized_orig, deserialized)

    def test_layouts_smoke(self):
        '''
        Test the HardwareLayouts can parse all database files.
        '''
        # Load all layouts
        HardwareLayouts.from_files()


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
