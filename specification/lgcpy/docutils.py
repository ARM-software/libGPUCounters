#
# Copyright (c) 2019-2025 Arm Limited.
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
This module contains utilities we use when pretty-printing documentation
sections in the database, and when building user-visible documentation from the
machine readable specification.
'''

import re

import markdown

from .database import CounterDatabase
from .view.indexedview import IndexedView
from . import equationutils as eu
from . import xmlutils as xu


def resolve_doc_to_text(document: str, index_view: IndexedView) -> str:
    '''
    Resolve all symbolic references in a document entry to text values.

    This function replaces all symbolic references in a database document
    entry with the appropriate string for the current product. Supported
    symbolic references are:

    -  {{K:GPU_NAME}}: Insert GPU product name.
    -  {{C:<MachineName>}}: Insert human name of <MachineName> counter.
    -  {{C:<MachineName>.equation}}: Insert equation of <MachineName> counter.

    It is likely that web-based documentation may want something more nuanced,
    such as also injecting hyperlinks to cross-reference counters, but this
    is a good reference implementation for more advanced use.

    Args:
        document: The description to resolve.
        index_view: The view for the current GPU we can use to find references.

    Returns:
        Resolved string.
    '''
    product = CounterDatabase.get_product_info_for(index_view.gpu)

    # Callable to replace any counter references in the description text
    def replace(match):
        pattern = match.group(1)

        # Validate reference pattern is legal
        ref_type, _, reference = pattern.partition('::')
        assert ref_type in ('C', 'K'), f'Bad reference type {pattern}'

        ref_name, _, ref_part = reference.partition('.')
        assert ref_part in ('', 'equation'), f'Bad reference part {pattern}'

        # Literal constant reference
        if ref_type == 'K':
            if ref_name == 'GPU_NAME':
                return product.get_document_name(allow_indirect=True)

        # Counter reference
        elif ref_type == 'C':
            counter = index_view.get_by_machine_name(ref_name)

            # No postfix returns the human name
            if ref_part == '':
                return counter.human_name

            # Equation postfix returns the equation value
            if ref_part == 'equation':
                expression = eu.equation_ast_to_string(counter.equation_ast)
                return str(expression)

        # Should never reach this ...
        assert False

    return re.sub(r'{{(.*?)}}', replace, document, 0)


def resolve_doc_to_hyperlink(document: str, index_view: IndexedView) -> str:
    '''
    Resolve all symbolic references in a document entry.

    This function replaces all symbolic references in a database document
    entry with the appropriate string for the current product. Supported
    symbolic references are:

    -  {{K:GPU_NAME}}: Insert GPU product name.
    -  {{C:<MachineName>}}: Insert human name of <MachineName> counter.
    -  {{C:<MachineName>.equation}}: Insert equation of <MachineName> counter.

    It is likely that web-based documentation may want something more nuanced,
    such as also injecting hyperlinks to cross-reference counters, but this
    is a good reference implementation for more advanced use.

    Args:
        document: The description to resolve.
        index_view: The view for the current GPU we can use to find references.

    Returns:
        Resolved string.
    '''
    product = CounterDatabase.get_product_info_for(index_view.gpu)

    # Callable to replace any counter references in the description text
    def replace(match):
        pattern = match.group(1)

        # Validate reference pattern is legal
        ref_type, _, reference = pattern.partition('::')
        assert ref_type in ('C', 'K'), f'Bad reference type {pattern}'

        ref_name, _, ref_part = reference.partition('.')
        assert ref_part in ('', 'equation'), f'Bad reference part {pattern}'

        # Literal constant reference
        if ref_type == 'K':
            if ref_name == 'GPU_NAME':
                return product.get_document_name(allow_indirect=True)

        # Counter reference
        elif ref_type == 'C':
            counter = index_view.get_by_machine_name(ref_name)

            # No postfix returns the human name
            if ref_part == '':
                label = counter.human_name
                link = f'<a href="#{counter.get_anchor()}">{label}</a>'
                return link

            # Equation postfix returns the equation value
            if ref_part == 'equation':
                label = eu.equation_ast_to_string(counter.equation_ast)
                link = f'<a href="#{counter.get_anchor()}">{label}</a>'
                return link

            # Should never reach this ...
            assert False

        # Should never reach this ...
        assert False

    return re.sub(r'{{(.*?)}}', replace, document, 0)


def to_markdown_string(document: str) -> str:
    '''
    Format a long description as a Markdown string.

    Args:
        document: The description to format.

    Returns:
        The formatted string.
    '''
    return xu.to_pretty_xml(document, True, 0, 0, 79)


def to_html_string(document: str) -> str:
    '''
    Format a long description as an HTML string.

    Args:
        document: The description to format.

    Returns:
        The formatted string.
    '''
    return markdown.markdown(xu.to_pretty_xml(document, True, 0, 0, 79))
