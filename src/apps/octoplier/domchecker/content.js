//------------------------------------------------------------------------------------------------
//Our Context Menu.
//------------------------------------------------------------------------------------------------
var context_menu_context = {
        type : null,
        data : null
}

var context_menu = {
        initial_elements_group : {
            is_header : true,
            label : "<center>Find Elements</center>",
            title : "These options allow you to find a set of elements based on a given property."
        },
        find_by_data : {
            onSelect : function(event) {console.log('ddd')}
        },
        find_by_type : {
            onSelect : function(event) {console.log('ddd')},
        },
        find_by_position : {
            onSelect : function(event) {console.log('ddd')},
        },
        find_similar_vertical : {
            onSelect : function(event) {
                var elements = get_similar_vertical(event)
                for (var i=0; i<elements.length; i++) {
                    console.log('similar element['+i+'] ' + get_xpath(elements[i]))
                }
            },
        },
        find_similar_horizontal: {
            onSelect : function(event) {console.log('ddd')},
        },
        whittle_elements_group : {
            is_header : true,
            label : "<center>Whittle down Elements</center>",
            title : "These options allow you to whittle down the set of elements by adding extra contraints."
        },
        find_closest : {
            label: 'Find closest element to this',
            onSelect: function(event) {console.log('eee')},
        },
        find_closest_below : {
            label: 'Find closest element below this',
            onSelect: function(event) {console.log('eee')},
        },
        find_closest_above : {
            label: 'Find closest element above this',
            onSelect: function(event) {console.log('eee')},
        },
        find_closest_left : {
            label: 'Find closest element on the left of this',
            onSelect: function(event) {console.log('eee')},
        },
        find_closest_right : {
            label: 'Find closest element on the right of this',
            onSelect: function(event) {console.log('eee')},
        },
        element_interaction_group : {
            is_header : true,
            label : "<center>Interact with Element</center>",
            title : "These options allow you to interact with the currently focused element."
        },
        click : {
            label: 'Click',
            onSelect: function(event) {console.log('aaa')},
        },
        type : {
            label: 'Type',
            onSelect: function(event) {
                console.log('bbb'); 
                window.prompt("sometext","defaultText");
            },
        },
        submit : {
            label: 'Type enter/submit',
            onSelect: function(event) {console.log('ccc')},
        },
        extract_text: {
            label: 'Extract text',
            onSelect: function(event) {console.log('ccc')},
        },
        test_group : {
            is_header : true,
            label : "<center>Other</center>",
            title : "These are other options."
        },
        close_context_menu: {
            label: 'Close this context menu',
            onSelect: function(event) {},
        },
};

function update_context_menu(event, context_menu) {
    var element = event.target
    // Get the text value.
    var element_data = {
            text: get_text_value(element),
            media: get_image_values(element),
            get_as_string: function() {
                if (this.text) {
                    return this.text
                } else if (this.media) {
                    return JSON.stringify(this.media)
                }
                return ""
            }
    }
    // Get the type.
    var type = element.tagName.toLowerCase()
    // Get the rect in view space.
    var rect = element.getBoundingClientRect()
    // Get the page in page space
    var position = [rect.left + window.scrollX, rect.top+window.scrollY]
    
    context_menu.find_by_data.label = "<B>By data</B>: <em>" + element_data.get_as_string() + "</em>"
    context_menu.find_by_type.label = "<B>By type</B>: <em>" + type + "</em>"
    context_menu.find_by_position.label = "<B>By position</B>: <em>" + position + "</em>"
    context_menu.find_similar_vertical.label = "<B>By similarity vertically</B>: <em>" + position + "</em>"
    context_menu.find_similar_horizontal.label = "<B>By similarity horizontally</B>: <em>" + position + "</em>"
        
//        context_menu.find_by_data.enabled = true
//        context_menu.find_similar_vertical.enabled = true
//        context_menu.find_similar_horizontal.enabled = true
//        context_menu.find_by_data.enabled = false
//        context_menu.find_similar_vertical.enabled = false
//        context_menu.find_similar_horizontal.enabled = false
}

document.addEventListener ('DOMContentLoaded', on_loaded, false);
function on_loaded () {
    console.log("document is loaded!")
    window.document.addEventListener("contextmenu", function(event) {
        update_context_menu(event, context_menu)
        ContextMenu.display(event, context_menu, { horizontalOffset : 5 } );
        return false
    }, true );
}

