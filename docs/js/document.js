/*
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
*/

var dataModel = null;

// Are we showing advanced application counters?
var showAdvancedApp = false;

// Are we showing advanced systems counters?
var showAdvancedSys = false;

// Are we showing equations?
var showEquations = false;

// We are using libGPUCounters dialect?
var useDialect = 'libgpucounters';

// Is filtering temporarily showing a different dialect?
var temporaryDialect = null;

// Is filtering temporarily showing advanced counters?
var temporaryAdvancedApp = false;
var temporaryAdvancedSys = false;

// What is the current text filter?
var currentFilter = ''

/**
 * Compile user filter text as a case-insensitive regular expression.
 *
 * Invalid expressions are treated as literal text so that partially typed
 * patterns do not break live filtering.
 */
function compileFilter(filter) {
    if (filter == null || filter instanceof RegExp) {
        return filter;
    }

    if (filter == '') {
        return null;
    }

    try {
        return new RegExp(filter, 'i');
    } catch (error) {
        var literalFilter = filter.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
        return new RegExp(literalFilter, 'i');
    }
}

class DataModelNode {

    constructor(nodeType, elem, name, searchAliases=[], dialectSearchNames={}) {
        this.nodeType = nodeType;
        this.elem = elem;
        this.menuElem = null;
        this.name = name;

        this.commonSearchNames = [name, ...searchAliases].filter(
            searchName => searchName != null && searchName != '');

        this.dialectSearchNames = dialectSearchNames;
        this.searchNames = [...this.commonSearchNames];

        for (var dialectNames of Object.values(this.dialectSearchNames)) {
            this.searchNames.push(...dialectNames.filter(
                searchName => searchName != null && searchName != ''));
        }

        this.childNodes = [];
    }

    collectFilterMatches(filter, matches) {
        var filterRegex = compileFilter(filter);
        if (filterRegex == null) {
            return;
        }

        if (this.commonSearchNames.some(
                searchName => filterRegex.test(searchName))) {
            matches.common = true;
        }

        for (var [dialect, searchNames] of Object.entries(this.dialectSearchNames)) {
            if (searchNames.some(searchName => filterRegex.test(searchName))) {
                matches.dialects.add(dialect);
            }
        }

        for (var child of this.childNodes) {
            child.collectFilterMatches(filterRegex, matches);
        }
    }

    collectFilterResultTypes(matches) {
        if (this.nodeType == 'Series') {
            if (this.isFiltered()) {
                return;
            }

            if (this.isAdvancedApp()) {
                matches.advancedApp = true;
            } else if (this.isAdvancedSys()) {
                matches.advancedSys = true;
            } else {
                matches.novice = true;
            }

            return;
        }

        for (var child of this.childNodes) {
            child.collectFilterResultTypes(matches);
        }
    }

    propagateFilter(filter) {
        var filterRegex = compileFilter(filter);

        // Determine if this node is filtered based on regex match
        var filtered = false;
        if (this.searchNames.length != 0 && filterRegex != null) {
            filtered = !this.searchNames.some(
                searchName => filterRegex.test(searchName));
        }

        // If this node is not filtered then always keep its children
        if (this.name != null && filtered == false) {
            for(var child of this.childNodes) {
                child.propagateFilter(null);
            }
        // Else filter the children based on the filter string
        } else {
            for(var child of this.childNodes) {
                child.propagateFilter(filterRegex);
            }
        }

        // Determine if all children are filtered
        var childrenFiltered = this.childNodes.length != 0;
        for(var child of this.childNodes) {
            if (!child.isFiltered()) {
                childrenFiltered = false;
                break;
            }
        }

        // Do we need to hide this node?
        // Hide if all children are filtered if we have children or if
        // self if filtered if we don't

        if ((filtered && this.childNodes.length == 0) || childrenFiltered) {
            $(this.elem).addClass('lgc-filtered');
            $(this.menuElem).addClass('lgc-filtered');
        } else {
            $(this.elem).removeClass('lgc-filtered');
            $(this.menuElem).removeClass('lgc-filtered');
        }
    }

    getID() {
        return $(this.elem).children(':first').attr('id');
    }

    isNovice() {
        return !this.isAdvancedApp() && !this.isAdvancedSys();
    }

