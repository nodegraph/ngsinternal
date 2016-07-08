//----------------------------------------------------------------------------------------
//Arrays.
//----------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------
//String Processing.
//----------------------------------------------------------------------------------------

//Returns true if the text is composed of all whitespace characters.
function is_all_whitespace(text) {
    var regex = /^\s+$/;
    return regex.test(text)
}

//----------------------------------------------------------------------------------------
//Debugging.
//----------------------------------------------------------------------------------------

//Dump the xpaths of an array of elements.
function dump_array_of_elements(elements) {
  for (var i=0; i<elements.length; i++) {
      console.log("found: " + get_xpath(elements[i]))
  }
}

//Dump the client rect to the console.
function dump_client_rect(rect) {
    console.log("client rect: " + rect.left + "," + rect.right + "," + rect.top + "," + rect.bottom)
}

//----------------------------------------------------------------------------------------
//Element Transforms.
//----------------------------------------------------------------------------------------

function get_clickable_element(element) {
    // Calculate a click point somewhat inside the element.
    var rect = element.getBoundingClientRect()
    var offset = Math.min(rect.width, rect.height)/2.0
    var x = rect.left + offset
    var y = rect.top + offset
    // Get the element at that click point.
    return document.elementFromPoint(x, y)
}

//----------------------------------------------------------------------------------------
//Page Properties.
//----------------------------------------------------------------------------------------

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

function disable_hover() {
    var hover_regex = /:hover/;
    for (var i = 0; i < document.styleSheets.length; i++) {
        var sheet = document.styleSheets[i]
        var rules = sheet.cssRules
        // Note the cssRules will be null if the css stylesheet is loaded from another domain (cross domain security).
        // However is you use "chrome --disable-web-security --user-data-dir", then it will not be null allowing you to access it.
        if (!rules) {
            continue;
        }
        // Loop over the rules.
        for (var j = rules.length - 1; j >= 0; j--) {
            var rule = rules[j];
            if (rule.type === CSSRule.STYLE_RULE && hover_regex.test(rule.selectorText)) {
                console.log('deleting rule for ' + rule.selectorText + '+++' + rule.style)
                sheet.deleteRule(j);
            }
        }
    }
}

//function disable_hover() {
//var element = document.getElementById('smash_browse_hover_style');
//if (!element) {
//element = document.createElement('style');
//element.type = 'text/css';
//element.id = 'smash_browse_hover_style';
////var style = '*:not(smash_browse_submenu):not(smash_browse_menu_item):hover {pointer-events: none !important;}  *:not(smash_browse_submenu)::after {pointer-events: none !important}';
//var style = '*:not(smash_browse_submenu):not(smash_browse_menu_item):hover {pointer-events: none !important;}';
//element.appendChild(document.createTextNode(style));
//document.getElementsByTagName('head')[0].appendChild(element);
//}
//}
//
//function enable_hover() {
//var element = document.getElementById('smash_browse_hover_style');
//if (!element) {
//element.remove()
//}
//}

//----------------------------------------------------------------------------------------
//Element geometry tests.
//----------------------------------------------------------------------------------------

//Returns true if the outer element contains the inner element.
function element_contains(outer, inner) {
    //Get the rects.
    var outer_rect = outer.getBoundingClientRect()
    var inner_rect = inner.getBoundingClientRect()
    //Get the page boxes.
    var outer_box = get_page_box_from_client_rect(outer_rect)
    var inner_box = get_page_box_from_client_rect(inner_rect)
    //Test.
    return page_box_contains(outer_box, inner_box)
}

function element_intersects(a, b) {
    //Get the rects.
    var a_rect = a.getBoundingClientRect()
    var b_rect = b.getBoundingClientRect()
    //Get the page boxes.
    var a_box = get_page_box_from_client_rect(a_rect)
    var b_box = get_page_box_from_client_rect(b_rect)
    //Test.
    return page_box_intersects(a_box, b_box)
}

function element_contains_point(element, page_x, page_y) {
    var rect = element.getBoundingClientRect()
    var page_box = get_page_box_from_client_rect(rect)
    if ((page_x >= page_box[0]) && (page_x <= page_box[1]) && 
            (page_y >=page_box[2]) && (page_y <= page_box[3])) {
        return true
    }
    return false
}
        