//------------------------------------------------------------------------------------------------
//Event Blocking/Unblocking.
//------------------------------------------------------------------------------------------------

// Basically everything except the contextmenu event (right click).
var EVENT_TYPES = 'message scroll submit click dblclick mousedown mousemove mouseup mouseover mouseenter mouseout keydown keypress keyup input'.split(' ');

function block_event(event) {
    if (event.target && event.target.tagName) {
        if (window.smash_browse_context_menu && window.smash_browse_context_menu.contains(event.target)) {
            return true
        }
    }
    event.stopPropagation();
    event.preventDefault();
    event.stopImmediatePropagation();
    return false;
}

function block_events() {
    EVENT_TYPES.forEach(function (eventType) {
        window.addEventListener(eventType, block_event, true);
    });
}

function unblock_events() {
    EVENT_TYPES.forEach(function (eventType) {
        window.removeEventListener(eventType, block_event, true);
    });
}

block_events()

//------------------------------------------------------------------------------------------------
//The current set of elements.
//------------------------------------------------------------------------------------------------

var current_elements = document.getElementsByTagName("*");

//------------------------------------------------------------------------------------------------
//Our Context Menu.
//------------------------------------------------------------------------------------------------

//click dblclick mousedown mousemove mouseup mouseover keydown keypress keyup input
//var event_types = 'contextmenu'.split(' ');
//event_types.forEach(function (eventType) {
//    console.log('listening to: ' + eventType)
//    document.addEventListener(eventType, check_event, true);
//});

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
    if (image_path.length) {
        var elements = find_elements_by_image_path(image_path)
        var xpath2 = get_xpath(elements[0])
        console.log("num paths found: " + elements.length)
        console.log("passed: " + (xpath == xpath2) && (elements.length == 1))
    } else {
        console.log("passed: false")
    }
    
}

