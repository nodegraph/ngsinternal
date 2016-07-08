

//Returns true if the element matches the target element according
//to the given match criteria.
function elements_match(element, target_element, match_criteria) {
    var rect = element.getBoundingClientRect()
    var target_rect = target_element.getBoundingClientRect()

    // Test match.
    if (match_criteria.match_left && (rect.left != target_rect.left)) {
        return false
    }
    if (match_criteria.match_right && (rect.right != target_rect.right)) {
        return false
    }

    if (match_criteria.match_width && (rect.width != target_rect.width)) {
        return false
    }
    if (match_criteria.match_height && (rect.height != target_rect.height)) {
        return false
    }

    // Match xpath length.
    if (match_criteria.match_xpath_length) {
        xpath = get_xpath(element)
        target_xpath = get_xpath(target_element)
        if (xpath.split('/').length != target_xpath.split('/').length) {
            return false
        }
    }

    // Match font style.
    if (match_criteria.match_font || match_criteria.match_font_size) {
        var style = window.getComputedStyle(element, null)
        var target_style = window.getComputedStyle(target_element, null)

        if (match_criteria.match_font && (style.font != target_style.font)) {
            return false
        }
        if (match_criteria.match_font_size && (style.fontSize != target_style.fontSize)) {
            return false
        }
    }
    return true
}

// Returns an array starting with the target element and followed by all matching elements.
// The matching elements are found by scanning vertically from begin_y to end_y.
function get_matching_elements_on_y(target_element, match_criteria, begin_y, end_y) {
    // Results.
    var elements = [target_element]
    
    // Determine the y delta.
    var delta = -1
    if ((end_y - begin_y) >= 0) {
        delta = 1
    }
    
    // Determine the x sampling value.
    var target_rect = target_element.getBoundingClientRect()
    var target_rect_sample_x = target_rect.left + (Math.min(target_rect.width, target_rect.height) / 2.0)
    
    // Scan vertically.
    var h = begin_y
    while(((delta < 0) && (h > end_y)) || ((delta > 0) && (h < end_y))) {
        var element = document.elementFromPoint(target_rect_sample_x, h)
        h += delta
        
        // Continue if we don't have an element at this point.
        if (!element) {
            continue
        }
        
        // Continue if we already have this element.
        if (elements.indexOf(element) >= 0) {
            continue
        }
        
        // Does this element match the target element.
        if (!elements_match(element, target_element, match_criteria)){
            continue
        }
        
        // We have found a matching element.
        elements.push(element)
    }
    return elements
}

//Returns an array of elements which are similar to the target element 
//and intersect a vertical line passing through the target element.
//This array will include the target element, and will not be completely ordered.
//The array will start with the target element, then the elements above, then 
//the elements below.
function get_similar_elements_on_y(target_element) {
    var page_height = get_page_height()

    // Get properties of the element.
    var target_rect = target_element.getBoundingClientRect()
    var target_text = get_text_value(target_element)
    var target_urls = get_image_values(target_element)

    // The match properties object.
    var match_criteria = {
        match_left: true,
        match_right: false,
        match_width: false,
        match_height: false,
        match_font: false,
        match_font_size: false,
        match_xpath_length: true
    }

    // If we have images or video then match the width also.
    if (target_urls.length) {
        match_criteria.match_width = true;
        match_criteria.match_height = true;
    } else {
        match_criteria.match_font = true;
        match_criteria.match_font_size = true;
    }
    
    // Scan downwards.
    var down_elements = get_matching_elements_on_y(target_element, match_criteria, target_rect.bottom, page_height)

    // Scan upwards.
    var up_elements = get_matching_elements_on_y(target_element, match_criteria, target_rect.top, 0)

    //console.log('up down sizes: ' + up_elements.length + ',' + down_elements.length)

    // Join up and down elements.
    if (up_elements.length < 2) {
        if (down_elements.length < 2) {
            return [target_element]
        } else {
            return down_elements
        }
    } else {
        if (down_elements.length < 2) {
            return up_elements
        } else {
            down_elements.shift()
            return up_elements.concat(down_elements)
        }
    }
    // We will never reach this return statement.
    return [target_element]
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