//----------------------------------------------------------------------------------------
//Page box geometry tests.
//----------------------------------------------------------------------------------------

//Returns a bounding box in pages space as an array [min_x, max_x, min_y, max_y]
function get_page_box_from_client_rect(rect) {
    return [rect.left + window.scrollX,
            rect.right + window.scrollX,
            rect.top + window.scrollY,
            rect.bottom + window.scrollY]
}

//Returns true if the outer rect contains the inner rect.
function page_box_contains(outer, inner) {
    if ((inner[0] >= outer[0]) && 
            (inner[1] <= outer[1]) && 
            (inner[2] >= outer[2]) &&
            (inner[3] <= outer[3]) 
    ) {
        return true
    }
    return false
}

function page_box_intersects(a, b) {
    if (a[1] < b[0]) {
        return false
    } else if (a[0] > b[1]) {
        return false
    } else if (a[3] < b[2]) {
        return false
    } else if (a[2] > b[3]) {
        return false
    }
    return true
}

//----------------------------------------------------------------------------------------
//Element value/info extractors.
//----------------------------------------------------------------------------------------

function element_is_visible(element) {
    if ((element.offsetWidth == 0) || (element.offsetHeight == 0)) {
        return false
    }
    // Check the :after pseudo element first.
    var after_style = window.getComputedStyle(element, ':after'); 
    if (after_style.visibility == "hidden") {
        return false
    }
    // Now check the actual element.
    var style = window.getComputedStyle(element, null)
    if (style.visibility != "visible") {
        return false
    }
    return true
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

//Retrieves the image value directly on an element in the dom hierarchy.
//Note that there may multiple images (ie overlapping image) however they
//will always be returned as one string from this function.
function get_image_direct(element) {
    // Get the tag name.
    var tag_name = element.tagName.toLowerCase()

    // Return the src for images and video elements.
    if ((tag_name === 'img') || (tag_name === 'source')) {
        return element.src
    }

    // We serialize the svg data, but note that it is shallow and doesn't expand
    // <use> elements in the svg.
    if (tag_name === 'svg') {
        var serializer = new XMLSerializer();
        return serializer.serializeToString(element);
    }

    function get_image(style) {
        if (style.backgroundImage.indexOf('url(') == 0) {
            var background_image = style.backgroundImage.slice(4, -1);
            if (background_image != "") {
                // Add on the image scaling and offset, as some images are used like packed textures.
                var scaling = style.backgroundSize
                var offset = style.backgroundPosition
                return background_image + "::" + scaling + "::" + offset
            }
        }
    }
    
    // Otherwise we query the computed style for the background image.
    var after_style = window.getComputedStyle(element, ':after'); 
    var style = window.getComputedStyle(element, null)
    var result = ""
    // Check the :after pseudo element first.
    if (after_style) {
        result = get_image(after_style)
    }
    // If it has no image, then check the actual element.
    if (!result) {
        if (style) {
            result = get_image(style)
        }
    }
    return result
}

//Retrieves the opacity value directly on an element in the dom hierarchy.
function get_opacity_direct(element) {
//  // Check the :after pseudo element first.
//  var after_style = window.getComputedStyle(element, ':after')
//  if (after_style.opacity) {
//      return after_style.opacity
//  }
  // Check the actual element.
  var style = window.getComputedStyle(element, null)
  if (style.opacity) {
      return style.opacity
  }
  return ""
}

function get_background_color_direct(element) {
//    // Check the :after pseudo element first.
//    var after_style = window.getComputedStyle(element, ':after')
//    if (after_style.backgroundColor) {
//        return after_style.backgroundColor
//    }
    // Check the actual element.
    var style = window.getComputedStyle(element, null)
    if (style.backgroundColor) {
        return style.backgroundColor
    }
    return ""
}

// This is detects the back ground element of web pages and also popup divs.
// It's heuristic is to detect fully opaque elements.
// Note this is just a heuristic and may need changes in the future.
function element_is_back_plate(element) {
    bg = get_background_color_direct(element)
    if (bg == 'rgba(0, 0, 0, 0)') {
        return false
    }
    opacity = get_opacity_direct(element)
    if (opacity != 1) {
        return false
    }
    if (bg.startsWith('rgba(')) {
        var sub = bg.slice(5,-1)
        var colors = sub.split(',')
        if (!colors[3].startsWith('1')) {
            return false
        }
    }
    return true
}


//Retrieves the text value directly under an element in the dom hierarchy.
//Note that there may be multiple texts (ie muliple paragraphs) however they
//will always be returned as one string from this function.
function get_text_direct(element) {
    var text = ""
    // Loop through children accumulating text node values.
    for (var c=0; c<element.childNodes.length; c++) {
        var child = element.childNodes[c]
        if (child.nodeType == Node.TEXT_NODE) {
            var value = child.nodeValue
            value = value.trim()
            // Add text if it's not all whitespace.
            if (!is_all_whitespace(value)) {
                text += value
            }
        }
    }
    return text
}

//Returns an array of elements under the given page point.
//The document.elementsFromPoint seems to skip svg elements,
//so this function simply code to include svg elements.
function get_elements_from_point(page_x, page_y) {
    // Use the document.elementsFromPoint class.
    var elements = document.elementsFromPoint(page_x-window.scrollX, page_y-window.scrollY)
    //console.log('num elements: ' + elements.length)
//    var opaque_index = -1
//    for (var i=0; i<elements.length; i++) {
//        console.log('element['+i+']: opacity' + get_opacity_direct(elements[i]) 
//                + ' bg: ' + get_background_color_direct(elements[i]) + ' xpath: ' + get_xpath(elements[i]) )
//    }
    
    // Trim it to point where we reach the back plate of the page.
    for (var i=0; i<elements.length; i++) {
        if (element_is_back_plate(elements[i])) {
            elements.length = i+1
            break
        }
    }
    
    // Build xpath to find all svg elements.
    var xpath = "//*[local-name() = 'svg']"
    var svgs = find_elements_by_xpath(xpath)
    
    // Loop over the elements.
    var svg = null
    var rect = null
    for (var i=0; i<svgs.length; i++) {
        svg = svgs[i]
        
        // Make sure the svg is visible.
        if (!element_is_visible(svg)) {
            continue
        }
        
        // Make sure the svg contains the page point.
        if (!element_contains_point(svg, page_x, page_y)) {
            continue
        }
        
        // Add the svg to the elements.
        elements.unshift(svg)
    }
    return elements
}

//Returns the element with the smallest area containing the given page point.
function get_smallest_element_at_point(page_x, page_y) {
    var elements = get_elements_from_point(page_x, page_y)
    var smallest_area = 0
    var smallest_element = null
    for (var i=0; i<elements.length; i++) {
        var element = elements[i]
        var rect = element.getBoundingClientRect()
        var area = rect.width * rect.height
        if (i == 0) {
            smallest_area = area
            smallest_element = element
        } else if (area < smallest_area) {
            smallest_area = area
            smallest_element = element
        }
    }
    return smallest_element
}

function get_first_text_element_at_point(page_x, page_y) {
    var elements = get_elements_from_point(page_x, page_y)
    for (var i=0; i<elements.length; i++) {
        var element = elements[i]
        var text = get_text_direct(element)
        if (text) {
            return element
        }
    }
    return null
}

function get_first_image_element_at_point(page_x, page_y) {
    var elements = get_elements_from_point(page_x, page_y)
    for (var i=0; i<elements.length; i++) {
        var element = elements[i]
        var image = get_image_direct(element)
        if (image) {
            return element
        }
    }
    return null
}

//Returns an array of values obtained by looping through the 
//elements and retrieving values using the supplied getter.
function get_visible_values(elements, getter) {
    var values = []
    for (var i=0; i<elements.length; i++) {
        var value = getter(elements[i])
        if (!value || is_all_whitespace(value)) {
            continue
        }
        if (values.indexOf(value) >= 0) {
            continue
        }
        values.push(value)
    }
    return values
}

//Returns an array of images values from elements under the given page point.
function get_image_values_from_point(page_x, page_y) {
    var elements = get_elements_from_point(page_x, page_y)
    var values = get_visible_values(elements, get_image_direct)
    return values
}

//Returns an array of text values from elements under the given page point.
function get_text_values_from_point(page_x, page_y) {
    var elements = get_elements_from_point(page_x, page_y)
    var values = get_visible_values(elements, get_text_direct)
    // Values will contain text from bigger overlapping divs with text.
    // Unlike images text won't overlap, so we only want the first text.
    if (values.length > 1) {
        values.length = 1
    }
    return values
}

//----------------------------------------------------------------------------------------
//Element finders.
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
//Note that is the complete opposite of match_element_at_click.
//1) When finding the element from a click position we find the element with the smallest area
//and the longest xpath that contains the click point. (the smallest-deepest element)
//2) Then we retrieve values like text or images using the smallest-deepest element from above.
//These are values are found by looking at all elements in the page, and checking to see if they
//are visible and intersect the smallest-deepest element. If they do then any text of image values
//that they might have are accumulated into a results array.
//3) Typically we will not be storing click positions in the browser automation scripts.
//Instead we will be storing the text/image values determined from above.
//4) Now when it comes to automating the browser at a later time, we won't have the click position,
//but instead we will have the the text/image values of the elements.
//We use thiese values to find the largest element with the shortest xpath length.
//Note how find the largest area and shortest xpath is the exact opposite of the finding the
//smallest-deepest element from step (1).
//----------------------------------------------------------------------------------------

//Returns an array of elements with the given xpath.
function find_elements_by_xpath(xpath) {
  var set = document.evaluate(xpath, document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null)
  var elements = []
  for (var i=0; i<set.snapshotLength; i++) {
      elements.push(set.snapshotItem(i))
  }
  return elements
}

//Returns an array of elements selected by the matcher.
function find_elements(getter, target_values) {
    // Build xpath to find all elements.
    var xpath = "//*"
    var elements = find_elements_by_xpath(xpath)
    
    // Loop over the elements.
    var element = null
    var value = null
    var candidates = []
    for (var i=0; i<elements.length; i++) {
        element = elements[i]
        
        // Make sure the element is visible.
        if (!element_is_visible(element)) {
            continue
        }
        
        // Skip the element if it's part of the smash browse menu.
        if (smash_browse_context_menu.element_is_part_of_menu(element)) {
            continue
        }
        
        // See if this elements value matches something in target_values
        value = getter(element)
        if (target_values.indexOf(value) >= 0) {
            candidates.push(element)
        }
    }
    
    //console.log('num candidates: ' + candidates.length)
    
    // Determines arrays of surrounding elements for each element.
    var results = []
    var overlaps = []
    for (var i=0; i<candidates.length; i++) {
        overlaps.push([])
        for (var j=0; j<candidates.length; j++) {
            if (element_intersects(candidates[j], candidates[i])) {
                overlaps[i].push(getter(candidates[j]))
            }
        }
    }
    
//    console.log('num overlaps: ' + overlaps.length)
//    for(var i=0; i<overlaps.length; i++) {
//        console.log('overlaps length['+i+']: ' + overlaps[i].length)
//    }
    
    // Find the elements which have surrounding elements matching the target_values.
    var matching = []
    for (var i=0; i<overlaps.length; i++) {
        var found = true
        for (var j=0; j<target_values.length; j++) {
            if (overlaps[i].indexOf(target_values[j]) < 0) {
                found = false
                break
            }
        }
        if (found) {
            matching.push(candidates[i])
        }
    }
    
    // Initialize eliminated to all false.
    var eliminated = []
    for (var i=0; i<matching.length; i++) {
        eliminated.push(false)
    }
    
    // Coalesce all contained elements together
    // and find the smallest most internal element
    for (var i=0; i<matching.length; i++) {
        if (eliminated[i]) {
            continue
        }
        for(var j=0; j<matching.length; j++) {
            if (j==i) {
                continue
            }
            if (eliminated[j]) {
                continue
            }
            if (element_contains(matching[j], matching[i])) {
                eliminated[j] = true
            }
        }
    }
    
    // Extract the non eliminated elements out.
    var results = []
    for (var i=0; i<eliminated.length; i++) {
        if (!eliminated[i]) {
            results.push(matching[i])
        }
    }
    
    // Phew we're done!
    return results
}

//Returns an array of elements which have matching image values.
function find_elements_by_image_values(image_values) {
    return find_elements(get_image_direct, image_values)
}

//Returns an array of elements which have matching text values.
function find_elements_by_text_values(text_values) {
    return find_elements(get_text_direct, text_values)
}