function perform_text_path_check(event, data) {
    var element = event.target
    var results = []
    
    console.log('*********************************************************')
    console.log('Element Info:')
    console.log('*********************************************************')
    
    console.log("full xpath: " + data.target)
    
    var text_path = get_text_structure(element).text_path
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
        console.log("--text path[" + i + "] " + get_text_structure(elements[i]).text_path)
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

// Returns an array of image urls which are all overlapping the given element.
function get_image_values(element) {
    var original_element = element
    
    // Find the first visible element in the parent chain.
    var internal_element = null
    var internal_rect = null
    while (element) {
        // If visible.
        if ((element.offsetWidth != 0) || (element.offsetHeight != 0)) {
            internal_element = element
            internal_rect = element.getBoundingClientRect()
            break
        }
        element = element.parentNode
    }
    
    // Find images which overlap the internal rect.
    var images = []
    
    // Build xpath to find all elements.
    var xpath = "//*"
    var elements = find_elements_by_xpath(xpath)
    console.log("determining image values from num elememnts: " + elements.length)
    
    // Loop over the elements backwards as we want the deepest element of a chain.
    for (var i=0; i<elements.length; i++) {
        var element = elements[i]
        
        // Make sure the element is visible.
        if ((element.offsetWidth == 0) || (element.offsetHeight == 0)) {
            continue
        }
        
        // Make sure the element contains the internal element.
        if (!element_contains(element, internal_element)) {
            continue
        }
        
        // Get the tag name.
        var tag_name = element.tagName.toLowerCase()
        
        // Return the src for images and video elements.
        if ((tag_name === 'img') || (tag_name === 'video')) {
            if (images.indexOf(element.src)<0) {
                //console.log("element source: " + element.src)
                images.push(element.src)
            }
        }
        
        // Otherwise we query the computed style for the background image.
        var style = element.currentStyle || window.getComputedStyle(element, false)
        if (style) {
            // console.log("ssssstyle: " + JSON.stringify(style))
            if (style.backgroundImage.indexOf('url(') == 0) {
                var background_image = style.backgroundImage.slice(4, -1);
                if (background_image != "") {
                    if (images.indexOf(background_image)<0) {
                        //console.log("background image: " + background_image)
                        images.push(background_image)
                    }
                }
            }
        }
    }
    return images
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
function get_text_structure(element) {
    // Our text path.
    var text_path = []
    var text_path_elements =[]
    // Our last top left child.
    var last_top_left_child = null
    // Start visiting ancestors from the element.
    while(element) {
        // Get the top left text inside the element.
        var top_left_child = get_top_left_text_child(element)
        if (top_left_child && top_left_child != last_top_left_child) {
            var text = get_text_value(top_left_child)
            if (text != '') {
                text_path.unshift(text)
                text_path_elements.unshift(element)
            }
            last_top_left_child = top_left_child
        }
        element = element.parentNode
    }
    
    return {
        text_path: text_path,
        text_path_elements: text_path_elements
    }
}

function find_odd_spacing(spacing) {
    var last_diff = spacing[1];
    for (var i=1; i<spacing.length; i++) {
        if (spacing[i]-spacing[i-1] != last_diff) {
            return i
        }
    }
    return spacing.length
}

function get_similar_vertical(start_element) {
    
    if (!start_element) {
        console.log("no start element")
    }
    
    var page_height = get_page_height()
    
    // Get properties of the element.
    var target_rect = start_element.getBoundingClientRect()
    var target_text = get_text_value(start_element)
    var target_urls = get_image_values(start_element)
    
    // Match settings.
    var match_left = true;
    var match_width = false;
    var match_height = true; // text, images, and video must be of the same height.
    var match_inter_spacing = true
    
    // If we have images or video then match the width also.
    if (target_urls.length) {
        match_width = true;
    }
    
    // Scan upwards.
    var up_elements = [start_element]
    var up_spacing = [0]
    var h = target_rect.top - 1
    while (h>=0) {
        var element = document.elementFromPoint(target_rect.left, h)
        h -= 50
        if (!element) {
            continue
        }
        
        // If we have this element already then continue.
        if (up_elements.indexOf(element) >= 0) {
            continue
        }
        
        // Get properties.
        var rect = element.getBoundingClientRect()
        var text = get_text_value(element)
        //var urls = get_image_values(element)
        
        // Test match.
        if (match_left && (rect.left != target_rect.left)) {
            continue
        }
        if (match_width && (rect.width != target_rect.width)) {
            continue
        }
        if (match_height && (rect.height != target_rect.height)) {
            continue
        }
        up_elements.push(element)
        up_spacing.push(rect.bottom-target_rect.bottom)
    }
    
    // Scan downwards.
    var down_elements = [start_element]
    var down_spacing = [0]
    h = target_rect.bottom + 1
    while (h < page_height) {
        
        console.log('h: ' + h + ", " + page_height)
        
        var element = document.elementFromPoint(target_rect.left, h)
        h += 50
        if (!element) {
            continue
        }
        
        // If we have this element already then continue.
        if (down_elements.indexOf(element) >= 0) {
            continue
        }
        
        // Get properties.
        var rect = element.getBoundingClientRect()
        var text = get_text_value(element)
        var urls = get_image_values(element)
        
        // Test match.
        if (match_left && (rect.left != target_rect.left)) {
            continue
        }
        if (match_width && (rect.width != target_rect.width)) {
            continue
        }
        if (match_height && (rect.height != target_rect.height)) {
            continue
        }
        down_elements.push(element)
        down_spacing.push(rect.bottom-target_rect.bottom)
    }
    console.log('up down sizes: ' + up_spacing.length + ',' + down_spacing.length)
    console.log('up_spacing: ' + up_spacing)
    console.log('down_spacing: ' + down_spacing)
    // Now check the inter spacing.
    if (up_spacing.length < 2) {
        if (down_spacing.length < 2) {
            return [element]
        } else {
            var index = find_odd_spacing(down_spacing)
            return down_spacing.slice(0,index)
        }
    } else {
        if (down_spacing.length < 2) {
            var index = find_odd_spacing(up_spacing)
            return up_spacing.slice(0,index)
        } else {
            var up_index = find_odd_spacing(up_spacing)
            var down_index = find_odd_spacing(down_spacing)
            return up_spacing.splice(0,up_index).concat(down_spacing.splice(1,down_index))
        }
    }
    return [start_element]
}

function get_similar_horizontal(page_pos) {
    
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
                console.log("xpath["+i+"] " + get_text_structure(filtered[i]).text_path)
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
    var image_path = get_text_structure(element).text_path
    var urls = get_image_values(element)
    if (urls.length == 0) {
        return []
    }
    image_path.push(urls)
    return image_path
}

//------------------------------------------------------------------------------------------------
//Utilities.
//------------------------------------------------------------------------------------------------

//Returns an array of elements which have text nodes.
//The elements under consideration are the element argument and its children.
function find_text_elements(element) {
    var element_xpath = get_xpath(element)
    
    // Build xpath to find all elements with text nodes under the element xpath.
    var xpath = element_xpath;
    if (xpath != '/') {
        xpath += '/'
    }
    xpath += "/*[count(text()) > 0]";
    return find_elements_by_xpath(xpath)
}

// Returns an array of elements which have background images or is an image or video itself.
function find_image_elements(text_path, grouping_rect, urls) {
    var elements = []
    var xpath = "//*"
    
    // Look for elements containing the image url.
    // We look backwards so that we get the deepest and latest elements first in document order.
    var children = find_elements_by_xpath(xpath)
    console.log("num children to look at: " + children.length)
    for (var i=0; i<children.length; i++) {
        var child = children[i]
        
        // If the child isn't visible skip it.
        if ((child.offsetWidth == 0) || (child.offsetHeight == 0)) {
            continue
        }
        
        // If the child doesn't intersect the grouping_rect skip it.
        if (!intersects(child.getBoundingClientRect(), grouping_rect)) {
            continue
        }
        
        // Make sure we're getting top most element, when elements are overlapping. (like divs)
        child = get_top_element(child)
        if (!child) {
            continue
        }
        
        // Get the text path of the child.
        var path = get_text_structure(child).text_path
        if (!arrays_are_equal(text_path, path)) {
            continue
        }
        
        // Otherwise try getting the image value.
        var image_urls = get_image_values(child)
        
        // If it matches our url we've found one.
        if (arrays_are_equal(image_urls,urls) ) {
            if (elements.indexOf(child) <0) {
                elements.push(child)
            }
        }
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
        var path = get_text_structure(element).text_path
        if (arrays_are_equal(path, text_path)) {
            elements.push(element)
        }
    }
    return elements
}

function find_elements_by_image_path(image_path) {
    var text_path = image_path
    var image_urls = text_path.pop()
    
    var image_elements = []
    var text_elements = find_elements_by_text_path(text_path)
    
    console.log("num text elements: " + text_elements.length)
    
    
    for (var e=0; e<text_elements.length; e++) {
        var text_element = text_elements[e]
        var obj = get_text_structure(text_element)
        var grouping_element = obj.text_path_elements[obj.text_path_elements.length-2]
        image_elements = image_elements.concat(find_image_elements(text_path, grouping_element.getBoundingClientRect(), image_urls))
    }
    
    for (var t=0; t<image_elements.length; t++) {
        console.log("ppp: " + get_xpath(image_elements[t]))
        console.log("qqq: " + get_image_path(image_elements[t]))
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

function get_top_element(element) {
    // Make sure the element is visible.
    if ((element.offsetWidth == 0) || (element.offsetHeight == 0)) {
        console.log('t')
        return null
    }
    
    var rect = element.getBoundingClientRect()
    var x = rect.left + 1
    var y = rect.top + 1
    return document.elementFromPoint(x, y)
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

function get_page_height() {
    var doc_body = document.body
    var doc_elem = document.documentElement
    return Math.max( doc_body.scrollHeight, doc_body.offsetHeight, 
            doc_elem.clientHeight, doc_elem.scrollHeight, doc_elem.offsetHeight );
}

function get_page_width() {
    var doc_body = document.body
    var doc_elem = document.documentElement
    return Math.max( doc_body.scrollWidth, doc_body.offsetWidth, 
            doc_elem.clientWidth, doc_elem.scrollWidth, doc_elem.offsetWidth );
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

function element_intersects(outer, inner) {
    //Associate null with zero area element
    if (outer == null) {
        return false
    }
    if (inner == null) {
        return false
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
    return intersects(outer_rect, inner_rect)
}
		
function intersects(rect_a, rect_b) {
    if (rect_a.right < rect_b.left) {
        return false
    } else if (rect_a.left > rect_b.right) {
        return false
    } else if (rect_a.bottom < rect_b.top) {
        return false
    } else if (rect_a.top > rect_b.bottom) {
        return false
    }
    return true
}



