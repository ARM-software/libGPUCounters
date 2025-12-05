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
This module contains tests for the semanticlayout module.

These tests aim to sense check the implementation of the Python code, and
do not check the validity of the data in the hardware layout database.
'''

import sys
import unittest

from .semanticlayout import SemanticLayout


class SemanticLayoutTestSuite(unittest.TestCase):
    '''
    Unit tests for the semanticlayout module.
    '''

    def test_smoke(self):
        '''
        Test the SemanticLayout can parse all database files.
        '''
        # Deserialize it ...
        deserialized_original = SemanticLayout.from_file()

        # Serialize it
        serialized = deserialized_original.to_yaml_str()

        # Deserialize it
        deserialized = SemanticLayout.from_yaml_str(serialized)
        self.assertEqual(deserialized_original, deserialized)


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
