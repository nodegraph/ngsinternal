console.log('hahaha')

//Catch any uncaught exceptions at the top level window.
window.onerror = function (message, url, line_number, column_number, error) {
    console.log('Error: caught at Window: ' + message + 
            ' Script: ' + url + 
            ' Line: ' + line_number + 
            ' Column: ' + column_number + 
            ' Error: ' + error);
}

document.addEventListener ('DOMContentLoaded', on_loaded, false);
function on_loaded () {
    console.log("document is loaded!")
}
//click dblclick mousedown mousemove mouseup mouseover keydown keypress keyup input
var event_types = 'contextmenu'.split(' ');
event_types.forEach(function (eventType) {
    console.log('listening to: ' + eventType)
    document.addEventListener(eventType, check_event, true);
});

function check_event(event) {
    var target = event.target
    var rect = target.getBoundingClientRect ? target.getBoundingClientRect() : {};
    var data = {
            altKey: event.altKey,
            button: event.button,
            buttons: event.buttons,
            ctrlKey: event.ctrlKey,
            clientX: event.clientX,
            clientY: event.clientY,
            elementX: event.clientX - rect.left,
            elementY: event.clientY - rect.top,
            keyIdentifier: event.keyIdentifier,
            key: event.key,
            code: event.code,
            charCode: event.charCode,
            keyCode: event.keyCode,
            location: event.location,
            metaKey: event.metaKey,
            shiftKey: event.shiftKey,
            target: get_xpath(target),
            targetFrame: [],
            type: event.type,
    } 
    perform_text_path_check(event, data)
    perform_position_check(event, data)
    perform_image_check(event, data)
}

function perform_position_check(event, data) {
    
}

function perform_image_check(event, data) {
    
}

function perform_text_path_check(event, data) {
    var element = event.target
    var results = []
    
    console.log('*********************************************************')
    console.log('Element Info:')
    console.log('*********************************************************')
    
    console.log("full xpath: " + data.target)
    
    var deep_text = get_deep_text([element], null)
    var text_path = get_text_path(element)
    var text = get_text_value(element)
    var xpath = form_text_match_xpath(text_path[text_path.length-1])
    
    console.log("text path: " + JSON.stringify(text_path))
    console.log("xpath: " + xpath)
    
    var results =  find_elements_by_xpath(xpath)
    console.log('-----------------------------------------------------------')
    console.log("num elements found by xpath: " + results.snapshotLength)
    console.log('-----------------------------------------------------------')
    for (var i=0; i<results.snapshotLength; i++) {
        console.log("--xpath match[" + i + "] " + get_xpath(results.snapshotItem(i)))
        console.log("--text path[" + i + "] " + get_text_path(results.snapshotItem(i)))
    }
    
    results = find_elements_by_text_path(text_path)
    console.log('-----------------------------------------------------------')
    console.log("num elements found by text path: " + results.length)
    console.log('-----------------------------------------------------------')
    for (var i=0; i<results.length; i++) {
        console.log("--match[" + i + "] " + get_xpath(results[i]))
    }
    console.log('-----------------------------------------------------------')
    console.log("check passes: " + ((results.length == 1) && (results[0] == element)))
    console.log('-----------------------------------------------------------')
}

//Find elements by xpath. 
//This returns a snapshot of elements, which is something different from an array.
function find_elements_by_xpath(xpath) {
    return document.evaluate(xpath, document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null)
}

//Dump found elements to the console.
function dump_found_elements(results) {
    for (var i=0; i<results.snapshotLength; i++) {
        console.log("found: " + get_xpath(results.snapshotItem(i)))
    }
}

//Form an xpath expression which will match any node with a 
//textnode inside of them whose text matches that given.
function form_text_match_xpath(text) {
    // We need to break out single quotes and concat them
    // to form a proper xpath.
    var splits = text.split("'")
    if (splits.length>1) {
        // Concat the splits with single quotes.
        text = "concat(";
        for (var s=0; s<splits.length; s++) {
            if (s!=0){
                text += "," + "\"'\"" + ","  // single quote is added here
            }
            text += "'" + splits[s] + "'"
        }
        text += ")"
    } else {
        // The text has no single quotes here.
        text = "'" + text + "'"
    }
    return "//*[text()[string(.) = "+ text +"]]"
}

// Returns true if the text is composed of all whitespace characters.
function is_all_whitespace(text) {
    var regex = /^\s+$/;
    return regex.test(text)
}