    isAdvancedApp() {
        return $(this.elem).hasClass('lgc-advanced-app');
    }

    isAdvancedSys() {
        return $(this.elem).hasClass('lgc-advanced-sys');
    }

    isFiltered() {
        return $(this.elem).hasClass('lgc-filtered');
    }

    addChildNode(node) {
        this.childNodes.push(node)
    }

    debugLog(depth=0) {
        if (this.name != null) {
            var prefix = ' '.repeat(depth * 2);
            console.log(depth + ':' + prefix + '-' + this.name);
        }

        for (var child of this.childNodes) {
            child.debugLog(depth + 1);
        }
    }
}

/*
 * Build a data model for a single counter series.
 */
function buildDataModelSeries(parent, elem) {
    // Build this series
    var heading = $(elem).find('>:first-child');

    // Lookup names are already emitted as direct children of the series.
    // Direct-child selection avoids treating derived equations as names.
    var machineName = $(elem).children(
        '.lgc-equation-format-lgc').children('code').first().text();
    var streamlineName = $(elem).children(
        '.lgc-equation-format-sl').children('code').first().text();
    var hardwareName = $(elem).children(
        '.lgc-equation-format-hw').children('code').first().text();

    // Counter anchors have the form "c_<StableID>".
    var anchor = $(heading).attr('id');
    var stableID = anchor && anchor.startsWith('c_')
        ? anchor.substring(2)
        : null;


    var searchAliases = [stableID];

    var dialectSearchNames = {
        'libgpucounters': [machineName],
        'streamline': [streamlineName],
        'hardware': [hardwareName],
    };

    var series = new DataModelNode(
        'Series', elem, $(heading).text().trim(), searchAliases, dialectSearchNames)

    parent.addChildNode(series);
}

/*
 * Build a data model for a single counter group.
 */
function buildDataModelGroup(parent, elem) {
    // Build this group
    heading = $(elem).find('>:first-child');
    var group = new DataModelNode('Group', elem, $(heading).text().trim())
    parent.addChildNode(group);

    // Find and add series
    $(elem).find('.lgc-series').each(function() {;
        buildDataModelSeries(group, this)
    });
}

/*
 * Build a data model for a single counter section.
 */
function buildDataModelSection(parent, elem) {
    // Build this section
    heading = $(elem).find('>:first-child');
    var section = new DataModelNode('Section', elem, $(heading).text().trim())
    parent.addChildNode(section);

    // Find and add groups
    $(elem).find('.lgc-group').each(function() {;
        buildDataModelGroup(section, this)
    });
}

/*
 * Build a data model representing the counter hierarchy in the document.
 */
function buildDataModel() {
    // Build the raw data model
    var data = new DataModelNode('Document', null, null);
    $('.lgc-section').each(function() {
        buildDataModelSection(data, this);
    });

    return data;
}

/**
 * Return the only dialect matched by a filter, or null if the match is
 * generic, ambiguous, or absent.
 */
function getUnambiguousFilterDialect(filter) {
    var filterRegex = compileFilter(filter);
    if (filterRegex == null) {
        return null;
    }

    var matches = {
        common: false,
        dialects: new Set(),
    };
    dataModel.collectFilterMatches(filterRegex, matches);

    if (matches.common || matches.dialects.size != 1) {
        return null;
    }

    return matches.dialects.values().next().value;
}

/**
 * Show a single naming dialect without changing the user's selection.
 */
function showDialect(dialect) {
    $('.lgc-equation-format-lgc').hide();
    $('.lgc-equation-format-sl').hide();
    $('.lgc-equation-format-hw').hide();

    if (dialect == 'libgpucounters') {
        $('.lgc-equation-format-lgc').show();
    } else if (dialect == 'streamline') {
        $('.lgc-equation-format-sl').show();
    } else if (dialect == 'hardware') {
        $('.lgc-equation-format-hw').show();
    }
}

/**
 * Apply or clear a temporary filter-driven dialect override.
 */
