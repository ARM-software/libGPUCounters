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
This module contains utilities we use when processing Arm GPU names.
'''
import re

# Bifrost, Valhall, and early 5th Generation architecture - e.g. Mali-G77.
_GPU_GROUP_0 = re.compile(r'^(Mali|Immortalis)-G(\d+)$')

# Later 5th Generation names excluding postfix - e.g. Mali G1.
_GPU_GROUP_1 = re.compile(r'^Mali G(\d+)$')

# Later 5th Generation names including postfix - e.g. Mali G1-Ultra.
_GPU_GROUP_2 = re.compile(r'^Mali G(\d+)-(\S+)$')

# Internal Arm codenames which use non-numeric identifiers e.g. Mali GAAx.
_GPU_GROUP_3 = re.compile(r'^Mali (\S+)$')


def _sort_gpu_code(product_name: str) -> tuple[int, int, int, str]:
    '''
    Convert a GPU name into a sortable tuple.

    Args:
        product_name: Name of the product.

    Returns:
        Sortable tuple (group, product, subproduct, name). Group, product and
        subproduct are integers, allowing a list of tuples to be sorted, and
        then name to be recovered in presentation-order.
    '''
    if match := _GPU_GROUP_0.match(product_name):
        group = 0
        product = int(match.group(2))

        group0_subproducts = {
            'Mali': 0,
            'Immortalis': 1,
        }

        subproduct = group0_subproducts[match.group(1)]

    elif match := _GPU_GROUP_1.match(product_name):
        group = 1
        product = int(match.group(1))
        subproduct = 0

    elif match := _GPU_GROUP_2.match(product_name):
        group = 2
        product = int(match.group(1))

        group2_subproducts = {
            'Pro': 0,
            'Premium': 1,
            'Ultra': 2
        }

        subproduct = group2_subproducts[match.group(2)]

    elif match := _GPU_GROUP_3.match(product_name):
        group = 3
        plen = len(match.group(1))
        parts = [(plen - i) * 256 * ord(x)
                 for i, x in enumerate(match.group(1))]
        product = sum(parts)
        subproduct = 0

    else:
        assert False

    return (group, product, subproduct, product_name)


def sort_gpus(product_names: list[str]) -> list[str]:
    '''
    Sort a list of Arm GPU products into presentation order, oldest first.

    Presentation order is approximately based on product age, with more modern
    products appearing later in the list, but it is not a simple date sort.
    For example, Mali-G68 came out after Mali-G77 but is in the list before it.

    Args:
        products_names: An unsorted list of Arm GPU products.

    Returns:
        A list of Arm GPU products in presentation order.
    '''
    product_list = [_sort_gpu_code(x) for x in product_names]
    product_list.sort()
    return [x[3] for x in product_list]
