#
# Copyright (c) 2026 Arm Limited.
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
This module contains tests for the xmlutils module.
'''

import unittest

from . import xmlutils as xu


class XMLUtilsTestSuite(unittest.TestCase):
    '''
    Unit tests for the XMLUtils module.
    '''

    def test_text_wrap_multiline(self):
        '''
        Test the text wrap utility functions.
        '''
        # Word wrap on column 7
        test_text = '\n'.join([
            '  aa bb',    # Full line
            '',           # New line
            '  * a b',    # Full line
            '  * a',      # Under-filled - expect collapse to one line
            '    b',
            '  * a bb',   # Over-filled - expect wrap to two lines
            '',
            '  aa',       # Under-filled - expect collapse to one line
            '  bb',
            '',
            '  aa bbb',   # Over-filled - expect collapse to one line
        ])

        expected_text = '\n'.join([
            '',
            '  aa bb',
            '',
            '  * a b',
            '  * a b',
            '  * a',
            '    bb',
            '',
            '  aa bb',
            '',
            '  aa',
            '  bbb',
            ''
        ])

        parsed = xu.from_pretty_xml(
            test_text,
            multiline=True)

        serial = xu.to_pretty_xml(
            parsed,
            multiline=True,
            indent=2,
            outdent=0,
            width=7)

        self.assertEqual(expected_text, serial)


if __name__ == '__main__':
    unittest.main()