function updateFilterDialect(filter) {
    var matchedDialect = getUnambiguousFilterDialect(filter);
    if (matchedDialect != null && matchedDialect != useDialect) {
        temporaryDialect = matchedDialect;
    } else {
        temporaryDialect = null;
    }

    var displayedDialect = temporaryDialect || useDialect;
    showDialect(displayedDialect);

    $('input[name="formatRadios"]')
        .removeClass('lgc-temporary-override')
        .prop('checked', false);

    var controlIDs = {
        'libgpucounters': '#lgc-format-lgc',
        'streamline': '#lgc-format-sl',
        'hardware': '#lgc-format-hw',
    };

    $(controlIDs[displayedDialect]).prop('checked', true);

    if (temporaryDialect != null) {
        $(controlIDs[temporaryDialect]).addClass('lgc-temporary-override');
    }
}

/**
 * Show advanced counters based on the manual and temporary selections.
 */
function updateAdvancedVisibility() {
    var effectiveAdvancedApp = showAdvancedApp || temporaryAdvancedApp;
    var effectiveAdvancedSys = showAdvancedSys || temporaryAdvancedSys;

    $('.lgc-advanced-app:not(.lgc-advanced-sys)').toggle(effectiveAdvancedApp);
    $('.lgc-advanced-sys:not(.lgc-advanced-app)').toggle(effectiveAdvancedSys);
    $('.lgc-advanced-app.lgc-advanced-sys').toggle(
        effectiveAdvancedApp || effectiveAdvancedSys);
}

/**
 * Apply or clear temporary filter-driven advanced-counter overrides.
 */
function updateFilterAdvanced(filter) {
    var matches = {
        novice: false,
        advancedApp: false,
        advancedSys: false,
    };

    if (compileFilter(filter) != null) {
        dataModel.collectFilterResultTypes(matches);
    }

    var onlyAdvanced = !matches.novice &&
        (matches.advancedApp || matches.advancedSys);

    temporaryAdvancedApp = onlyAdvanced && matches.advancedApp &&
        !showAdvancedApp;
    temporaryAdvancedSys = onlyAdvanced && matches.advancedSys &&
        !showAdvancedSys;

    updateAdvancedVisibility();

    $('#lgc-enableAdvancedApp')
        .removeClass('lgc-temporary-override')
        .prop('checked', showAdvancedApp || temporaryAdvancedApp);
    $('#lgc-enableAdvancedSys')
        .removeClass('lgc-temporary-override')
        .prop('checked', showAdvancedSys || temporaryAdvancedSys);

    if (temporaryAdvancedApp) {
        $('#lgc-enableAdvancedApp').addClass('lgc-temporary-override');
    }

    if (temporaryAdvancedSys) {
        $('#lgc-enableAdvancedSys').addClass('lgc-temporary-override');
    }
}

/**
 * Highlight the controls drawer when filtering overrides any control.
 */
function updateTemporaryOverrideIndicator() {
    var hasTemporaryOverride = temporaryDialect != null ||
        temporaryAdvancedApp || temporaryAdvancedSys;
    $('.lgc-menu-controls').toggleClass(
        'lgc-temporary-override', hasTemporaryOverride);
}

/**
 * Update all filter-driven control overrides.
 */
function updateFilterOverrides(filter) {
    updateFilterDialect(filter);
    updateFilterAdvanced(filter);
    updateTemporaryOverrideIndicator();
}

function setAdvancedAppFromControl(control) {
    if ($(control).hasClass('lgc-temporary-override')) {
        showAdvancedApp = true;
    } else {
        showAdvancedApp = $(control).is(':checked');
    }
}

function setAdvancedSysFromControl(control) {
    if ($(control).hasClass('lgc-temporary-override')) {
        showAdvancedSys = true;
    } else {
        showAdvancedSys = $(control).is(':checked');
    }
}

