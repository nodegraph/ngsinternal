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
        //console.log('Found outer, inner: ' + JSON.stringify(outer) + "," + JSON.stringify(inner))
        return true
    }
    //console.log('NOT found outer, inner: ' + JSON.stringify(outer) + "," + JSON.stringify(inner))
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

// Returns an array of values extracted from elements overlapping the given element.
// Makes use of a functor getter to retrieve values.
function get_visible_values(target_element, getter) {
    // Build xpath to find all elements.
    var xpath = "//*"
    var elements = find_elements_by_xpath(xpath)
    console.log("determining values from num elememnts: " + elements.length)
    
    // Loop over the elements.
    var element = null
    for (var i=0; i<elements.length; i++) {
        element = elements[i]
        
        // Make sure the element is visible.
        if ((element.offsetWidth == 0) || (element.offsetHeight == 0)) {
            continue
        }
        
        // Skip the element if it's part of the smash browse menu.
        if (smash_browse_context_menu.element_is_part_of_menu(element)) {
            continue
        }
        
        // Retrieve values using the getter.
        getter(element)
    }
}

// Functor for get_visible_values.
// Grabs text from the element and pushes it onto results.
function text_getter(data, element) {
    // We want all text inside the original element.
    // So we make sure that the element is fully inside the original element.
    if (!element_contains(data.target_element, element)) {
        return
    }
    
    // Loop through children accumulating text node values.
    for (var c=0; c<element.childNodes.length; c++) {
        var child = element.childNodes[c]
        if (child.nodeType == Node.TEXT_NODE) {
            var text = child.nodeValue
            // Add text if it's not all whitespace.
            if (!is_all_whitespace(text)) {
                data.results.push(text)
            }
        }
    }
}

//Functor for get_visible_values.
//Grabs images from the element and pushes it onto results.
function image_getter(data, element) {
    // We want all text inside the original element.
    // So we make sure that the element is fully inside the original element.
    if (!element_contains(element, data.target_element)) {
        return
    }
    
    // Get the tag name.
    var tag_name = element.tagName.toLowerCase()
    
    // Return the src for images and video elements.
    if ((tag_name === 'img') || (tag_name === 'video')) {
        if (data.results.indexOf(element.src)<0) {
            console.log("xpath: " + get_xpath(element))
            console.log("elem source: " + element.src)
            data.results.push(element.src)
        }
    }
    
    // We can't really get unique data for svgs so just return some text.
    if (tag_name === 'svg') {
        data.results.push('svg')
    }
    
    // Otherwise we query the computed style for the background image.
    var style = element.currentStyle || window.getComputedStyle(element, false)
    if (style) {
        // console.log("ssssstyle: " + JSON.stringify(style))
        if (style.backgroundImage.indexOf('url(') == 0) {
            var background_image = style.backgroundImage.slice(4, -1);
            if (background_image != "") {
                if (data.results.indexOf(background_image)<0) {
                    console.log("xpath: " + get_xpath(element))
                    console.log("background image: " + background_image)
                    data.results.push(background_image)
                }
            }
        }
    }
}

function get_image_direct(element) {
    // Get the tag name.
    var tag_name = element.tagName.toLowerCase()

    // Return the src for images and video elements.
    if ((tag_name === 'img') || (tag_name === 'video')) {
        return element.src
    }

    // We can't really get unique data for svgs so just return some text.
    if (tag_name === 'svg') {
        return 'svg'
    }

    // Otherwise we query the computed style for the background image.
    var style = element.currentStyle || window.getComputedStyle(element, false)
    if (style) {
        if (style.backgroundImage.indexOf('url(') == 0) {
            var background_image = style.backgroundImage.slice(4, -1);
            if (background_image != "") {
                return background_image
            }
        }
    }
}

function get_text_direct(data, element) {
    var text = ""
    // Loop through children accumulating text node values.
    for (var c=0; c<element.childNodes.length; c++) {
        var child = element.childNodes[c]
        if (child.nodeType == Node.TEXT_NODE) {
            var value = child.nodeValue
            // Add text if it's not all whitespace.
            if (!is_all_whitespace(text)) {
                text += value
            }
        }
    }
    return text
}

