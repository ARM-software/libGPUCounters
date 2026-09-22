#!/usr/bin/env python3
#
# Copyright (c) 2025-2026 Arm Limited.
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
This file is an exporter for Mali GPU counter reference documentation, which
can be published directly on GitHub.
'''

import argparse
import datetime
import os
import pathlib
import re
import shutil
import sys
from typing import Optional

from lgcpy import CounterDatabase, CounterView, CounterVisibility, \
                  IndexedView, SemanticSectionView, SemanticGroupView, \
                  ProductInfo

import lgcpy.docutils as du
import lgcpy.equationutils as eu


def is_number(value) -> bool:
    '''
    Utility function to test if a string is a number.

    Returns:
        True if numeric, False otherwise.
    '''
    try:
        float(value)
        return True
    except ValueError:
        return False


def show_progress(value: int, max_value: int) -> None:
    '''
    Show a progress bar.

    Args:
        value: The current progress.
        max_value: The total progress required.
    '''
    if not sys.stdout.isatty():
        return

    progress_len = 20
    progress_bar = [' ' for x in range(0, progress_len)]

    percent = (float(value) / float(max_value)) * 100.0

    current_len = int((float(value) / float(max_value)) * progress_len)
    for i in range(0, current_len):
        progress_bar[i] = '='

    label = f'  [{"".join(progress_bar)}] {percent:0.1f}%'
    print(label, flush=True, end='\r')


def find_images(path: str) -> set[str]:
    '''
    Find all SVGs in a directory tree.

    Args:
        path: Base directory.

    Returns:
        The set of found image files.
    '''
    base_dir = pathlib.Path(path)

    paths = set()

    for image_path in base_dir.rglob('*.svg'):
        image_path = image_path.relative_to(base_dir)
        paths.add(f'./images/{image_path.as_posix()}')

    return paths


def remove_empty_dirs(path: pathlib.Path) -> None:
    '''
    Remove empty directories in a subtree.

    Args:
        path: Base directory.
    '''
    for root, _, _ in os.walk(path, topdown=False):
        root_path = pathlib.Path(root)
        is_empty = not any(root_path.iterdir())
        if is_empty:
            root_path.rmdir()


def replace_markdown_images_with_pictures(document: str) -> str:
    '''
    Replace generated HTML images with light/dark picture selections.

    Args:
        document: Generated HTML from a Markdown document.

    Returns:
        Modified HTML with image paragraphs replaced by picture blocks.
    '''
    image_pattern = re.compile(
        r'<p>\s*'
        r'<img alt="(?P<alt>[^"]*)" '
        r'src="(?P<light>\./images/light/[^"]+\.svg)"\s*/>'
        r'\s*</p>'
    )

    def replace(match: re.Match[str]) -> str:
        alt_text = match.group('alt')
        light_path = match.group('light')
        dark_path = light_path.replace('/light/', '/dark/', 1)

        return '\n'.join([
            '<picture>',
            f'  <source media="(prefers-color-scheme: dark)" '
            f'srcset="{dark_path}">',
            f'  <source media="(prefers-color-scheme: light)" '
            f'srcset="{light_path}">',
            f'  <img src="{light_path}" alt="{alt_text}">',
            '</picture>'
        ])

    return image_pattern.sub(replace, document)


def generate_file_name(gpu: ProductInfo) -> str:
    '''
    Get the file name for the document.

    Args:
        gpu: The GPU to generate a file name for.

    Returns:
        The file name.
    '''
    name = gpu.get_document_name()
    assert name is not None

    name = name.replace(' ', '_')
    name = name.lower()
    return f'{name}_counter_reference.html'


def generate_documentation_aliases_list(gpu: ProductInfo) -> list[str]:
    '''
    Get the list of GPUs which have common documentation with this GPU.

    Args:
        gpu: The GPU to generate a file name for.

    Returns:
        The file name.
    '''
    product_db = CounterDatabase.get_product_infos()
    names = list(reversed(product_db.get_aliases_for(gpu.database_key)))
    return names


def get_section_class(section: SemanticSectionView) -> str:
    '''
    Get the HTML class for a given section based on counters it contains.

    Args:
        section: The section to scan.

    Returns:
        HTML class snippet.
    '''
    any_normal = False
    any_advanced_app = False
    any_advanced_sys = False

    for group in section:
        for series in group:
            if series.is_advanced_application():
                any_advanced_app = True
            elif series.is_advanced_system() or series.is_internal():
                any_advanced_sys = True
            else:
                any_normal = True

    classes = ['lgc-section']
    if any_advanced_app and not any_normal:
        classes.append('lgc-advanced-app')
    if any_advanced_sys and not any_normal:
        classes.append('lgc-advanced-sys')

    class_string = ' '.join(classes)
    return f'class="{class_string}"'


def get_group_class(group: SemanticGroupView) -> str:
    '''
    Get the HTML class for a given group based on counters it contains.

    Args:
        group: The group to scan.

    Returns:
        HTML class snippet.
    '''
    any_normal = False
    any_advanced_app = False
    any_advanced_sys = False

    for series in group:
        if series.is_advanced_application():
            any_advanced_app = True
        elif series.is_advanced_system() or series.is_internal():
            any_advanced_sys = True
        else:
            any_normal = True

    classes = ['lgc-group']
    if any_advanced_app and not any_normal:
        classes.append('lgc-advanced-app')
    if any_advanced_sys and not any_normal:
        classes.append('lgc-advanced-sys')

    class_string = ' '.join(classes)
    return f'class="{class_string}"'


def get_series_class(series: CounterView) -> str:
    '''
    Get the HTML class for a given counter.

    Args:
        series: The counter to scan.

    Returns:
        HTML class snippet.
    '''
    classes = ['lgc-series']
    if series.is_advanced_application():
        classes.append('lgc-advanced-app')
    elif series.is_advanced_system() or series.is_internal():
        classes.append('lgc-advanced-sys')

    class_string = ' '.join(classes)
    return f'class="{class_string}"'


def generate_introduction_block(index_view: IndexedView) -> str:
    '''
    Generate the HTML for a given group.

    Args:
        index_view: Index view to use for any symbol resolution.

    Returns:
        HTML snippet.
    '''
    a_info = CounterDatabase.get_architecture_info_for(index_view.gpu)

    parts = []

    parts.append('<h2 id="s_introduction">Introduction</h2>')

    # Insert note indicating if document is common to multiple GPUs
    product = CounterDatabase.get_product_info_for(index_view.gpu)
    names = generate_documentation_aliases_list(product)
    if len(names) > 1:
        last_name = names.pop()
        names.append(f'and {last_name}')
        name_str = ', '.join(names)

        parts.append(
            '<div class="alert alert-secondary" role="alert">\n'
            f'  <p>This document applies to {name_str}.</p>\n'
            '</div>')

    description = du.resolve_doc_to_hyperlink(
        a_info.long_description, index_view)

    description = du.to_html_string(description)
    description = replace_markdown_images_with_pictures(description)

    parts.append(description)

    return '\n'.join(parts)


def generate_section_block(index_view: IndexedView,
                           section: SemanticSectionView,
                           anchor_link: str) -> str:
    '''
    Generate the HTML for a given group.

    Args:
        index_view: Index view to use for any symbol resolution.
        section: Section to generate documentation for.
        anchor_link: Prebuilt snippet for clipboard links.

    Returns:
        HTML snippet.
    '''
    parts = []

    hanchor = f'id="{section.get_anchor()}"'
    parts.append(f'<h2 {hanchor}>{section.name}{anchor_link}</h2>')

    description = du.resolve_doc_to_hyperlink(
        section.long_description, index_view)

    parts.append(du.to_html_string(description))

    return '\n'.join(parts)


def generate_group_block(index_view: IndexedView,
                         group: SemanticGroupView,
                         anchor_link: str) -> str:
    '''
    Generate the HTML for a given group.

    Args:
        index_view: Index view to use for any symbol resolution.
        group: Group to generate documentation for.
        anchor_link: Prebuilt snippet for clipboard links.

    Returns:
        HTML snippet.
    '''
    parts = []

    hanchor = f'id="{group.get_anchor()}"'
    parts.append(f'<h3 {hanchor}>{group.name}{anchor_link}</h3>')

    description = du.resolve_doc_to_hyperlink(
        group.long_description, index_view)

    parts.append(du.to_html_string(description))

    return '\n'.join(parts)


def generate_series_block(index_view: IndexedView,
                           counter: CounterView,
                           anchor_link: str) -> str:
    '''
    Generate the HTML for a given counter.

    Args:
        index_view: Index view to use for any symbol resolution.
        counter: Counter to generate documentation for.
        anchor_link: Prebuilt snippet for clipboard links.

    Returns:
        HTML snippet.
    '''

    def emit_lookup_name(name: Optional[str], fmt: str, cls: str) -> None:
        # Skip if name doesn't exist in this format
        if not name:
            return

        parts.append(f'<div class="lgc-equation-format-{cls}">')
        parts.append(f'{fmt} name: <code>{name}</code>')
        parts.append('</div>')

    def emit_equation(eqn: str, fmt: str, cls: str) -> None:
        parts.append('<div><div class="lgc-equation">')
        parts.append(f'<div class="lgc-equation-format-{cls}">')
        parts.append(f'<p>{fmt} derivation:</p>')
        parts.append(f'<pre><code>{eqn}</code></pre>')
        parts.append('</div>')
        parts.append('</div></div>')

    # Build resolved heading
    hanchor = f'id="{counter.get_anchor()}"'
    cname = counter.group_human_name
    heading = f'<h4 {hanchor}>{cname}{anchor_link}</h4>'

    # Build resolved description
    description = du.resolve_doc_to_hyperlink(
        counter.long_description, index_view)

    description = du.to_html_string(description)

    parts = [
        heading,
        description
    ]

    # Build original lookup name
    lgc_name = counter.machine_name
    emit_lookup_name(lgc_name, 'libGPUCounters', 'lgc')

    if not counter.is_derived():
        sl_name = eu.get_streamline_expression(index_view, counter)
        emit_lookup_name(sl_name, 'Streamline', 'sl')

        hw_name = counter.source_name
        emit_lookup_name(hw_name, 'Hardware', 'hw')

    # Build full derivation
    if counter.is_derived():
        # Get the AST expanded for the current GPU
        equation = eu.get_machine_name_expression(index_view, counter)
        emit_equation(equation, 'libGPUCounters', 'lgc')

        equation = eu.get_streamline_expression(index_view, counter)
        emit_equation(equation, 'Streamline', 'sl')

        equation = eu.get_source_name_expression(index_view, counter)
        emit_equation(equation, 'Hardware', 'hw')

    return '\n'.join(parts)


def generate_index_payload(gpus: list[str]) -> dict[str, list[list[str]]]:
    '''
    Generate a list of GPU payloads for the index HTML page.

    Args:
        gpus: List of user-requested product names.

    Returns:
        Ordered dict of architectures, oldest to newest in terms of
        presentation order. Values are extracted GPU payloads, also ordered
        oldest to newest in terms of presentation order (not quite time order).

        Entries are:
         - arch name
         - release year
         - product name
         - product codename
         - HTML file name
    '''
    products = CounterDatabase.get_product_infos()
    arch_products: dict[str, list[list[str]]] = {}
    active_products = set()

    for gpu in gpus:
        product = products.get_gpu(gpu)

        arch = str(product.architecture)
        name = product.get_document_name()

        # Skip GPUs that are aliased for documentation purposes
        # Should never be None here, but this keeps mypy happy
        if (name is None) or (name in active_products):
            continue

        active_products.add(name)

        if arch not in arch_products:
            arch_products[arch] = []

        project_name = 'Unknown'
        if product.project_name:
            project_name = product.project_name.title()

        arch_products[arch].insert(
            0, [arch,
                str(product.release_year),
                name,
                project_name,
                generate_file_name(product)])

    return arch_products


def generate_counter_index(out_dir: pathlib.Path,
                           gpus: list[str],
                           add_codenames: bool) -> None:
    '''
    Generate the counter index page.

    Output is written directly into the file system by this function.

    Args:
        out_dir: The output directory to write the file in to.
        gpus: List of all GPU documentation built.
        add_codenames: Add codenames, if available, to the index table.
    '''
    # GPU data
    arch_products = generate_index_payload(gpus)

    # Load template
    file_name = out_dir / 'index_template.html'
    with open(file_name, encoding='utf-8') as handle:
        document = handle.read()

    page_content = []

    # Generate table
    page_content.append('<table class="table table-striped">')
    page_content.append('<thead>')

    data = [
        'Architecture',
        'Release Year',
        'Performance Counter Guide'
    ]

    if add_codenames:
        data.insert(2, 'Codename')

    cells = [f'<th>{x}</th>' for x in data]
    cells = ['<tr>', *cells, '</tr>']
    page_content.append(''.join(cells))

    page_content.append('</thead>')
    page_content.append('<tbody class="table-group-divider">')

    for product_list in reversed(arch_products.values()):
        for i, product in enumerate(product_list):
            link = f'<a href="{product[4]}">{product[2]} Counter Guide</a>'

            if i == 0:
                cells = [
                    f'<td rowspan="{len(product_list)}">{product[0]}</td>',
                    f'<td>{product[1]}</td>',
                    f'<td>{link}</td>',
                ]

            else:
                cells = [
                    f'<td>{product[1]}</td>',
                    f'<td>{link}</td>',
                ]

            if add_codenames:
                cells.insert(-1, f'<td>{product[3]}</td>')

            cells = ['<tr>', *cells, '</tr>']
            page_content.append(''.join(cells))

    page_content.append('</tbody>')
    page_content.append('</table>')

    # Populate template
    document = document.replace('{{COPYRIGHT_YEAR}}',
                                str(datetime.date.today().year))

    document = document.replace('{{PAGE_CONTENT}}',
                                '\n'.join(page_content))

    # Save populated template
    file_name = out_dir / 'index.html'
    with open(file_name, 'w', encoding='utf-8') as handle:
        handle.write(document)


def populate_counter_reference_template(
        template_dir: pathlib.Path, product: ProductInfo, page_content: str,
        build: str) -> str:
    '''
    Populate a text template with document content.

    Args:
        template_dir: The directory containing the template file.
        product: The GPU we are generate the document for.
        page_content: The page content to inject.
        build: The build type of the document.

    Returns:
        The populated template.
    '''
    file_name = template_dir / 'gpu_template.html'
    with open(file_name, encoding='utf-8') as handle:
        document = handle.read()

    gpu_name = product.get_document_name()
    assert gpu_name

    generate_documentation_aliases_list(product)

    gpu_name_full = gpu_name
    if (build != 'public') and product.project_name:
        project_name = product.project_name.title()
        gpu_name_full = f'{gpu_name_full} ({project_name})'

    gpu_name_tm = gpu_name.replace('Mali', 'Mali™')
    gpu_name_tm = gpu_name_tm.replace('Immortalis', 'Immortalis™')

    document = document.replace('{{CONFIDENTIAL}}',
                                '' if (build == 'public') else 'CONFIDENTIAL ')

    document = document.replace('{{GPU_NAME_FULL}}',
                                gpu_name_full)

    document = document.replace('{{GPU_NAME_TM}}',
                                gpu_name_tm)

    document = document.replace('{{GPU_NAME}}',
                                gpu_name)

    document = document.replace('{{COPYRIGHT_YEAR}}',
                                str(datetime.date.today().year))

    document = document.replace('{{PAGE_CONTENT}}',
                                page_content)

    # Check all references have been replaced
    assert '{{' not in document

    return document


def generate_counter_reference(out_dir: pathlib.Path,
                               gpu: str, build: str) -> set[str]:
    '''
    Generate the full counter reference guide for a single GPU.

    Output is written directly into the file system by this function. Note
    that this document is confidential, and includes ALL counters and
    derivations for the targeted hardware.

    Args:
        out_dir: The output directory to write the file in to.
        gpu: The GPU name to build a document for, can be an alias.
        build: The build type of the document.

    Returns:
        Set of used image file names.
    '''
    # Preload resources
    product = CounterDatabase.get_product_info_for(gpu)
    index_view = CounterDatabase.get_indexed_view_for(gpu)
    sem_view = CounterDatabase.get_semantic_view_for(gpu)

    # Hide internal counters in release builds
    if build != 'internal':
        sem_view = sem_view.filter(CounterVisibility.ADVANCED_SYSTEM, True)

    page_content = []
    page_content.append('<div class="lgc-section">')

    page_content.append(
        generate_introduction_block(index_view)
    )

    page_content.append('</div>')

    tooltip_config = 'data-bs-toggle="tooltip" ' \
                     'data-bs-delay="400" ' \
                     'data-bs-placement="right" ' \
                     'title="Copy to clipboard"'

    anchor_link = f' <span {tooltip_config}>' \
                  '<a class="lgc-link-copy link-secondary">' \
                  '<i class="fa-solid fa-link"></i>' \
                  '</a>' \
                  '</span>'

    for section in sem_view:
        # Start section
        page_content.append(f'<div {get_section_class(section)}>')

        page_content.append(
            generate_section_block(index_view, section, anchor_link)
        )

        for group in section:
            # Start group
            page_content.append(f'<div {get_group_class(group)}>')

            page_content.append(
                generate_group_block(index_view, group, anchor_link)
            )

            for series in group:
                # Start series
                page_content.append(f'<div {get_series_class(series)}>')

                page_content.append(
                    generate_series_block(index_view, series, anchor_link)
                )

                # Stop series
                page_content.append('</div>')

            # Stop group
            page_content.append('</div>')

        # Stop section
        page_content.append('</div>')

    # Products that disallow documents should be stopped earlier
    document_name = product.get_document_name()
    assert document_name

    document = populate_counter_reference_template(
        out_dir, product, '\n'.join(page_content), build)

    file_name = out_dir / generate_file_name(product)
    with open(file_name, 'w', encoding='utf-8') as handle:
        handle.write(document)

    return set(re.findall(r'./images/.*/.*\.svg', document))


def is_enabled(args, gpu):
    '''
    Determine if the selected GPU is enabled.

    Args:
        args: Script parameters
        gpu: GPU to test.

    Return:
        True if GPU is enabled, False if skipped.
    '''
    product = CounterDatabase.get_product_info_for(gpu)

    # Never generate docs if turned off in the configuration
    if not product.get_document_name():
        return False

    # Never generate docs if not the primary configuration
    if gpu != product.names[0]:
        return False

    # Never emit confidential products in release builds
    if not product.is_public() and args.build == 'public':
        return False

    # Did the user select this?
    return any(x for x in product.names if x in args.gpu)


def parse_cli() -> argparse.Namespace:
    '''
    Parse the command line.

    Returns:
        Return an argparse results object.
    '''
    parser = argparse.ArgumentParser()

    # Not all GPUs generate documents so hide the ones that don't
    choices1: list[str] = []
    for gpu in CounterDatabase.get_supported_gpus():
        product = CounterDatabase.get_product_info_for(gpu)
        if product.get_document_name():
            choices1.append(product.names[0])

    parser.add_argument(
        '-o', '--output', type=str, required=True,
        help='destination directory for outputs')

    parser.add_argument(
        '-g', '--gpu', type=str, action='append', choices=choices1, default=[],
        help='target gpu')

    choices2 = ('internal', 'partner', 'public')
    parser.add_argument(
        '--build', choices=choices2, required=True,
        help='configure the build type required')

    parser.add_argument(
        '--overwrite', action='store_true', default=False,
        help='delete all existing content in the output directory')

    args = parser.parse_args()

    if not args.gpu:
        args.gpu = choices1

    args.output = pathlib.Path(args.output)

    return args


def main() -> int:
    '''
    The main function.

    Returns:
        The application exit code.
    '''
    args = parse_cli()

    # Create the list of GPUs to emit
    gpu_list = CounterDatabase.get_supported_gpus()
    gpu_list = [x for x in gpu_list if is_enabled(args, x)]

    if not gpu_list:
        print('ERROR: No GPUs remain after build type filtering')
        return 1

    # Set up directory structure
    if args.output.exists():
        if not args.overwrite:
            print('ERROR: Output path exists and --overwrite not specified')
            return 1
        shutil.rmtree(args.output)

    # Clone bulk documentation framework

    # Find the template dir to use
    root = pathlib.Path(__file__).parent
    docs_template_opts = [
        root / 'data' / 'docs_github',         # Build from CGen repo
        root.parent / 'data' / 'docs_github',  # Build from CGen release
        root.parent / 'docs'                   # Build from libGPUCounters repo
    ]

    for docs_template_dir in docs_template_opts:
        if docs_template_dir.exists():
            break
    else:
        assert False, 'ERROR: No documentation template directory found'

    ignore = ['index.html', 'mali*']
    shutil.copytree(docs_template_dir, args.output,
                    ignore=shutil.ignore_patterns(*ignore))

    # Generate index documentation
    generate_counter_index(args.output, gpu_list, args.build != 'public')

    # Generate per-GPU documentation, tracking used image files
    used_images = set()

    for i, gpu in enumerate(gpu_list):
        images = generate_counter_reference(args.output, gpu, args.build)

        # Ignore light/dark differences for this check
        image_dirs = {
            str(pathlib.PurePosixPath(x).parent).replace('/dark/', '/light/')
            for x in images
        }
        assert len(image_dirs) <= 1, f'Mixed image directories {image_dirs}'

        used_images.update(images)
        show_progress(i + 1, len(gpu_list))

    print('\n')

    # Check used images exist
    for image_file in used_images:
        image_path = os.path.join(args.output, image_file)
        if not os.path.exists(image_path):
            print(f'ERROR: Missing image: {image_file}')
            return 1

    # Remove unused images
    found_images = find_images(os.path.join(args.output, 'images'))
    unused_images = found_images - used_images
    for image_path in unused_images:
        remove_path = pathlib.Path(args.output) / image_path
        remove_path.unlink()

    # Remove unused directories if all images in them are deleted
    remove_empty_dirs(pathlib.Path(args.output) / 'images')

    return 0


if __name__ == '__main__':
    sys.exit(main())
