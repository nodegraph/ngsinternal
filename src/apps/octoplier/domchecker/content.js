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
    //perform_text_path_check(event, data)
    //perform_position_check(event, data)
    perform_image_check(event, data)
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
    //console.log("target xpath: " + data.target)
    //console.log("pageX,pageY: " + event.pageX + "," + event.pageY)
    var image_path = get_image_path(event.target)
    var xpath = get_xpath(event.target)
    console.log("xpath: " + xpath)
    console.log("image_path: " + image_path)
    var elements = find_elements_by_image_path(image_path)
    var xpath2 = get_xpath(elements[0])
    console.log("passed: " + (xpath == xpath2) && (elements.length == 1))
    
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
    get_similar_text_paths(element, false)
    
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



// Gets the text value from an element's immediate textnode child.
function get_text_value(element) {
    // Get the element's children.
    var children = element.childNodes
    // Loop through them looking for a text node.
    for (var i=0; i<children.length; i++) {
        var child = children[i]
        if (child.nodeType == Node.TEXT_NODE) {
            var text = child.nodeValue
            // If the text is not all white space, then we have found our text.
            if (!is_all_whitespace(text)) {
                return text
            }
        }
    }
    // Otherwise we didn't find any text for this element.
    return ""
}

function get_image_value(element) {
    // Make sure we have an element.
    if (!element.tagName) {
        return ""
    }
    
    // If we have an image element, then return the src attribute.
    if (element.tagName.toLowerCase() === 'img') {
        return element.src
    }
    
    // Otherwise we query the computed style for the background image.
    var style = element.currentStyle || window.getComputedStyle(element, false)
    if (style) {
        var background_image = style.backgroundImage.slice(4, -1);
        if (background_image != "") {
            return background_image
        }
    }
    
    // Otherwise we recurse on our children.
    var children = element.childNodes
    for (var i=0; i<children.length; i++) {
        var child = children[i]
        var url = get_image_value(child)
        if (url != "") {
            return url
        }
    }
    
    // Otherwise we have failed to find an image value.
    return ""
}

function is_text_child(child) {
    // Skip unwanted elements.
    var tagName = child.tagName
    if (tagName) {
        tagName = tagName.toLowerCase()
        if ((tagName == 'script') || (tagName == 'noscript') || (tagName == 'style')) {
            return false
        }
    }
    // Skip comment nodes.
    if (child.nodeType == Node.COMMENT_NODE) {
        return false
    }
    // Skip elements with empty text.
    if (get_text_value(child) == '') {
        return false
    }
    // Skip elements which are not visible.
    if ((child.offsetWidth == 0) || (child.offsetHeight == 0)) {
        return false
    }
    return true
}

// Recurses to find text embedded in an element which is acting like a label.
function get_top_left_text_child(element) {
    var children = find_text_elements(element)
    // The elements are in document order. Usually this implies visual order as well.
    // We take the first element we can find meeting our criteria.
    // The top left child could be the element argument itself.
    for (var i=0; i<children.length; i++) {
        var child = children[i]
        if (is_text_child(child)) {
            return  child
            break
        }
    }
    return null
}

function get_bottom_left_text_child(element) {
    var children = find_text_elements(element)
    // The elements are in document order. Usually this implies visual order as well.
    // We take the last element we can find meeting our criteria.
    // The bottom left child could be the element argument itself.
    for (var i=children.length-1; i>=0; i--) {
        var child = children[i]
        if (is_text_child(child)) {
            return child
            break
        }
    }
    return null
}

function get_text_siblings(element) {
    var children = find_text_elements(element)
    var siblings = []
    // The elements are in document order. Usually this implies visual order as well.
    // We take the last element we can find meeting our criteria.
    // The bottom left child could be the element argument itself.
    for (var i=children.length-1; i>=0; i--) {
        var child = children[i]
        if (is_text_child(child)) {
            siblings.push(child)
        }
    }
    return siblings
}

