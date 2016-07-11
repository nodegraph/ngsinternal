// This class wraps the dom element with extra functionality.
var ElemWrap = function(element) {
    this.element = element
    this.page_box = new PageBox(this.get_client_rect())
}

//----------------------------------------------------------------------------------------
//Element geometry.
//----------------------------------------------------------------------------------------

ElemWrap.prototype.get_client_rect = function() {
    return this.element.getBoundingClientRect()
}

//Returns true if the outer element contains the inner element.
ElemWrap.prototype.contains = function(wrapper) {
  var inner_box = new PageBox(wrapper.get_client_rect())
  return this.page_box.contains(inner_box)
}

ElemWrap.prototype.intersects = function(wrapper) {
  var other_box = new PageBox(wrapper.get_client_rect())
  return this.page_box.intersects(other_box)
}

ElemWrap.prototype.contains_page_point = function(page_x, page_y) {
  return this.page_box.contains_page_point(page_x, page_y)
}

ElemWrap.prototype.is_visible = function() {
    if ((this.element.offsetWidth == 0) || (this.element.offsetHeight == 0)) {
        return false
    }
    // Check the :after pseudo element first.
    var after_style = window.getComputedStyle(this.element, ':after'); 
    if (after_style.visibility == "hidden") {
        return false
    }
    // Now check the actual element.
    var style = window.getComputedStyle(this.element, null)
    if (style.visibility != "visible") {
        return false
    }
    return true
}

//----------------------------------------------------------------------------------------
//Element values.
//----------------------------------------------------------------------------------------

//Get the full xpath for an element, anchored at the html document.
//For simplicity each path element always has a [] index suffix.
ElemWrap.prototype.get_xpath = function() {
    var path = [];
    var element = this.element
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
ElemWrap.prototype.get_image_direct = function() {
    // Get the tag name.
    var tag_name = this.element.tagName.toLowerCase()

    // Return the src for images and video elements.
    if ((tag_name === 'img') || (tag_name === 'source')) {
        return this.element.src
    }

    // We serialize the svg data, but note that it is shallow and doesn't expand
    // <use> elements in the svg.
    if (tag_name === 'svg') {
        var serializer = new XMLSerializer();
        return serializer.serializeToString(this.element);
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
    var after_style = window.getComputedStyle(this.element, ':after'); 
    var style = window.getComputedStyle(this.element, null)
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

ElemWrap.prototype.get_tag_name_direct = function() {
    return this.element.tagName.toLowerCase()
}

//Retrieves the opacity value directly on an element in the dom hierarchy.
ElemWrap.prototype.get_opacity_direct = function() {
//  // Check the :after pseudo element first.
//  var after_style = window.getComputedStyle(element, ':after')
//  if (after_style.opacity) {
//      return after_style.opacity
//  }
  // Check the actual element.
  var style = window.getComputedStyle(this.element, null)
  if (style.opacity) {
      return style.opacity
  }
  return ""
}

ElemWrap.prototype.get_background_color_direct = function() {
//    // Check the :after pseudo element first.
//    var after_style = window.getComputedStyle(element, ':after')
//    if (after_style.backgroundColor) {
//        return after_style.backgroundColor
//    }
    // Check the actual element.
    var style = window.getComputedStyle(this.element, null)
    if (style.backgroundColor) {
        return style.backgroundColor
    }
    return ""
}

// This is detects the back ground element of web pages and also popup divs.
// It's heuristic is to detect fully opaque elements.
// Note this is just a heuristic and may need changes in the future.
ElemWrap.prototype.is_back_plate = function() {
    bg = this.get_background_color_direct()
    if (bg == 'rgba(0, 0, 0, 0)') {
        return false
    }
    opacity = this.get_opacity_direct()
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
ElemWrap.prototype.get_text_direct = function() {
    var text = ""
        
    // Loop through children accumulating text node values.
    for (var c=0; c<this.element.childNodes.length; c++) {
        var child = this.element.childNodes[c]
        if (child.nodeType == Node.TEXT_NODE) {
            var value = child.nodeValue
            value = value.trim()
            // Add text if it's not all whitespace.
            if (!is_all_whitespace(value)) {
                text += value
            }
        }
    }
    
    if  (this.element.tagName.toLowerCase() == 'input') {
        console.log('found input')
        var value = this.element.getAttribute('value')
        if (!is_all_whitespace(value)) {
            text += value
        }
    }
    
    var before_style = window.getComputedStyle(this.element, ':before'); 
    value = before_style.content
    value = strip_quotes(value)
    if (!is_all_whitespace(value)) {
        text += value
    }
    
    var after_style = window.getComputedStyle(this.element, ':after'); 
    value = after_style.content
    value = strip_quotes(value)
    if (!is_all_whitespace(value)) {
        text += value
    }
    return text
}