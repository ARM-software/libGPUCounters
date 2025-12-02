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
This module contains utilities we use when processing XML, and serializing and
deserializing database entries to/from XML.
'''

from __future__ import annotations

from xml.dom import minidom
import re
import textwrap
from typing import Optional
import xml.etree.ElementTree as et


def add_copyright_to_xml_str(document: str, copyright_msg: str) -> str:
    '''
    Inject the copyright message into an XML string.

    Assumes input XML is pretty-printed, with an <?xml> node on the first line.

    We can't do this using ElementTree because it cannot add comments between
    XML declaration and the root node.

    Args:
        document: The XML file.
        copyright_msg: The copyright payload to add.

    Returns:
        Modified document with copyright added.
    '''
    doc_lines = document.splitlines()

    com_lines = copyright_msg.splitlines()
    com_lines.insert(0, '<!--')
    com_lines.append('-->')

    doc_lines = doc_lines[:1] + com_lines + doc_lines[1:]

    return '\n'.join(doc_lines) + '\n'


def get_copyright_from_xml_str(document: str) -> str:
    '''
    Extract the copyright message from an XML string.

    Assumes that the copyright is the first comment that is a child node of
    the document root.

    We can't do this using ElementTree because it discards comments.

    Args:
        document: The XML file.

    Returns:
        Multi-line copyright string.
    '''
    xml = minidom.parseString(document)

    # First comment in the XML must be the copyright message
    for node in xml.childNodes:
        if node.nodeType == node.COMMENT_NODE:
            return node.data.strip()

    return ''


def get_copyright_from_yaml_str(document: str) -> str:
    '''
    Extract the copyright message from a YAML string.

    Assumes that the copyright is the first block of comments in the document.

    We can't do this using PyYAML because it discards comments.

    Args:
        document: The YAML file.

    Returns:
        Multi-line copyright string.
    '''
    copyright_msg: list[str] = []
    lines = document.splitlines()

    for line in lines:
        is_comment = line.startswith('#')

        # End the loop when we end the first comment
        if not is_comment:
            if copyright_msg:
                break

        copyright_msg.append(line)

    return '\n'.join(copyright_msg)


def get_node_str(root: et.Element[str], tag: str) -> str:
    '''
    Helper to get a known-to-exist string from a known-to-exist XML node.

    This utility exists with asserts to keep mypy type checking happy.

    Args:
        root: The root XML node to search.
        tag: The XML tag of the child.

    Return:
        The string value.
    '''
    element = root.find(tag)
    assert element is not None
    result = element.text
    assert result is not None
    return result


def get_node_opt_str(root: et.Element[str], tag: str) -> Optional[str]:
    '''
    Helper to get a known-to-exist string from an optional XML node.

    This utility exists with asserts to keep mypy type checking happy.

    Args:
        root: The root XML node to search.
        tag: The XML tag of the child.

    Return:
        The string value if the node exists, or None otherwise.
    '''
    element = root.find(tag)
    if element is None:
        return None

    result = element.text
    assert result is not None
    return result


def get_node_opt_int(root: et.Element[str], tag: str) -> Optional[int]:
    '''
    Helper to get a known-to-exist int from an optional XML node.

    This utility exists with asserts to keep mypy type checking happy.

    Args:
        root: The root XML node to search.
        tag: The XML tag of the child.

    Return:
        The int value if the node exists, or None otherwise.
    '''
    element = root.find(tag)
    if element is None:
        return None

    result = element.text
    assert result is not None
    return int(result)


def _to_pretty_xml__form_blocks(data: str, indent: int,
                                width: int) -> list[str]:
    '''
    Form a list of text-wrapped blocks we can emit in a markdown-like syntax.

    Input data is a list of lines where lines that do not start with '*' are
    normal paragraphs, and lines that start with '*' are bullet list items. The
    syntax is deliberately simple so we do not handle corner cases:

    * Paragraphs are not allowed to start with "*" characters.
    * Bulleted lists are only allowed to have a single level, and two different
      lists cannot be adjacent without an interposing paragraph.

    Args:
        data: List of paragraphs or bullet items.
        indent: Number of spaces to indent by to support emitting into
            nicely nested pretty-printed XML files.
        width: Number of characters per line including indent.

    Returns:
        A list of word-wrapped lines that can be emitted as elements in a
        pretty-printed markdown document, but not yet spaced so that markdown
        would parse things as paragraphs and lists.
    '''
    paras = data.splitlines()
    new_paras: list[str] = []

    for para in paras:
        is_list = para.startswith('*')

        # List wrapping aligns on the first character after the bullet
        if is_list:
            lines = textwrap.wrap(para,
                                  break_on_hyphens=False,
                                  width=width - indent - 2)

            new_text = '\n  '.join(lines)

        # Else wrap at the start of the indent block.
        else:
            lines = textwrap.wrap(para,
                                  break_on_hyphens=False,
                                  width=width - indent)

            new_text = '\n'.join(lines)

        new_paras.append(textwrap.indent(new_text, ' ' * indent))

    return new_paras


def _to_pretty_xml__space_blocks(paras: list[str], indent: int) -> list[str]:
    '''
    Form a list of text-wrapped blocks we can emit in a markdown-like syntax.

    Input data is a list of lines where lines that do not start with '*' are
    normal paragraphs, and lines that start with '*' are bullet list items,
    any of which may be pre-indented by 'indent' spaces.

    Args:
        data: List of paragraphs or bullet items.
        indent: Number of spaces to indent by to support emitting into
            nicely nested pretty-printed XML files.

    Returns:
        A list of word-wrapped lines that can be emitted as elements in a
        pretty-printed markdown document, with blank lines emitted between
        elements so that markdown would parse things as paragraphs and lists.
    '''
    bullet_indent = (' ' * indent) + '*'
    was_in_list = False

    new_paras = []
    for i, para in enumerate(paras):
        # Add newlines before the current element where needed
        in_list_now = para.startswith(bullet_indent)
        end_of_list = was_in_list and not in_list_now
        if (i != 0) and ((not was_in_list) or end_of_list):
            new_paras.append('')

        new_paras.append(para)
        was_in_list = in_list_now

    return new_paras


def to_pretty_xml(data: str, multiline: bool = False,
                  indent: int = 6, outdent: int = 4, width: int = 79) -> str:
    '''
    Format a string so that it prints nicely in pretty printed XML.

    Args:
        data: The string to pretty print.
        multiline: True if to use multi-line pretty print line wrapping.
        indent: Characters of line indent to use on all content lines.
        outdent: Characters of line intent to use on a final line.
        width: The column width to wrap on, including any indent.

    Returns:
        The formatted data.
    '''
    if not multiline:
        return f'\n{" " * indent}{data}\n{" " * outdent}'

    # Break into paragraphs for wrapping and spacing
    paras = _to_pretty_xml__form_blocks(data, indent, width)
    paras = _to_pretty_xml__space_blocks(paras, indent)

    document = '\n'.join(paras)
    return f'\n{document}\n{" " * outdent}'


def from_pretty_xml(data: str, multiline: bool = False) -> str:
    '''
    Convert from a pretty-printed string to an internal representation.

    This step undoes whitespace indentation and multi-line wordwrap.

    Args:
        data: The string to pretty print.
        multiline: True if we need to unpick multi-line pretty-printing.

    Returns:
        The formatted data, stored as a set of newline separated records.

        If a line does not start with a '*' then it is a normal paragraph,
        else it is a bullet list item. Any sequence of contiguous lines
        starting with '*' will form a single bullet list.
    '''
    # Break into lines of text
    lines = data.strip().splitlines()

    # Single line strings are just prefix padded - strip it.
    if not multiline:
        assert len(lines) == 1
        line = lines[0]
        return line.strip()

    is_newline = 0  # Last saw a newline
    is_text = 1  # Last saw a text paragraph
    is_bullet = 2  # Last saw a bullet paragraph
    status = is_newline

    new_data: list[str] = []

    for line in lines:
        line = line.strip()
        if line:
            # If we last saw a newline start a new paragraph
            if status == is_newline:
                if line.startswith('*'):
                    status = is_bullet
                else:
                    status = is_text
                new_data.append(line)
            # Elif we last saw a bullet is this a new bullet?
            elif status == is_bullet and line.startswith('*'):
                new_data.append(line)
            # Else this is a more content for the last thing we saw
            else:
                spacing = ' '

                # Are we reassembling across a hyphen split, instead of a
                # whitespace split? Note - assumption here is that any
                # non-space followed by a `-` is a hyphen and should be
                # merged with the next line.
                if len(new_data[-1]) >= 2:
                    pattern = re.compile(r'^.*\S{1}-$')
                    if pattern.match(new_data[-1]):
                        spacing = ''

                new_data[-1] += spacing + line

        else:
            status = is_newline

    return '\n'.join(new_data)