//Returns an array of text which are all overlapping the given element.
function get_text_values(element) {
    var data = {
            target_element: element,
            results: []
    }
    get_visible_values(element, text_getter.bind(undefined, data))
    return data.results
}

//Returns an array of image urls which are all overlapping the given element.
function get_image_values(element) {
    var data = {
            target_element: element,
            results: []
    }
    get_visible_values(element, image_getter.bind(undefined, data))
    return data.results
}

function get_image_values2(page_x, page_y) {
    var elements = document.elementsFromPoint(page_x-window.scrollX, page_y-window.scrollY)
    console.log("xxx num elements: " + elements.length)
    
//    var svgroot = window.document; //document.documentElement;
//    var myRect = svgroot.createSVGRect();
//    myRect.x = page_x-window.scrollX;
//    myRect.y = page_y-window.scrollY;
//    myRect.width = 1;
//    myRect.height = 1;
//    var intersectList = 
//        svgroot.getIntersectionList(myRect,null);
//    for (var i=0; i<intersectList.length; i++) {
//        console.log("intersection["+i+"]: " + get_xpath(intersectList[i]))
//    }
    
    var images = []
    for (var i=0; i<elements.length; i++) {
        
        console.log('examing: ' + get_xpath(elements[i]))
        var image = get_image_direct(elements[i])
        if (!image) {
            continue
        }
        if (images.indexOf(image) >= 0) {
            continue
        }
        images.push(image)
    }
    return images
}

//Returns true if the element has an immediate textnode with text.
function element_has_direct_text(element) {
    var data = {
            target_element: element,
            results: []
    }
    text_getter(data, element)
    if (data.results.length) {
        return true
    }
    return false
}