//Returns an array of text which will uniquely identify an element which has a textnode.
function get_text_path(element) {
    // Our text path.
    var text_path = []
    // Our last top left child.
    var last_top_left_child = null
    // Start visiting ancestors from the element.
    while(element) {
        // Get the top left text inside the element.
        var top_left_child = get_top_left_text_child(element)
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

//Get text paths similar to and including the element.
//This is intended to retrieve paths for elements in the same visual group as the element.
function get_similar_text_paths(element, horizontal) {
    var original_element = element
    var element_rect = element.getBoundingClientRect()

    // Find first ancestor with siblings that include the element argument.
    while(element) {
        var siblings = get_text_siblings(element)
        var filtered = []
        if (siblings.length>1) {
            for (var i=0; i<siblings.length; i++) {
                var sibling = siblings[i]
                var rect = sibling.getBoundingClientRect()
                dump_client_rect(rect)
                if (horizontal) {
                    if (rect.bottom == element_rect.bottom) {
                        filtered.push(sibling)
                    }
                } else {
                    if (rect.left == element_rect.left) {
                        filtered.push(sibling)
                    }
                }
            }
        }
        // If we found more than one then return it.
        if (filtered.length > 1) {
            console.log("num filtered: " + filtered.length)
            for (var i=0; i<filtered.length; i++) {
                console.log("filtered["+i+"] "+get_xpath(filtered[i]))
                console.log("xpath["+i+"] " + get_text_path(filtered[i]))
            }
            return filtered
        } else {
            filtered.length = 0
        }
        
        // Otherwise check our next parent.
        element = element.parentNode
    }
    
    console.log("num filtered: " + 1)
    console.log("filtered[0] original element")
    console.log("xpath[0] original element")

    return [original_element]
}

//Get the full xpath for an element, anchored at the html document.
//For simplicity each path element always has a [] index suffix.
function get_xpath(element) {
    var path = [];
    while(element) {
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
        element = element.parentNode
    }
    return '/'+path.join('/');
}

// Get image path.
function get_image_path(element) {
    var image_path = get_text_path(element)
    var url = ""
        
    while(element) {
        console.log("checking path: " + get_xpath(element))
        url = get_image_value(element)
        if (url != "") {
            console.log("found!!")
            break;
        }
        element = element.parentNode
    }
    
    if ((url != "") && (get_text_path(element).length>=image_path.length)) {
        image_path.push(url)
        return image_path
    }
    return []
}

//------------------------------------------------------------------------------------------------
//Utilities.
//------------------------------------------------------------------------------------------------

//Returns an array of elements which have text nodes.
//The elements under consideration are the element argument and its children.
function find_text_elements(element) {
    var element_xpath = get_xpath(element)
    
    // Build xpath to find all elements with text nodes under the element xpath.
    var children_xpath = element_xpath;
    if (children_xpath != '/') {
        children_xpath += '/'
    }
    children_xpath += "/*[count(text()) > 0]";
    return find_elements_by_xpath(children_xpath)
}

function find_image_elements(element, url) {
    var elements = []
    var element_xpath = get_xpath(element)
    
    // Build xpath to find all elements with text nodes under the element xpath.
    var children_xpath = '/' //element_xpath;
    if (children_xpath != '/') {
        children_xpath += '/'
    }
    children_xpath += "/*[self::img or self::div]";
    console.log("children xpath: " + children_xpath)
    
    // Look for elements containing the image url.
    // We look backwards so that we get the deepest and latest elements first in document order.
    var children = find_elements_by_xpath(children_xpath)
    console.log("num children to look at: " + children.length)
    var last_child = null
    for (var i=children.length; i>=0; i--) {
        var child = children[i]
        if (!child) {
            continue
        }
        // If the last_child is contained in this child, we skip it.
        if (child.contains(last_child)) {
            continue
        }
        // Otherwise try getting the image value.
        var image_url = get_image_value(child)
        // If it matches our url we've found one.
        if (image_url == url) {
            console.log("found element with matching url")
            elements.push(child)
        }
        last_child = child
    }
    return elements
}

//Returns an array of elements matching the text path.
function find_elements_by_text_path(text_path) {
    var text_path_tail = text_path[text_path.length-1]
    var xpath = form_text_match_xpath(text_path_tail)
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

function find_elements_by_image_path(image_path) {
    var text_path = image_path
    var image_url = text_path.pop()
    console.log("find text_path: " + text_path)
    console.log("find image_url: " + image_url)
    var image_elements = []
    var text_elements = find_elements_by_text_path(text_path)
    console.log("num text elements: " + text_elements.length)
    for (var e=0; e<text_elements.length; e++) {
        var element = text_elements[e]
        var temp = find_image_elements(element, image_url)
        for (var t=0; t<temp.length; t++) {
            console.log("ppp: "+ get_xpath(temp[t]))
        }
        console.log("num temp elements: " + temp.length)
        image_elements = image_elements.concat(temp)
        console.log("num image elements: " + image_elements.length)
    }
    return image_elements
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

//Returns true if the text is composed of all whitespace characters.
function is_all_whitespace(text) {
    var regex = /^\s+$/;
    return regex.test(text)
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
		