function buildContentSubtree(dataNode, domNode) {
    // Leaf node
    if (!dataNode.childNodes.length) {
        var item = $('<li></li>');
        dataNode.menuElem = item.get(0);

        var itemLink = $('<a href="#' + dataNode.getID() + '"></a>');
        itemLink.text(dataNode.name);
        item.append(itemLink);

        if (dataNode.isAdvancedApp()) {
            item.addClass('lgc-advanced-app');
        }

        if (dataNode.isAdvancedSys()) {
            item.addClass('lgc-advanced-sys');
        }

        // Insert at the end to minimize rebuild cost
        domNode.append(item);
        return;
    }

    // Branch node
    var item = $('<li></li>');
    dataNode.menuElem = item.get(0);

    var details = $('<details open></details>');
    item.append(details);

    var summary = $('<summary></summary>');
    details.append(summary)

    var summaryLink = $('<a href="#' + dataNode.getID() + '"></a>');
    summaryLink.text(dataNode.name);
    summary.append(summaryLink);

    var list = $('<ul></ul>');
    details.append(list);

    if (dataNode.isAdvancedApp()) {
        item.addClass('lgc-advanced-app');
    }

    if (dataNode.isAdvancedSys()) {
        item.addClass('lgc-advanced-sys');
    }

    // Insert children
    for (var child of dataNode.childNodes) {
        buildContentSubtree(child, list);
    }

    // Insert at the end to minimize rebuild cost
    domNode.append(item);
}

function buildContents(dataModel) {
    var root = $('#lgc-toc');

    // Build the list
    var list = $('<ul></ul>');

    // Insert children
    for (var child of dataModel.childNodes) {
        buildContentSubtree(child, list);
    }

    // Insert at the end to minimize rebuild cost
    root.append(list);
}

/*
 * Get the base from a URL.
 */
function getUrlBase(uri) {
    var parts = uri.split('?')[0];
    return parts.split('#')[0];
};

/*
 * Get the parameter string from the URL.
 */
function getUrlParams(uri) {
    // Strip bookmarks
    var uriParts = uri.split('#')[0];

    // Split parameters from the remaining string
    uriParts = uriParts.split('?');

    // No parameters found
    if (uriParts.length < 2) {
        return false;
    }

    return uriParts[1];
};

/*
 * Get a bookmark from the URL.
 */
function getUrlBookmark(uri) {
    var uriParts = uri.split('#');

    // No bookmark found or no-op self-link
    if (uriParts.length < 2 || !uriParts[1].length) {
        return false;
    }

    return '#' + uriParts[1];
};

/*
 * Get a parameter from the URL.
 */
function getUrlParam(uri, key) {
    // Get the parameter part of the string
    var uriParts = getUrlParams(uri);
    if (!uriParts) {
        return false;
    }

    // Split into separate parameter values
    var uriParams = uriParts.split('&');
    for (var param of uriParams) {
        paramParts = param.split('=');

        // Key-only parameter, true if set
        if (paramParts.length < 2) {
            if (paramParts[0] == key) {
                return true;
            }
        }

        // Key-value parameter, string value if set
        if (paramParts[0] == key) {
            return decodeURIComponent(paramParts[1]);
        }
    }

    // No matching key found
    return false;
};

/*
 * Update the page URL.
 */
function getQueryString() {
    var queryString = '';

    if (showAdvancedApp) {
        if (queryString) {
            queryString += '&';
        }

        queryString += 'advancedApp=true';
    }

    if (showAdvancedSys) {
        if (queryString) {
            queryString += '&';
        }

        queryString += 'advancedSys=true';
    }

    if (useDialect != 'libgpucounters') {
        if (queryString) {
            queryString += '&';
        }

        queryString += 'dialect=' + useDialect;
    }


    if (showEquations) {
        if (queryString) {
            queryString += '&';
        }

        queryString += 'equations=true';
    }

    if (currentFilter) {
        if (queryString) {
            queryString += '&';
        }

        encodedFilter = encodeURIComponent(currentFilter);
        queryString += 'filter=' + encodedFilter;
    }

    if (queryString) {
        queryString = '?' + queryString;
    }

    return queryString;
}

/*
 * Is a bookmark visible on the page?
 */
function isBookmarkVisible(bookmark) {
    var isAdvancedApp = $(bookmark).parent().hasClass('lgc-advanced-app');
    var isAdvancedSys = $(bookmark).parent().hasClass('lgc-advanced-sys');
    var effectiveAdvancedApp = showAdvancedApp || temporaryAdvancedApp;
    var effectiveAdvancedSys = showAdvancedSys || temporaryAdvancedSys;

    return (!isAdvancedApp || effectiveAdvancedApp) &&
           (!isAdvancedSys || effectiveAdvancedSys);
}

function needAdvancedAppEnable(bookmark) {
    return $(bookmark).parent().hasClass('lgc-advanced-app');
}

function needAdvancedSysEnable(bookmark) {
    return $(bookmark).parent().hasClass('lgc-advanced-sys');
}