//Returns true if the element is and image or video or a div with a background image.
function element_has_direct_image(element) {
    var data = {
            target_element: element,
            results: []
    }
    image_getter(data, element)
    if (data.results.length) {
        return true
    }
    return false
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
function find_elements(matcher) {
    // Build xpath to find all elements.
    var xpath = "//*"
    var elements = find_elements_by_xpath(xpath)
    
    // Loop over the elements.
    var element = null
    for (var i=0; i<elements.length; i++) {
        element = elements[i]
        
        // Make sure the element is visible.
        if ((element.offsetWidth == 0) || (element.offsetHeight == 0)) {
            continue
        }
        
        // Skip the element if it's part of the smash browse menu.
        if (smash_browse_context_menu.element_is_part_of_menu(element)) {
            continue
        }
        
        // Check if we have a match.
        matcher(element)
    }
}

function match_smallest_deepest(data, element) {
    var rect = element.getBoundingClientRect()
    var area = rect.width * rect.height
    var xpath_length = get_xpath(element).split('/').length
    var found = false
    
    // Have we found a better element.
    if (!data.smallest_elem) {
        found = true
    } else {
        // We always want the element with the smallest area.
        if (area < data.smallest_area) {
            found = true
        } 
        // However if there is a tie, then we want the one with the longest xpath.
        else if (area == data.smallest_area) {
            if (xpath_length > data.longest_xpath_length) {
                found = true
            }
        }
    }
    
    // If we found a better element, update our data structures.
    if (found) {
        data.smallest_elem = element
        data.smallest_area = area
        data.longest_xpath_length = xpath_length
        data.results.length = 0
        data.results.push(element)
    }
}

//Used to find the deepest element at the click point.
function match_element_at_click(data, element) {
    // Make sure the element intersects the internal element.
    if (!element_contains_point(element, data.page_x, data.page_y)) {
        return
    }
    match_smallest_deepest(data, element)
}

//Used to find the deepest image around the element determined from the click point (above).
function match_smallest_encompassing_image(data, element) {
    // Make sure the element intersects the internal element.
    if (!element_contains(element, data.target_element)) {
        return
    }
    // Make sure the element has an image directly.
    if (!element_has_direct_image(element)) {
        return
    }
//    if (get_image_values(element).length == 0) {
//        return
//    }
    match_smallest_deepest(data, element)
}

//Used to find the deepest text around the element determined from the click point (above).
function match_smallest_encompassing_text(data, element) {
    // Make sure the element intersects the internal element.
    if (!element_contains(element, data.target_element)) {
        return
    }
    // Make sure the element has an image directly.
    if (!element_has_direct_text(element)) {
        return
    }
//    if (get_text_values(element).length == 0) {
//        return
//    }
    match_smallest_deepest(data, element)
}

//Results is set to have just the one given element if it has the 
//largest area and shortest xpath we have seen so far.
function match_image_values(data, element) {
  // Find the clickable element.
//  element = get_clickable_element(element)
//  if (!element) {
//      return
//  }
  
//  // Find the first encompassing image element.
//  var results = find_visible_image_element(element)
//  if (results.length == 0) {
//      console.log('yyy')
//      return
//  }
//  element = results[0]
//  if (!element) {
//      console.log('xxx')
//  }
  
  // Get the elements image values.
  var values = get_image_values(element)
  // If it doesn't match the image values we're looking for, skip it.
  if (!arrays_are_equal(values,data.image_values)) {
      return
  }
  // Get the xpath.
  var xpath = get_xpath(element)
  // If a prefix of xpath already exists in results then skip it.
  // We only want the upper part of paths, when one path is a prefix of another.
  // We also want the biggest containing element.
  // Note that we will see prefixes first, because the elements are search in page order.
  for (var i=0; i<data.results.length; i++) {
      if (xpath.indexOf(get_xpath(data.results[i])) == 0) {
          return
      }
      if (element_contains(data.results[i], element)) {
          return
      }
  }
  data.results.push(element)
}

function match_text_values(data, element) {
    // Get the text values.
    var values = get_text_values(element)
    // If it doesn't matches the text values we're looking for then skip it.
    if (!arrays_are_equal(values,data.text_values) ) {
        return
    }
    // Get the xpath.
    var xpath = get_xpath(element)
    // If a prefix of xpath already exists in results then skip it.
    // We only want the upper xpaths.
    // We also want the biggest containing element.
    // Note that we will see prefixes first, because the elements are search in page order.
    for (var i=0; i<data.results.length; i++) {
        if (xpath.indexOf(get_xpath(data.results[i])) == 0) {
            return
        }
        if (element_contains(data.results[i], element)) {
            return
        }
    }
    data.results.push(element)
}

// Finds the smallest and deepest element at the given position.
function find_visible_inner_element(page_x, page_y) {
    var data = {
            page_x : page_x, 
            page_y : page_y,
            results : []
    }
    find_elements(match_element_at_click.bind(undefined, data))
    return data.results
}

// Get the visible element.
function find_visible_image_element(element) {
    data = {
        target_element: element,
        results : []
    }
    find_elements(match_smallest_encompassing_image.bind(undefined, data))
    return data.results
}

function find_visible_image_element_at_point(page_x, page_y) {
    var results = find_visible_inner_element(page_x, page_y);
    if (results.length == 0) {
        return null
    }
    return find_visible_image_element(results[0])
}

function find_visible_text_element(page_x, page_y) {
    // Find the inner most element at the click point.
    var data = {
            page_x : page_x, 
            page_y : page_y,
            results : []
    }
    find_elements(match_element_at_click.bind(undefined, data))
    return data.results
}

//Returns an array of elements which have matching image values.
function find_elements_by_image_values(image_values) {
    data = {
            image_values : image_values,
            results : []
    }
    find_elements(match_image_values.bind(undefined, data))
    return data.results
}

//Returns an array of elements which have matching text values.
function find_elements_by_text_values(text_values) {
    var data = {
            text_values : text_values,
            results : []
    }
    find_elements(match_text_values.bind(undefined, data))
    return data.results
}

