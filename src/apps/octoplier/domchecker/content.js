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
    //perform_position_check(event, data)
    //perform_image_check(event, data)
}

function perform_position_check(event, data) {
    console.log("target xpath: " + data.target)
    console.log("pageX,pageY: " + event.pageX + "," + event.pageY)
    var element = document.elementFromPoint(event.pageX-window.scrollX, event.pageY-window.scrollY)
    var xpath
    if (element) {
        xpath = get_xpath(element)
    }
    console.log("xpath: " + xpath)
    var results = find_elements_by_xpath(xpath)
    console.log("num found: " + results.length)
    dump_array_of_elements(results)
    console.log("pass: " + (xpath == data.target) && (results.length == 1))
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
    
    var text_path = get_text_path(element)
    var text = get_text_value(element)
    var xpath = form_text_match_xpath(text_path[text_path.length-1])
    
    console.log("text path: " + JSON.stringify(text_path))
    console.log("xpath: " + xpath)
    
    var elements =  find_elements_by_xpath(xpath)
    console.log('-----------------------------------------------------------')
    console.log("num elements found by xpath: " + elements.length)
    console.log('-----------------------------------------------------------')
    for (var i=0; i<elements.length; i++) {
        console.log("--xpath match[" + i + "] " + get_xpath(elements[i]))
        console.log("--text path[" + i + "] " + get_text_path(elements[i]))
    }
    
    elements = find_elements_by_text_path(text_path)
    console.log('-----------------------------------------------------------')
    console.log("num elements found by text path: " + elements.length)
    console.log('-----------------------------------------------------------')
    for (var i=0; i<elements.length; i++) {
        console.log("--match[" + i + "] " + get_xpath(elements[i]))
    }
    console.log('-----------------------------------------------------------')
    console.log("check passes: " + ((elements.length == 1) && (elements[0] == element)))
    console.log('-----------------------------------------------------------')
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

// Recurses to find text embedded in an element which is acting like a label.
function get_top_left_child(element) {
    var element_xpath = get_xpath(element)
    
    // Build xpath to find all elements with text nodes under the element xpath.
    var children_xpath = element_xpath;
    if (children_xpath != '/') {
        children_xpath += '/'
    }
    children_xpath += "/*[count(text()) > 0]";
    
    // Find all the elements.
    var children = find_elements_by_xpath(children_xpath)
    
    // The elements are in document order.
    // We want to use the earliest element we can find meeting our criteria.
    var top_left_child = null
    for (var i=0; i<children.length; i++) {
        var child = children[i]
        
        // Skip unwanted elements.
        var tagName = child.tagName
        if (tagName) {
            tagName = tagName.toLowerCase()
            if ((tagName == 'script') || (tagName == 'noscript') || (tagName == 'style')) {
                continue
            }
        }
        // Skip comment nodes.
        if (child.nodeType == Node.COMMENT_NODE) {
            continue
        }
        // Skip elements with empty text.
        if (get_text_value(child) == '') {
            continue
        }
        // Skip elements which are not visible.
        if ((child.offsetWidth == 0) || (child.offsetHeight == 0)) {
            continue
        }
        // We take the first child we find, because the elements are in
        // document order. Usually document order implies visual order as well.
        top_left_child = child
        break
    }
    return top_left_child
}

// Returns an array of text which will uniquely identify an element which has a textnode.
function get_text_path(element) {
    // Our text path.
    var text_path = []
    // Our last top left child.
    var last_top_left_child = null
    // Start visiting ancestors from the element.
    while(element) {
        // Get the top left text inside the element.
        var top_left_child = get_top_left_child(element)
        if (top_left_child && top_left_child != last_top_left_child) {
            var text = get_text_value(top_left_child)
            if (text != '') {
                text_path.unshift(text);
            }
            last_top_left_child = top_left_child
        }
        element = element.parentNode
    }
    return text_path
}

//Get the full xpath for an element, anchored at the html document.
//For simplicity each path element always has a [] index suffix.
function get_xpath(element) {
    var path = [];
    do {
        if (element.parentNode) {
            var node_name = element.nodeName;
            var sibling = element
            var count = 0;
            while (sibling) {
                if (sibling.nodeName == node_name) {
                    ++count;
                }
                sibling = sibling.previousElementSibling
            }
            path.unshift(node_name + '[' + count + ']');
        }
    } while ((element = element.parentNode));
    return '/'+path.join('/');
}

//Get position path.
function get_position_path() {
    
}

//------------------------------------------------------------------------------------------------
//Utilities.
//------------------------------------------------------------------------------------------------

//Returns an array of elements matching the text path.
function find_elements_by_text_path(text_path) {
    var xpath = form_text_match_xpath(text_path[text_path.length-1])
    var xpath_results = find_elements_by_xpath(xpath)
    var elements = []
    for (var i=0; i<xpath_results.length; i++) {
        var element = xpath_results[i]
        var path = get_text_path(element)
        if (arrays_are_equal(path, text_path)) {
            elements.push(element)
        }
    }
    return elements
}

//Find elements by xpath. 
//This returns an array of elements.
function find_elements_by_xpath(xpath) {
    var set = document.evaluate(xpath, document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null)
    var elements = []
    for (var i=0; i<set.snapshotLength; i++) {
        elements.push(set.snapshotItem(i))
    }
    return elements
}

//Dump the xpaths of an array of elements.
function dump_array_of_elements(elements) {
  for (var i=0; i<elements.length; i++) {
      console.log("found: " + get_xpath(elements[i]))
  }
}

//Returns true if the contents of two arrays of primitives are equal.
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

//Dump the client rect to the console.
function dump_client_rect(rect) {
    console.log(rect.left + "," + rect.right + "," + rect.top + "," + rect.bottom)
}

//Returns true if the outer element contains the inner element.
function element_contains(outer, inner) {
    //Associate null with zero area element
    if (outer == null) {
        return false
    }
    if (inner == null) {
        return true
    }
    //Outer and inner must be elements.
    if (!outer.getBoundingClientRect) {
        return false
    }
    if (!inner.getBoundingClientRect) {
        return false
    }
    //Call out to our helper.
    var outer_rect = outer.getBoundingClientRect()
    var inner_rect = inner.getBoundingClientRect()
    return contains(outer_rect, inner_rect)
}

// Returns true if the outer rect contains the inner rect.
function contains(outer, inner) {
    if (inner.left >= outer.left && 
            inner.right <= outer.right && 
            inner.bottom <= outer.bottom && 
            inner.top >= outer.top) {
        return true
    }
    return false
}
		