/*
 * Update the page URL.
 */
function updatePageUrl() {
    // Update the page history address so user can bookmark it
    var href = window.location.href;
    var uriParts = href.split('#')

    var bookmarkString = '';
    if (uriParts.length >= 2 && uriParts[1].length)
    {
        bookmarkString = '#' + uriParts[1];
    }

    // Hide bookmarks that a user cannot see after the change
    if (bookmarkString.length && !isBookmarkVisible(bookmarkString))
    {
        bookmarkString = '';
    }

    var baseString = uriParts[0].split('?')[0];
    var queryString = getQueryString();
    var updatedUri = baseString + queryString + bookmarkString;
    window.history.replaceState({}, document.title, updatedUri);

    // Update links to persist the query string
    $('a.keep-params').each(function() {
        var href = $(this).attr('href');
        var uriParts = href.split('#')

        // Extract bookmark
        var bookmarkString = '';
        if (uriParts.length >= 2)
        {
            bookmarkString = '#' + uriParts[1];
        }

        var baseString = uriParts[0].split('?')[0];
        var updatedUri = baseString + queryString + bookmarkString;
        $(this).attr('href', updatedUri);
    });
}

/**
 * What is the default color scheme.
 */
function getDefaultTheme() {
  return window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light';
}

/**
 * Patch bootstrap theme to match the default theme.
 */
function setTheme(theme) {
    document.documentElement.setAttribute('data-bs-theme', theme)
}


