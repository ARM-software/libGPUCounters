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
This module contains utilities we use when processing equations, which are
defined using a language which can be parsed into an AST using Lark with a
formal grammar.

Lark provides standard frameworks for manipulating ASTs, including transformers
that implement a bottom-up tree traversal.

Available functionality includes:

* Converters to switch between text and AST representations.
* Transformer to simplify an AST for pretty-printing.
* Transformer to resolve an AST, recursively replacing references to
  derived counters until only hardware counters, constants, and literals
  remain.
* Transformer to change dialect from machine names to hardware names.
* Transformer to change dialect from machine names to Streamline names.
'''
from __future__ import annotations

import pathlib
import re
from typing import Any, Optional, TYPE_CHECKING

from lark.exceptions import LarkError, VisitError
from lark import Lark, Transformer, v_args

if TYPE_CHECKING:
    from .view.counterview import CounterView
    from .view.indexedview import IndexedView


# Pattern for matching constant variable names, which can consist of only upper
# case, numbers, and underscores.
CONSTANT_PATTERN = re.compile('^[A-Z0-9_]+$')


def _load_grammar() -> Lark:
    '''
    Build a lark parser from the textural grammar file.
    '''
    root = pathlib.Path(__file__).parent
    file_path = root / 'equationgrammar.lark'

    with open(file_path, 'r', encoding='utf-8') as handle:
        grammar = handle.read()

    parser = Lark(grammar, ambiguity='explicit')
    return parser


# Preload the grammar so we don't have to re-create the parser repeatedly
g_parser = _load_grammar()


def is_number(value) -> bool:
    '''
    Utility function to test if a string is a number.

    Returns:
        True if numeric, False otherwise.
    '''
    try:
        float(str(value))
        return True
    except ValueError:
        return False


def to_literal(value: float) -> str:
    '''
    Utility function to turn a number into a string.

    Returns:
        String value, which will be formatted as an integer if value can be
        represented losslessly as an integer, else formatted as a float.
    '''
    int_str = str(int(value))
    int_test = float(int_str)

    if value == int_test:
        return int_str

    return str(float)


class EquationError(BaseException):
    '''
    Utility exception class for GPU equation parse errors.

    Attributes:
        original: The original input string to the parse.
        error: The error string from the parser.
    '''

    def __init__(self, original: str, error: Any):
        '''
        Create a new GPU equation error exception.

        Args:
            original: The original input string to the parse.
            error: The error string from the parser.
        '''
        self.original = original
        self.error = error


class FunctionNode:
    '''
    Instances of this class represent variadic functions in expressions.

    These functions are of the form 'func(A, B, ...)' where parameters
    themselves may be subexpressions.

    Attributes:
        function: The name of the function in this node
        string: The string form of the equation in this node.
    '''

    def __init__(self, function: str, *params):
        '''
        Create a new equation node.

        Args:
            function: The function name.
            params: List of function parameter subexpressions.
        '''
        self.function = function
        self.string = f'{function}({", ".join(str(x) for x in params)})'

    def __str__(self):
        return self.string


class OperatorNode:
    '''
    Instances of this class represent operators in an expression.

    These operators are of the form 'A op B' where both A and B may be nested
    subexpressions or simple values.

    Attributes:
        operator: The operator of this node.
        precedence: The precedence of the operator in this node.
        string: The string form of the equation in this node.
    '''

    def __init__(self, op_a: str, operator: str, op_b: str, precedence: int):
        '''
        Create a new equation node.

        Args:
            operand_a: The first operand string.
            operator: The operator string.
            operand_b: The second operand string.
            precedence: The operator precedence.
        '''
        self.operator = operator
        self.precedence = precedence
        self.op_a = self.add_parens(op_a)
        self.op_b = self.add_parens(op_b)

        self.string = f'{self.op_a} {operator} {self.op_b}'

    def add_parens(self, operand):
        '''
        Utility function to add parentheses around an operand if needed.

        Parentheses are added whenever ...

          * an operand is actually a nested expression with a different
            operator precedence than the current expression. E.g. turn the
            tree [a * [b + c]] in to 'a * (b + c)' not 'a * b + c'.
          * an operator is actually a nested expression with a different
            operator than the current expression. E.g. turn the tree
            [a * [b / c]] in to 'a * (b / c)' not 'a * b / c'.

        Args:
            operand: The operand string.

        Returns:
            The operand in string form (either an expression or a name)
        '''
        if not isinstance(operand, OperatorNode):
            return operand

        if self.needs_parens(operand):
            operand = f'({operand})'

        return operand

    def needs_parens(self, operand):
        '''
        Does this operand need parens?

        The scheme used here will inject explicit parens for either a different
        operator or a different precedence.

        Args:
            operand: One of the operands of this node.

        Returns:
            True if parens are needed, False otherwise.
        '''
        return (self.operator != operand.operator) or \
               (self.precedence != operand.precedence)

    def __str__(self):
        return self.string


class EquationPrettyPrintTransformer(Transformer):
    '''
    Lark transformer to generate a pretty-printed equation string.
    '''

    def start(self, nodes):
        '''
        Entry point grammar rule rewrite.
        '''
        return nodes[0]

    @v_args(inline=True)
    def fname(self, name):
        '''
        min/max() function grammar rule rewrite.
        '''
        return name

    @v_args(inline=True)
    def function(self, name, *params):
        '''
        min/max() function grammar rule rewrite.
        '''
        return FunctionNode(name, *params)

    @v_args(inline=True)
    def min(self, *params):
        '''
        min() function grammar rule rewrite.
        '''
        return FunctionNode('min', *params)

    @v_args(inline=True)
    def max(self, *params):
        '''
        max() function grammar rule rewrite.
        '''
        return FunctionNode('max', *params)

    @v_args(inline=True)
    def add(self, operand_a, operator, operand_b):
        '''
        Add (and subtract) expression grammar rule rewrite.
        '''
        return OperatorNode(operand_a, operator, operand_b, 0)

    @staticmethod
    def _simplify_mul_str_str(operand_a, operator, operand_b):
        '''
        Attempt to rewrite literal muls for string * string.

        This peephole optimizes specific patterns we know occur in our XML
        expressions, it is not a totally generic optimizer that attempts to
        optimize all cases!

        Returns:
            Returns None if did not optimize, the mul rewrite otherwise.
        '''
        # Invalid types for this function
        if not isinstance(operand_a, str) or not isinstance(operand_b, str):
            return None

        # Invalid values for this function
        if not is_number(operand_a) or not is_number(operand_b):
            return None

        num_a = float(operand_a)
        num_b = float(operand_b)

        # A * B => value(A * B)
        if operator == '*':
            return to_literal(num_a * num_b)

        # A / B => value(A / B)
        return to_literal(num_a / num_b)

    @staticmethod
    def _simplify_mul_op_str(operand_a, operator, operand_b):
        '''
        Attempt to rewrite literal muls for operator * string.

        This peephole optimizes specific patterns we know occur in our XML
        expressions, it is not a totally generic optimizer that attempts to
        optimize all cases!

        Returns:
            Returns None if did not optimize, the mul rewrite otherwise.
        '''
        # Invalid types for this function
        if not isinstance(operand_a, OperatorNode) or \
           not isinstance(operand_b, str):
            return None

        # Invalid values for this function
        if not is_number(operand_a.op_b) or not is_number(operand_b):
            return None

        num_a = float(operand_a.op_b)
        num_b = float(operand_b)

        # (Aa * Ab) * B => Aa * value(Ab * B)
        if operand_a.operator == '*' and operator == '*':
            ab = to_literal(num_a * num_b)
            return OperatorNode(operand_a.op_a, '*', ab, 1)

        return None

    @v_args(inline=True)
    def mul(self, operand_a, operator, operand_b):
        '''
        Multiply (and divide) expression grammar rule rewrite.
        '''
        # Very basic optimizer to hide a*1, 1*b, and a/1
        if operand_b == '1':
            return operand_a

        if operator == '*' and operand_a == '1':
            return operand_b

        # More complex optimizers to merge "literal mul literal" patterns
        if value := self._simplify_mul_str_str(operand_a, operator, operand_b):
            return value

        if value := self._simplify_mul_op_str(operand_a, operator, operand_b):
            return value

        return OperatorNode(operand_a, operator, operand_b, 1)

    @v_args(inline=True)
    def name(self, args):
        '''
        Variable name grammar rule rewrite.
        '''
        if isinstance(args, OperatorNode):
            return args

        return str(args)


class EquationResolveTransformer(Transformer):
    '''
    Lark transformer to recursively resolve references in the AST so that the
    final AST only contain hardware counters, constants, and literals.
    '''

    def __init__(self, index_view: IndexedView):
        '''
        Create a new equation resolve transformer.

        Args:
            index_view: The compiled index view for this GPU.
        '''
        self.index_view = index_view
        super().__init__(visit_tokens=True)

    @v_args(inline=True)
    def NAME(self, args):  # pylint: disable=invalid-name
        '''
        Variable name grammar token rewrite.
        '''
        # For constants, just return the macro name directly
        if CONSTANT_PATTERN.match(args):
            return args

        # For counters, fetch the metadata
        counter = self.index_view.get_by_machine_name(args)
        assert counter is not None, f'Missing counter: {args}'

        # Hardware counters don't need tree resolve so return name directly
        if not counter.is_derived():
            return args

        # Derived counters need tree resolve so recursively resolve ...
        transformer = EquationResolveTransformer(self.index_view)
        return transformer.transform(counter.equation_ast)


class EquationHardwareTransformer(Transformer):
    '''
    Lark transformer to rename all counter machine names in an AST with the
    equivalent hardware layout name.

    This must only be used on an AST that has already been resolved to
    hardware counters using an EquationResolveTransformer.
    '''

    def __init__(self, index_view: IndexedView):
        '''
        Create a new equation rename transformer.

        Args:
            index_view: The compiled index view for this GPU.
        '''
        self.index_view = index_view
        super().__init__(visit_tokens=True)

    @v_args(inline=True)
    def NAME(self, args):  # pylint: disable=invalid-name
        '''
        Variable name grammar token rewrite.
        '''
        counter = self.index_view.get_by_machine_name(args)

        # This wasn't a counter so keep it as is
        if not counter:
            return args

        return counter.source_name


class EquationStreamlineTransformer(Transformer):
    '''
    Lark transformer to rename all counter machine names in an AST with the
    equivalent Streamline name.

    This must only be used on an AST that has already been resolved to
    hardware counters using an EquationResolveTransformer.

    Note that Streamline variable names depend on the group name and group
    human name of the counter, so all Streamline related assets must be
    regenerated from the same version of the database.
    '''

    CONSTANT_PATTERN = re.compile('^[A-Z0-9_]+$')

    def __init__(self, index_view: IndexedView):
        '''
        Create a new equation rename transformer.

        Args:
            index_view: The compiled index view for this GPU.
        '''
        self.index_view = index_view
        super().__init__(visit_tokens=True)

    @v_args(inline=True)
    def NAME(self, args):  # pylint: disable=invalid-name
        '''
        Variable name grammar token rewrite.
        '''
        if self.CONSTANT_PATTERN.match(args):
            return self.mangle_constant_name(args)

        counter = self.index_view.get_by_machine_name(args)
        assert counter
        return self.mangle_variable_name(counter)

    @staticmethod
    def mangle_variable_name(counter: CounterView) -> str:
        '''
        Generate the Streamline name for a counter.

        Args:
            counter: The CounterView instance.

        Returns:
            The Streamline variable name.
        '''
        assert not counter.is_derived()

        parts = []
        full_name = f'{counter.group_name} {counter.group_human_name}'

        # Split words based on things Streamline lets us keep
        for part in re.split(r'[^_A-Za-z0-9]+', full_name):
            if not part:
                continue

            # Upper case first letter of each word
            part_list = list(part)
            part_list[0] = part_list[0].upper()
            parts.append(''.join(part_list))

        new_name = ''.join(parts)

        # Clean up prefixes if a number
        if new_name[0] in '0123456789':
            new_name = f'_{new_name}'

        return f'$Mali{new_name}'

    @staticmethod
    def mangle_constant_name(counter: str) -> str:
        '''
        Generate the Streamline name for a constant.

        Args:
            counter: The CounterView instance.

        Returns:
            The Streamline constant name or expression.
        '''

        counter_names = {
            'MALI_CONFIG_TIME_SPAN': '$ZOOM',
            'MALI_CONFIG_L2_CACHE_COUNT': '$MaliConstantsL2SliceCount',
            'MALI_CONFIG_SHADER_CORE_COUNT': '$MaliConstantsShaderCoreCount',
            'MALI_CONFIG_EXT_BUS_BYTE_SIZE': '($MaliConstantsBusWidthBits / 8)'
        }

        return counter_names[counter]


def get_machine_name_expression(
        index_view: IndexedView, counter: CounterView) -> str:
    '''
    Get the machine name expression string for the target counter.

    Args:
        index_view: Counter database for the current GPU.
        counter: Counter to generate expression for.

    Return:
        The pretty-printed string using machine names.
    '''
    # Index view is included so we can keep all the get_ functions the same
    del index_view

    assert counter.equation_ast_resolved

    # Pretty print the output
    transformer = EquationPrettyPrintTransformer()
    ast = transformer.transform(counter.equation_ast_resolved)
    equation = str(ast)

    # Percentages are clamped between 0 and 100 in the visualization, as
    # approximations might exceed these bounds and get confusing.
    if counter.unit == 'percent':
        equation = f"max(min({equation}, 100), 0)"

    return equation


def get_source_name_expression(
        index_view: IndexedView, counter: CounterView) -> str:
    '''
    Get the hardware expression string for the target counter.

    Args:
        index_view: Counter database for the current GPU.
        counter: Counter to generate expression for.

    Return:
        The pretty-printed string using hardware names.
    '''
    assert counter.equation_ast_resolved

    # Generate Hardware AST
    transformer = EquationHardwareTransformer(index_view)
    ast = transformer.transform(counter.equation_ast_resolved)

    # Pretty print the output
    transformer2 = EquationPrettyPrintTransformer()
    ast = transformer2.transform(ast)
    equation = str(ast)

    # Percentages are clamped between 0 and 100 in the visualization, as
    # approximations might exceed these bounds and get confusing.
    if counter.unit == 'percent':
        equation = f"max(min({equation}, 100), 0)"

    return equation


def get_streamline_expression(
        index_view: IndexedView, counter: CounterView) -> str:
    '''
    Get the Streamline expression string for the target counter.

    Args:
        index_view: Counter database for the current GPU.
        counter: Counter to generate expression for.

    Return:
        The pretty-printed string using Streamline names.
    '''
    if not counter.is_derived():
        ast = equation_string_to_ast(counter.machine_name)[0]
    else:
        ast = counter.equation_ast_resolved

    # Generate Streamline AST
    transformer = EquationStreamlineTransformer(index_view)
    ast = transformer.transform(ast)

    # Pretty print the output
    transformer2 = EquationPrettyPrintTransformer()
    ast = transformer2.transform(ast)
    equation = str(ast)

    # Percentages are clamped between 0 and 100 in the visualization, as
    # approximations might exceed these bounds and get confusing.
    if counter.unit == 'percent':
        equation = f"max(min({equation}, 100), 0)"

    return equation


def equation_string_to_ast(string: str) -> tuple[Any, Optional[str]]:
    '''
    Convert an equation string into a parsed Lark AST.

    Args:
        string: The equation in string form.

    Returns:
        The parsed AST in arg0, or an error message in arg1.
    '''
    try:
        return (g_parser.parse(string), None)
    except LarkError as ex:
        return (None, str(ex).strip())


def equation_ast_to_string(ast: Any) -> str:
    '''
    Convert an equation AST back into a pretty string.

    Args:
        ast: The equation AST.

    Returns:
        The equation in string form.
    '''
    transformer = EquationPrettyPrintTransformer()
    return transformer.transform(ast)


def equation_ast_to_resolved_ast(
        ast: Any, index_view: IndexedView) -> tuple[Any, Optional[str]]:
    '''
    Resolve all symbols in an AST to remove derived references.

    Args:
        ast: The unresolved AST.
        index_view: The GPU view to use as a lookup.

    Returns:
        The resolved AST in arg0, or an error message in arg1. Errors should
        not occur in a production database release, as all counters must be
        present.
    '''
    try:
        transformer = EquationResolveTransformer(index_view)
        resolved = transformer.transform(ast)
        return (resolved, None)

    # This should not fail in production but can fail during development
    # if an equation has missing references. This is detected by the
    # validator!
    except VisitError as ex:
        return (None, str(ex.orig_exc).strip())
