#!/usr/bin/env python3
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
This script implements database content validation.

It is assumed that basic Python test suites have been run and passed before
running this validator. Early tests are expected to catch major structural
errors like malformed XML and missing database files, as many stateless
data errors where because those errors trigger asserts on load.

These earlier checks do not catch stateful data consistency issues that require
cross-referencing database entries to detect, or data formatting issues.

Checkers
========

Hardware counter completeness checks that ensure all expected counters, and no
unexpected counters, are present.

Derived counter completeness checks that ensure that all expressions can be
resolver for every GPU. This includes checking that public counters do not
refer to internal counters. This also checks that expressions do not mix
counters across clock domains without a suitable scaling factor.

Semantic layout completeness checks that ensure all expected counters, and no
unexpected counters, are present.

Database field consistency checks.
  * Stable IDs must be unique.
  * Units must be a known value.

Editing helpers
===============

To help with manual editing, the validator can inject some changes and write
the files back to disk for human review:

* Inject suggested StableID field if field is missing.
* Pretty print XML and YAML to standard formatting.
'''

import argparse
from collections import defaultdict
import enum
import re
import sys
import textwrap

from lgcpy import CounterDatabase, IndexedView
from lgcpy.data.counterinfo import CounterInfo
from lgcpy.data.hardwarelayout import HardwareBlockType as HWBlock


# Test functions will use internal class functions to access raw databases
# pylint: disable=protected-access

# Disable false-positive on iterating subclasses of lists
# pylint: disable=not-an-iterable


def print_err(reason: str) -> None:
    '''
    Print a formatted error.

    Args:
        reason: Human readable error.
    '''
    print(f'FAIL: {reason}')


def print_err_c(reason: str, counter: str) -> None:
    '''
    Print a formatted error for a counter.

    Args:
        reason: Human readable error.
        counter: The counter in error
    '''
    print(f'FAIL: {counter}: {reason}')


def print_err_cd(reason: str, detail: str, counter: str) -> None:
    '''
    Print a formatted error for a counter.

    Args:
        reason: Human readable error.
        detail: Detail of the error.
        counter: The counter in error
    '''
    print(f'FAIL: {counter}: {reason} [{detail}]')


def print_err_gc(reason: str, gpu: str, counter: str) -> None:
    '''
    Print a formatted error for a gpu and a counter.

    Args:
        reason: Human readable error.
        gpu: The GPU in error.
        counter: The counter in error
    '''
    print(f'FAIL: {gpu}.{counter}: {reason}')


def print_err_gcd(reason: str, detail: str, gpu: str, counter: str) -> None:
    '''
    Print a formatted error for a gpu and a counter.

    Args:
        reason: Human readable error.
        detail: Detail of the error.
        gpu: The GPU in error.
        counter: The counter in error
    '''
    print(f'FAIL: {gpu}.{counter}: {reason} [{detail}]')


def validate_whitespace() -> int:
    '''
    Validate whitespace consistency.

    Ensures we have no pre/post or double space whitespace in text fields.

    Returns:
        The number of errors discovered.
    '''
    errors = 0

    cnt_db = CounterDatabase.g_counter_info_db
    assert cnt_db is not None

    for counter in cnt_db:
        cname = counter.machine_name

        fields = {
            'Machine name': counter.machine_name,
            'Source name': counter.source_name if counter.source_name else '',
            'Human name': counter.human_name,
            'Group name': counter.group_name,
            'Group human name': counter.group_human_name,
            'Short description': counter.short_description,
            'Long description': counter.long_description
        }

        for field, value in fields.items():
            if value != value.strip():
                reason = 'Pre/post whitespace in CounterInfo'
                print_err_cd(reason, field, cname)
                errors += 1

            if '  ' in field:
                reason = 'Double whitespace in CounterInfo'
                print_err_cd(reason, field, cname)
                errors += 1

    si_db = CounterDatabase.g_semantic_section_info_db
    assert si_db

    for section in si_db:
        name = section.name

        fields = {
            'Name': section.name,
            'Long description': section.long_description
        }

        for field, value in fields.items():
            if value != value.strip():
                reason = 'Pre/post whitespace in SemanticSectionInfo'
                print_err_cd(reason, field, name)
                errors += 1

            if '  ' in value:
                reason = 'Double whitespace in SemanticSectionInfo'
                print_err_cd(reason, field, name)
                errors += 1

    gr_db = CounterDatabase.g_semantic_group_info_db
    assert gr_db

    for group in gr_db:
        name = group.name

        fields = {
            'Name': group.name,
            'Long description': group.long_description
        }

        for field, value in fields.items():
            if value != value.strip():
                reason = 'Pre/post whitespace in SemanticGroupInfo'
                print_err_cd(reason, field, name)
                errors += 1

            if '  ' in value:
                reason = 'Double whitespace in SemanticGroupInfo'
                print_err_cd(reason, field, name)
                errors += 1

    return errors


def validate_absence_of_resolves() -> int:
    '''
    Validate strings do not have resolves when not expected to.

    Returns:
        The number of errors discovered.
    '''
    errors = 0

    cnt_db = CounterDatabase.g_counter_info_db
    assert cnt_db is not None

    for counter in cnt_db:
        cname = counter.machine_name

        fields = {
            'Human name': counter.human_name,
            'Group name': counter.group_name,
            'Group human name': counter.group_human_name,
            'Short description': counter.short_description,
        }

        for field, value in fields.items():
            if '{{' in value:
                reason = 'Counter reference in CounterInfo'
                print_err_cd(reason, field, cname)
                errors += 1

    return errors


def validate_vulkan_string_lengths() -> int:
    '''
    Validate string lengths for things used directly in Arm Vulkan drivers.

    These strings must be below VK_MAX_DESCRIPTION_SIZE, which is 256
    characters including a NUL terminator.

    Returns:
        The number of errors discovered.
    '''
    errors = 0
    max_size = 255

    cnt_db = CounterDatabase.g_counter_info_db
    assert cnt_db is not None

    for counter in cnt_db:
        cname = counter.machine_name

        fields = {
            'Human name': counter.human_name,
            'Short description': counter.short_description
        }

        for field, value in fields.items():
            if len(value) > max_size:
                reason = 'Field too long for Vulkan'
                print_err_cd(reason, field, cname)
                errors += 1

    return errors


def validate_stable_ids() -> int:
    '''
    Validate StableID consistency.

    Stable IDs must be unique to a given MachineName, but there can be multiple
    copies of CounterInfo in the database with the same MachineName as we may
    have different entries for different GPUs. This can include differences in
    human name strings, as we may describe concepts different for different
    GPU architectures.

    Returns:
        The number of errors discovered.
    '''
    cnt_db = CounterDatabase.g_counter_info_db
    assert cnt_db is not None

    errors = 0

    # Counters with IDs that we've found
    # IDs must be used for the same machine name
    found_ids: dict[int, CounterInfo] = {}

    # Counters with MachineNames that we've found
    # IDs must be ONLY be used for the same machine name
    found_names: dict[str, CounterInfo] = {}

    # Counters with no IDs that need one
    missing_ids: defaultdict[str, list[CounterInfo]] = defaultdict(list)

    for counter in cnt_db:
        stable_id = counter.stable_id
        machine_name = counter.machine_name

        # Counter has no stable ID
        if stable_id is None:
            missing_ids[machine_name].append(counter)
            continue

        # Check stable ID is unique
        #   - Not found
        if stable_id not in found_ids:
            found_ids[stable_id] = counter

        #   - Found and mismatched
        elif found_ids[stable_id].machine_name != machine_name:
            cname = counter.machine_name
            reason = 'Stable ID reused for a different Machine Name'
            print_err_c(reason, cname)
            errors += 1

        # Check machine name aliases to the same stable ID
        if machine_name not in found_names:
            found_names[machine_name] = counter

        #   - Found and mismatched
        elif found_names[machine_name].stable_id != stable_id:
            cname = counter.machine_name
            reason = 'Machine Name alias using a different Stable ID'
            print_err_c(reason, cname)
            errors += 1

    # Propose Stable IDs for entries missing them, this patches the database
    # in-place so we can write the suggested values back to file automatically
    def get_unassigned_id() -> int:
        for i in range(0, max(found_ids.keys()) + 2):
            if i not in found_ids:
                return i

        # Should never hit this
        return 0

    for machine_name, counters in missing_ids.items():
        # Later counter with the same name assigned an ID
        if machine_name in found_names:
            stable_id = found_names[machine_name].stable_id
        # Else we need a new one
        else:
            stable_id = get_unassigned_id()

        for counter in counters:
            counter.stable_id = stable_id
            cname = counter.machine_name
            print_err_cd(
                'Missing stable ID', f'suggest {stable_id}',
                counter.machine_name)
            errors += 1

    return errors


def validate_semantic_layout() -> int:
    '''
    Validate semantic layout consistency.

    Section and group names must be unique in the whole layout. All sections
    must have at least one group. All groups must have at least one counter.
    All counters must have a group.

    Note that counter series group human names are NOT required to be globally
    unique, because they are scoped to the parent group name, but must be
    unique inside the group.

    Returns:
        The number of errors discovered.
    '''
    # TODO: Break this up ...
    # pylint: disable=too-many-locals,too-many-branches,too-many-statements
    cnt_db = CounterDatabase.g_counter_info_db
    assert cnt_db is not None

    sl_db = CounterDatabase.g_semantic_layout_db
    assert sl_db is not None

    errors = 0

    seen_sections: dict[str, int] = {}
    seen_groups: dict[str, list] = {}

    # Check for uniqueness
    for section in sl_db:
        sname = section.name

        # This is caught using load-time asserts currently
        if sname in seen_sections:
            reason = 'Duplicate section in SemanticLayout'
            print_err_c(reason, sname)
            errors += 1

        seen_sections[sname] = 0

        for group in section:
            gname = group.name

            # This is caught using load-time asserts currently
            if gname in seen_groups:
                reason = 'Duplicate group in SemanticLayout'
                print_err_c(reason, gname)
                errors += 1

            seen_groups[gname] = [sname, 0, [], []]
            group_counters = seen_groups[gname][2]

            for counters in group:
                cname = counters.name

                # This is caught using load-time asserts currently
                if cname in group_counters:
                    reason = 'Duplicate counter in SemanticLayout'
                    print_err_c(reason, f'{gname}.{cname}')
                    errors += 1

                group_counters.append(cname)

    # Stop checking if we've had errors already because they snowball
    if errors != 0:
        return errors

    # Check all counters have a group
    cnt_db = CounterDatabase.g_counter_info_db
    assert cnt_db is not None

    for counter in cnt_db:
        gname = counter.group_name

        if gname not in seen_groups:
            reason = 'Missing group in SemanticLayout'
            print_err_c(reason, gname)
            errors += 1
            continue

        sname = seen_groups[gname][0]
        seen_groups[gname][1] += 1
        seen_groups[gname][3].append(counter.group_human_name)
        seen_sections[sname] += 1

    # Check all sections/groups have counters
    for sname, count in seen_sections.items():
        # Detect sections with no counters
        if count == 0:
            reason = 'Extra section in SemanticLayout'
            print_err_c(reason, sname)
            errors += 1

    for gname, (sname, count, sl_names, cnt_names) in seen_groups.items():
        # Detect groups with no counters
        if count == 0:
            reason = 'Extra group in SemanticLayout'
            print_err_c(reason, gname)
            errors += 1

        # Detect extra counters in either direction
        sl_set = set(x for x in sl_names)
        cnt_set = set(x for x in cnt_names)

        sl_only = sl_set.difference(cnt_set)
        cnt_only = cnt_set.difference(sl_set)

        for counter in sl_only:
            reason = 'Extra semantic counter in SemanticLayout'
            print_err_c(reason, f'{gname}.{counter}')
            errors += 1

        for counter in cnt_only:
            reason = 'Extra semantic counter in CounterInfo'
            print_err_c(reason, f'{gname}.{counter}')
            errors += 1

    return errors


def validate_semantic_info() -> int:
    '''
    Validate semantic info consistency.

    Other checks will validate that every entry in the counter info has
    loaded a matching semantic info, but not that every info in the database is
    actually used by the layout. This check validates unused entries.

    Returns:
        The number of errors discovered.
    '''
    sl_db = CounterDatabase.g_semantic_layout_db
    sl_sdb = CounterDatabase.g_semantic_section_info_db
    sl_gdb = CounterDatabase.g_semantic_group_info_db
    assert sl_db and sl_sdb and sl_gdb

    errors = 0

    # Check sections
    layout_s = set(x.name for x in sl_db.iter_sections())
    info_s = set(x.name for x in sl_sdb)

    extra_info = info_s - layout_s
    for name in extra_info:
        reason = 'Extra section in SemanticSectionInfos'
        print_err_c(reason, name)
        errors += 1

    # Check groups
    layout_g = set(x.name for x in sl_db.iter_groups())
    info_g = set(x.name for x in sl_gdb)

    extra_info = info_g - layout_g
    for name in extra_info:
        reason = 'Extra section in SemanticGroupInfos'
        print_err_c(reason, name)
        errors += 1

    return errors


def validate_gpu_fields() -> int:
    '''
    Validate GPU field usage is consistent.

    * Only valid database keys must be used as a GPU ID.

    Returns:
        The number of errors discovered.
    '''
    errors = 0
    keys = set(x for x in CounterDatabase.get_supported_database_keys())

    cnt_db = CounterDatabase.g_counter_info_db
    assert cnt_db

    for counter in cnt_db:
        for gpu in counter.gpu_support:
            if gpu in keys:
                continue

            cname = counter.machine_name
            reason = 'Bad GPU for CounterInfo'
            print_err_gc(reason, gpu, cname)
            errors += 1

    si_db = CounterDatabase.g_semantic_section_info_db
    assert si_db

    for section in si_db:
        for gpu in section.gpu_support:
            if gpu in keys:
                continue

            name = section.name
            reason = 'Bad GPU for SemanticSectionInfo'
            print_err_gc(reason, gpu, name)
            errors += 1

    gr_db = CounterDatabase.g_semantic_group_info_db
    assert gr_db

    for group in gr_db:
        for gpu in group.gpu_support:
            if gpu in keys:
                continue

            name = group.name
            reason = 'Bad GPU for GroupSectionInfo'
            print_err_gc(reason, gpu, name)
            errors += 1

    return errors


def validate_units_fields() -> int:
    '''
    Validate units field usage is consistent.

    * Only valid database keys must be used as a GPU ID.

    Returns:
        The number of errors discovered.
    '''
    cnt_db = CounterDatabase.g_counter_info_db
    assert cnt_db

    errors = 0

    valid_units = set((
        # Standard units
        'percent',
        # Usage
        'beats',
        'cycles',
        'issues',
        # Sizes
        'bits',
        'bytes',
        # Rates
        'bytes/second',
        # Things
        'boxes',
        'blocks',
        'instances',
        'instructions',
        'interrupts',
        'jobs',
        'pixels',
        'primitives',
        'quads',
        'requests',
        'tasks',
        'tests',
        'tiles',
        'threads',
        'transactions',
        'warps',
        'batches',
        'nodes',
        'triangles',
        'rays',
    ))

    for counter in cnt_db:
        if counter.units not in valid_units:
            cname = counter.machine_name
            reason = 'Bad units for CounterInfo'
            print_err_cd(reason, counter.units, cname)
            errors += 1

    return errors


def validate_derived_equation_fields() -> int:
    '''
    Validate GPU equations parsed correctly.

    Returns:
        The number of errors discovered.
    '''
    cnt_db = CounterDatabase.g_counter_info_db
    assert cnt_db is not None

    errors = 0

    # Detect counters where AST compilation failed
    for counter in cnt_db:
        if not counter.equation_text or counter.equation_ast:
            continue

        assert counter.equation_ast_error

        cname = counter.machine_name
        reason = 'Bad equation for CounterInfo'
        detail = textwrap.indent(counter.equation_ast_error, ' ' * 4)

        print_err_cd(reason, f'\n{detail}', cname)
        errors += 1

    return errors


def validate_name_uniqueness(gpu: str) -> int:
    '''
    Validate that names are unique for any given GPU.

    Args:
        gpu: GPU database key to check.

    Returns:
        The number of errors discovered.
    '''
    cnt_db = CounterDatabase.g_counter_info_db
    assert cnt_db is not None

    errors = 0

    found_source_names = set()
    found_machine_names = set()
    found_human_names = set()
    found_group_names = set()

    for counter in cnt_db:
        if not counter.supports_gpu(gpu):
            continue

        # Check source name
        if counter.source_name in found_source_names:
            if not counter.source_name:
                continue

            reason = 'Duplicate SourceName'
            print_err_gc(reason, gpu, counter.source_name)
            errors += 1

        found_source_names.add(counter.source_name)

        # Check machine name
        if counter.machine_name in found_machine_names:
            reason = 'Duplicate MachineName'
            print_err_gc(reason, gpu, counter.machine_name)
            errors += 1

        found_machine_names.add(counter.machine_name)

        # Check human name
        if counter.human_name in found_human_names:
            reason = 'Duplicate HumanName'
            print_err_gc(reason, gpu, counter.human_name)
            errors += 1

        found_human_names.add(counter.human_name)

        # Check human group name
        group_name = f'{counter.group_name}.{counter.group_human_name}'
        if group_name in found_group_names:
            reason = 'Duplicate GroupName/GroupHumanName'
            print_err_gc(reason, gpu, group_name)
            errors += 1

        found_group_names.add(group_name)

    return errors


def validate_source_name_consistency(gpu: str) -> int:
    '''
    Validate SourceName consistency.

    All Source Names in a hardware view must have a counter, and all Counters
    that have a Source Name must be in the hardware view.

    Note that in public releases internal counters may be stripped from both
    databases, but the consistency check must still pass.

    Args:
        gpu: GPU database key to check.

    Returns:
        The number of errors discovered.
    '''
    assert CounterDatabase.g_hardware_layout_db
    hw_db = CounterDatabase.g_hardware_layout_db.get_gpu(gpu)

    cnt_db = CounterDatabase.get_indexed_view_for(gpu)

    errors = 0

    hw_source_names = set(x.name for x in hw_db.iter_counters())
    cnt_source_names = set(x.source_name for x in cnt_db if x.source_name)

    hw_only = hw_source_names.difference(cnt_source_names)
    cnt_only = cnt_source_names.difference(hw_source_names)

    for counter in hw_only:
        reason = 'SourceName only in HardwareView'
        print_err_gc(reason, gpu, counter)
        errors += 1

    for counter in cnt_only:
        reason = 'SourceName only in IndexedView'
        print_err_gc(reason, gpu, counter)
        errors += 1

    return errors


def validate_derived_equation_resolve(gpu: str) -> int:
    '''
    Validate derived equations can resolve.

    Args:
        gpu: GPU database key to check.

    Returns:
        The number of errors discovered.
    '''
    cnt_db = CounterDatabase.get_indexed_view_for(gpu)

    errors = 0

    # Detect counters where AST compilation failed
    for counter in cnt_db:
        if not counter.equation_ast or counter.equation_ast_resolved:
            continue

        assert counter.equation_ast_resolved_error

        cname = counter.machine_name
        reason = 'Bad equation resolve for CounterInfo'
        detail = textwrap.indent(counter.equation_ast_resolved_error, ' ' * 4)
        print_err_gcd(reason, f'\n{detail}', gpu, cname)

        errors += 1

    return errors


class Domain(enum.Enum):
    '''
    Cardinality domains with varying instance count.
    '''
    GPU = 'GPU'
    MEM = 'MEM'
    SC = 'SC'


def validate_derived_equation_cardinality(gpu: str) -> int:
    '''
    Validate that multiblock equations have use appropriate scaling factors.

    Equations that mix counters from multiple hardware blocks in different
    cardinality domains will not give correct results on all configurations
    because we will sum counters across variable numbers of domain instances.

    To give sensible results it is expected that counters are divided by the
    domain instance count to give a normalized count per instance. Once we
    have this, it is valid to compare across domains with expressions returning
    an "on average" result.

    This check crudely validates that expressions that cross domains use the
    necessary scaling factor. However it does not validate that the scaling
    factor is used correctly! Human review is still needed.

    Args:
        gpu: GPU to check.

    Returns:
        The number of errors discovered.
    '''
    cnt_db = CounterDatabase.get_indexed_view_for(gpu)

    errors = 0

    # Mapping of counter blocks to implementation domains
    # Each domain can have an independent instance count in any product config
    domains = {
        HWBlock.GPU_FRONTEND: Domain.GPU,
        HWBlock.TILER: Domain.GPU,
        HWBlock.MEMORY_SYSTEM: Domain.MEM,
        HWBlock.SHADER_CORE: Domain.SC,
    }

    # Recognized domain scaling factors
    scaling_constants = {
        'MALI_CONFIG_L2_CACHE_COUNT': Domain.MEM,
        'MALI_CONFIG_SHADER_CORE_COUNT': Domain.SC,
    }

    # Other constants we can ignore
    other_constants = (
        'MALI_CONFIG_TIME_SPAN',
        'MALI_CONFIG_EXT_BUS_BYTE_SIZE'
    )

    # Manually reviewed exceptions
    exceptions = set((
        'MaliFragOverdraw',
        'MaliSCBusTileWrBPerPx'
    ))

    # Test every expression in the counter database
    for counter in cnt_db:

        # Only process derived counters, skipping those with a parse error
        if not counter.is_derived() or not counter.equation_ast_resolved:
            continue

        # Skip exceptions that are already approved
        if counter.machine_name in exceptions:
            continue

        # Find all used domains and scaling constants
        domain_use = set()
        scale_use = set()

        for node in counter.equation_ast_resolved.find_data('name'):
            var_name = str(node.children[0])

            # Other constants and intermediate nodes we can ignore
            if (var_name in other_constants) or ('(' in var_name):
                continue

            # Detect constant scaling factors
            if var_name in scaling_constants:
                scale_use.add(scaling_constants[var_name])
                continue

            # Detect real counter, which we know must exist
            var_counter = cnt_db.get_by_machine_name(var_name)
            assert var_counter is not None

            index = var_counter.block_type
            assert index is not None

            domain_use.add(domains[index])

        # Nothing to check if expression only uses a single domain
        if len(domain_use) <= 1:
            continue

        # For multi-domain use any domain other than GPU must have matching
        # scaling factor so we can normalize values across domains
        for domain in domain_use:
            if domain == Domain.GPU:
                continue

            if domain not in scale_use:
                reason = f'Missing cardinality scaling for {domain.value}'
                print_err_gc(reason, gpu, counter.machine_name)
                errors += 1

    return errors


def validate_string_resolve(cnt_db: IndexedView, gpu: str, cname: str,
                            docs: str, source: str) -> int:
    '''
    Validate documentation references can resolve for a single doc string.

    Args:
        cnt_db: Index to use to resolve counters.
        gpu: GPU to validate.
        cname: Counter name to validate.
        docs: String to validate.
        source: Source type we're validating

    Returns:
        The number of errors discovered.
    '''
    pattern = re.compile('{{(.*?)}}')

    valid_constants = ('GPU_NAME', )
    valid_parts = ('', 'equation')

    errors = 0

    # Check all matches
    for match in pattern.finditer(docs):
        matched = match.group(1)

        # Validate reference pattern is legal
        ref_type, _, reference = matched.partition('::')

        ref_name, _, ref_part = reference.partition('.')

        # Validate literal constants
        if ref_type == 'K':
            if ref_name not in valid_constants:
                reason = f'Bad reference constant for {source} {matched}'
                print_err_gc(reason, gpu, cname)
                errors += 1

            if ref_part != '':
                reason = f'Bad reference constant part for {source} {matched}'
                print_err_gc(reason, gpu, cname)
                errors += 1

        # Validate counter references
        elif ref_type == 'C':
            if ref_part not in valid_parts:
                reason = f'Bad doc reference postfix for {source} {matched}'
                print_err_gc(reason, gpu, cname)
                errors += 1

            # Validate symbolic reference
            if not cnt_db.get_by_machine_name(ref_name):
                reason = f'Bad doc reference target for {source}'
                print_err_gc(reason, gpu, cname)
                errors += 1

        # Unknown type
        else:
            reason = f'Bad reference type for {source} {matched}'
            print_err_gc(reason, gpu, cname)
            errors += 1

    return errors


def validate_counter_documentation_resolve(gpu: str) -> int:
    '''
    Validate documentation references can resolve.

    Args:
        gpu: GPU database key to check.

    Returns:
        The number of errors discovered.
    '''
    cnt_db = CounterDatabase.get_indexed_view_for(gpu)

    errors = 0

    # Check counter documentation
    for counter in cnt_db:
        cname = counter.machine_name
        docs = counter.long_description
        source = 'CounterInfo'
        errors += validate_string_resolve(cnt_db, gpu, cname, docs, source)

    return errors


def validate_semantic_documentation_resolve(gpu: str) -> int:
    '''
    Validate documentation references can resolve.

    Args:
        gpu: GPU database key to check.

    Returns:
        The number of errors discovered.
    '''
    cnt_db = CounterDatabase.get_indexed_view_for(gpu)
    sem_db = CounterDatabase.get_semantic_view_for(gpu)

    errors = 0

    # Check section documentation
    for section in sem_db.iter_sections():
        cname = section.name
        docs = section.long_description
        source = 'SemanticSectionInfo'
        errors += validate_string_resolve(cnt_db, gpu, cname, docs, source)

    # Check group documentation
    for group in sem_db.iter_groups():
        cname = group.name
        docs = group.long_description
        source = 'SemanticGroupInfo'
        errors += validate_string_resolve(cnt_db, gpu, cname, docs, source)

    return errors


def parse_cli():
    '''
    Parse the command line.

    Returns:
        Return an argparse results object.
    '''
    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--overwrite', action='store_true', default=False,
        help='overwrite database files to apply suggested edits')

    args = parser.parse_args()
    return args


def main():
    '''
    The main function.

    Returns:
        The application exit code.
    '''
    args = parse_cli()

    errors = 0

    database_keys = CounterDatabase.get_supported_database_keys()

    # Validation on the full database
    errors += validate_whitespace()
    errors += validate_absence_of_resolves()
    errors += validate_vulkan_string_lengths()
    errors += validate_stable_ids()
    errors += validate_gpu_fields()
    errors += validate_units_fields()
    errors += validate_derived_equation_fields()
    errors += validate_semantic_layout()
    errors += validate_semantic_info()

    for gpu in database_keys:
        errors += validate_name_uniqueness(gpu)
        errors += validate_source_name_consistency(gpu)
        errors += validate_derived_equation_resolve(gpu)
        errors += validate_derived_equation_cardinality(gpu)
        errors += validate_counter_documentation_resolve(gpu)
        errors += validate_semantic_documentation_resolve(gpu)

    # Pretty print everything
    if args.overwrite:
        arch_db = CounterDatabase.g_architecture_info_db
        arch_db.to_file()

        cnt_db = CounterDatabase.g_counter_info_db
        cnt_db.to_files()

        sem_db = CounterDatabase.g_semantic_layout_db
        sem_db.to_file()

        sem_si_db = CounterDatabase.g_semantic_section_info_db
        sem_si_db.to_file()

        sem_sg_db = CounterDatabase.g_semantic_group_info_db
        sem_sg_db.to_file()

    if errors > 0:
        print(f'ERROR: Total of {errors} failures detected')
        return 1

    return 0


if __name__ == '__main__':
    sys.exit(main())