$(document).ready(function () {
    // Setup Bootstrap theme
    var theme = getDefaultTheme();
    setTheme(theme);

    // Handle theme mode changes
    const colorSchemeQueryList = window.matchMedia('(prefers-color-scheme: dark)');

    const setColorScheme = e => {
        if (e.matches) {
            setTheme('dark');
        } else {
            setTheme('light');
        }
    }

    setColorScheme(colorSchemeQueryList);
    colorSchemeQueryList.addEventListener('change', setColorScheme);

    // Define all of our jQuery handlers
    $('#lgc-filterInput').on('input', function() {
        filter = $(this).val();
        currentFilter = filter;
        dataModel.propagateFilter(filter);
        updateFilterOverrides(filter);

        // TODO: Can we do off return values from propagate?
        filtered = $('.lgc-filtered').length != 0
        if (filtered) {
            $('.lgc-filtered-pill').show();
        } else {
            $('.lgc-filtered-pill').hide();
        }

        updatePageUrl();
    });

    $('#lgc-enableAdvancedApp').change(function() {
        setAdvancedAppFromControl(this);
        updateFilterOverrides(currentFilter);
        updatePageUrl();
    });

    $('#lgc-enableAdvancedSys').change(function() {
        setAdvancedSysFromControl(this);
        updateFilterOverrides(currentFilter);
        updatePageUrl();
    });

    $('#lgc-enableEquations').change(function() {
        if ($(this).is(':checked')) {
            showEquations = true;
            $('.lgc-equation').show();
        }
        else
        {
            showEquations = false;
            $('.lgc-equation').hide();
        }
        updatePageUrl();
    });

    $('#lgc-format-lgc').click(function() {
        if ($(this).is(':checked')) {
            useDialect = 'libgpucounters';
            updateFilterOverrides(currentFilter);
            updatePageUrl();
        }
    });

    $('#lgc-format-sl').click(function() {
        if ($(this).is(':checked')) {
            useDialect = 'streamline';
            updateFilterOverrides(currentFilter);
            updatePageUrl();
        }
    });

    $('#lgc-format-hw').click(function() {
        if ($(this).is(':checked')) {
            useDialect = 'hardware';
            updateFilterOverrides(currentFilter);
            updatePageUrl();
        }
    });

    $('.lgc-link-copy').click(function() {
        var parent = $(this).parent().parent();
        var parentID = parent.attr('id');

        var base = getUrlBase(window.location.href);
        var params = getUrlParams(window.location.href);
        if (params) {
            params = '?' + params;
        } else {
            params = '';
        }

        // Update the clipboard
        var link = base + params + '#' + parentID;
        navigator.clipboard.writeText(link);
    });

    $('a').click(function() {
        var href = $(this).attr('href');
        if (href && !href.startsWith('#'))
        {
            return;
        }

        needAdvancedApp = !isBookmarkVisible(href) &&
                          needAdvancedAppEnable(href);

        needAdvancedSys = !isBookmarkVisible(href) &&
                          needAdvancedSysEnable(href);

        if (needAdvancedApp) {
            $('#lgc-enableAdvancedApp').click()
        }

        if (needAdvancedSys) {
            $('#lgc-enableAdvancedSys').click()
        }

        var advancedAlertElem = $('#liveToast');
        var advancedToast = new bootstrap.Toast(advancedAlertElem);
        advancedToast.show();

        $(href)[0].scrollIntoView();
    });

    var tooltipTriggerList = [].slice.call(document.querySelectorAll('[data-bs-toggle="tooltip"]'))
    tooltipTriggerList.map(function (tooltipTriggerEl) {
        return new bootstrap.Tooltip(tooltipTriggerEl)
    })

    // Create a data model of the counter hierarchy
    dataModel = buildDataModel();

    // Create table of contents from the data model
    buildContents(dataModel);

    // Update page based on URI parameters
    var loadTimeURI = window.location.href;

    var paramBookmark = getUrlBookmark(loadTimeURI);
    var needAdvancedApp = false;
    if (paramBookmark)
    {
        needAdvancedApp = !isBookmarkVisible(paramBookmark) &&
                          needAdvancedAppEnable(paramBookmark);
    }

    var needAdvancedSys = false;
    if (paramBookmark)
    {
        needAdvancedSys = !isBookmarkVisible(paramBookmark) &&
                          needAdvancedSysEnable(paramBookmark);
    }

    var paramFilter = getUrlParam(loadTimeURI, 'filter');
    if (paramFilter)
    {
        $('#lgc-filterInput').val(paramFilter).trigger('input');
    }

    var paramAdvancedApp = getUrlParam(loadTimeURI, 'advancedApp') == 'true';
    if (paramAdvancedApp || needAdvancedApp) {
        $('#lgc-enableAdvancedApp').click()
    }

    var paramAdvancedSys = getUrlParam(loadTimeURI, 'advancedSys') == 'true';
    if (paramAdvancedSys || needAdvancedSys) {
        $('#lgc-enableAdvancedSys').click()
    }

    var paramEquations = getUrlParam(loadTimeURI, 'equations') == 'true';
    if (paramEquations) {
        $('#lgc-enableEquations').click();
    }

    var paramDialect = getUrlParam(loadTimeURI, 'dialect');
    if (paramDialect == 'streamline') {
        $('#lgc-format-sl').click();
    }
    else if (paramDialect == 'hardware') {
        $('#lgc-format-hw').click();
    }

    if ((!paramAdvancedApp && needAdvancedApp) ||
        (!paramAdvancedSys && needAdvancedSys))
    {
        var elem = $(paramBookmark);
        var container = $('html');

        container.scrollTop(
            elem.offset().top - container.offset().top + container.scrollTop()
        );

        var advancedAlertElem = $('#liveToast');
        var advancedToast = new bootstrap.Toast(advancedAlertElem);
        advancedToast.show();

        $(paramBookmark)[0].scrollIntoView();
    }

    // Handle menu click behaviors
    $('summary').each(function() {
        var summary = $(this);
        var details = $(this).parent();

        // On click of a summary ...
        summary.on('click', function(ev) {
            var e = ev.originalEvent;

            // Stop propagating to parents
            e.cancelBubble = true;
            if (e.stopPropagation) {
                e.stopPropagation();
            }

            // Click on links do not count as clicks on the summary itself
            if (e.srcElement.tagName == 'A') {
                return;
            }

            // Click on summary triggers custom close animation if open
            var attr = details.attr('open');
            if (typeof attr !== 'undefined' && attr !== false) {
                e.preventDefault();
                details.addClass('closing');
            }
        });

        // When the 'close' animation is over then actually close
        details.on('animationend', function(ev) {
            var e = ev.originalEvent;

            // Stop propagating to parents
            e.cancelBubble = true;
            if (e.stopPropagation) {
                e.stopPropagation();
            }

            // Formalize the details disclosure
            if (e.animationName === 'close') {
                details.removeAttr('open');
                details.removeClass('closing');
            }
        });
    });
});