// Gets the text value from an element's immediate textnode child.
function get_text_value(element) {
    var tagName = element.tagName
    if (tagName) {
        tagName = tagName.toLowerCase()
        if ((tagName == 'script') || (tagName == 'noscript') || (tagName == 'style')) {
            return ""
        }
    }
    
    if (element.nodeType == Node.COMMENT_NODE) {
        return ""
    }
    
    var children = element.childNodes
    for (var i=0; i<children.length; i++) {
        var child = children[i]
        if (child.nodeType == Node.TEXT_NODE) {
            var text = child.nodeValue
            if (!is_all_whitespace(text)) {
                //console.log('element nodetype: ' + element.nodeType + ' tagname: ' + element.tagName + ' text: ' + text)
                return text
            }
        }
    }
    return ""
}

// Recurse to find text embedded in an element, where the 
// text's css style's class name differs from that of the containee.
// This use breadth first search.
function get_deep_text(element_queue, containee) {
    if (element_queue.length == 0) {
        return ""
    }
    // Pop an element from the queue.
    var element = element_queue.shift()
    
    // If the element has the same className as the containee
    // then we bail.
    if (containee && element.tagName) {
        if (element.className == containee.className) {
            return ""
        }
    }
    
    // Get the immediate text value.
    var text = get_text_value(element)
    if (text != "") {
        return text
    }
    
    // Otherwise find children to recurse to.
    var children = element.childNodes
    for (var i=0; i<children.length; i++) {
        var child = children[i]
        if (child == containee) {
            continue
        }
        if (child.nodeType != Node.TEXT_NODE) {
            element_queue.push(child)
        }
    }
    
    // Recurse
    return get_deep_text(element_queue, containee)
}

//Get the text path for an element.
//The text path uses class names to figure out the
//text hierarchy surrounding an element.
function get_text_path(element) {
    var text_path = []
    var containee = null
    do {
        var text = get_deep_text([element], containee)
        if (text && text != '') {
            text_path.unshift(text);
            containee = element
        }
        element = element.parentNode
    } while (element);
    return text_path
}

function arrays_are_equal(a1, a2) {
    if (a1.length != a2.length) {
        return false
    }
    for (var i=0; i<a1.length; i++) {
        if (a1[i] != a2[i]) {
            return false
        }
    }
    return true
}

function find_elements_by_text_path(text_path) {
    var xpath = form_text_match_xpath(text_path[text_path.length-1])
    var xpath_results = find_elements_by_xpath(xpath)
    var results = []
    for (var i=0; i<xpath_results.snapshotLength; i++) {
        var element = xpath_results.snapshotItem(i)
        var path = get_text_path(element)
        if (arrays_are_equal(path, text_path)) {
            results.push(element)
        }
    }
    return results
}

//Get the full xpath for an element.
function get_xpath(element) {
    var path = [];

    var ignoreId = true;

    do {
        if (element.id && !ignoreId) {
            path.unshift('id("' + element.id + '")');

            // No need to continue to ascend since we found a unique root
            break;
        }
        else if (element.parentNode) {
            var nodeName = element.nodeName;
            var hasNamedSiblings = Boolean(element.previousElementSibling || element.nextElementSibling);
            // XPath is 1-indexed
            var index = 1;
            var sibling = element;

            if (hasNamedSiblings) {
                while ((sibling = sibling.previousElementSibling)) {
                    if (sibling.nodeName == nodeName) {
                        ++index;
                    }
                }

                path.unshift(nodeName + '[' + index + ']');
            }
            else {
                path.unshift(nodeName);
            }
        }
        // The root node
        else {
            path.unshift('');
        }
    } while ((element = element.parentNode));

    var full_path =  path.join('/');
    if (full_path == "") {
        return "/"
    }
    return full_path
}

//Returns true if the outer element contains the inner element.
function element_contains(outer, inner) {
    if (inner == null) {
        return true
    }
    if (!outer.getBoundingClientRect) {
        return false
    }
    if (!inner.getBoundingClientRect) {
        return false
    }
    
    var outer_rect = outer.getBoundingClientRect()
    var inner_rect = inner.getBoundingClientRect()
    
    //console.log('outer: ' + outer_rect.left + ',' + outer_rect.right + ',' + outer_rect.bottom + ',' + outer_rect.top)
    //console.log('inner: ' + inner_rect.left + ',' + inner_rect.right + ',' + inner_rect.bottom + ',' + inner_rect.top)
    
    return contains(outer_rect, inner_rect)
}

// Returns true if the outer rect contains the inner rect.
function contains(outer, inner) {
    if (inner.left >= outer.left && 
            inner.right <= outer.right && 
            inner.bottom <= outer.bottom && 
            inner.top >= outer.top) {
        //console.log("contains is true")
        return true
    }
    //console.log('contains is false')
    return false
}
		


