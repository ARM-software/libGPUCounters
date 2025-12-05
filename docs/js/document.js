/*
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

// What is the current text filter?
var currentFilter = ''

class DataModelNode {

    constructor(nodeType, elem, name) {
        this.nodeType = nodeType;
        this.elem = elem;
        this.menuElem = null;
        this.name = name;
        this.childNodes = [];
    }

    propagateFilter(filter) {
        // Determine if this node is filtered based on substring match
        var filtered = false;
        if (this.name != null && filter != '') {
            var lname = this.name.toLowerCase();
            var lfilter = filter.toLowerCase();
            filtered = lname.indexOf(lfilter) == -1;
        }

        // If this node is not filtered then always keep its children
        if (this.name != null && filtered == false) {
            for(var child of this.childNodes) {
                child.propagateFilter('');
            }
        // Else filter the children based on the filter string
        } else {
            for(var child of this.childNodes) {
                child.propagateFilter(filter);
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
    heading = $(elem).find('>:first-child');
    var series = new DataModelNode('Series', elem, $(heading).text())
    parent.addChildNode(series);
}

/*
 * Build a data model for a single counter group.
 */
function buildDataModelGroup(parent, elem) {
    // Build this group
    heading = $(elem).find('>:first-child');
    var group = new DataModelNode('Group', elem, $(heading).text())
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
    var section = new DataModelNode('Section', elem, $(heading).text())
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

    return (!isAdvancedApp || showAdvancedApp) &&
           (!isAdvancedSys || showAdvancedSys);
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
        if ($(this).is(':checked')) {
            showAdvancedApp = true;

            $('.lgc-advanced-app').show();
            if (showAdvancedSys) {
                $('.lgc-advanced-app.lgc-advanced-sys').show();
            }
        }
        else
        {
            showAdvancedApp = false;
            $('.lgc-advanced-app:not(.lgc-advanced-sys)').hide();
            if (!showAdvancedSys) {
                $('.lgc-advanced-app.lgc-advanced-sys').hide();
            }
        }
        updatePageUrl();
    });

    $('#lgc-enableAdvancedSys').change(function() {
        if ($(this).is(':checked')) {
            showAdvancedSys = true;

            $('.lgc-advanced-sys').show();
            if (showAdvancedApp) {
                $('.lgc-advanced-app.lgc-advanced-sys').show();
            }
        }
        else
        {
            showAdvancedSys = false;
            $('.lgc-advanced-sys:not(.lgc-advanced-app)').hide();
            if (!showAdvancedApp) {
                $('.lgc-advanced-app.lgc-advanced-sys').hide();
            }
        }
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

    $('#lgc-format-lgc').change(function() {
        if ($(this).is(':checked')) {
            $('.lgc-equation-format-lgc').show();
            $('.lgc-equation-format-sl').hide();
            $('.lgc-equation-format-hw').hide();

            useDialect = 'libgpucounters';
            updatePageUrl();
        }
    });

    $('#lgc-format-sl').change(function() {
        if ($(this).is(':checked')) {
            $('.lgc-equation-format-lgc').hide();
            $('.lgc-equation-format-sl').show();
            $('.lgc-equation-format-hw').hide();

            useDialect = 'streamline';
            updatePageUrl();
        }
    });

    $('#lgc-format-hw').change(function() {
        if ($(this).is(':checked')) {
            $('.lgc-equation-format-lgc').hide();
            $('.lgc-equation-format-sl').hide();
            $('.lgc-equation-format-hw').show();

            useDialect = 'hardware';
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
