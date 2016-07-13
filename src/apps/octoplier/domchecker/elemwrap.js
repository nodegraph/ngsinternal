// This class wraps the dom element with extra functionality.
var ElemWrap = function(element) {
    this.element = element
    this.page_box = new PageBox(this.get_client_rect())
}

// Returns an unique opaque pointer for the purposes of identification.
// When two ElemWraps return the same id, they represent a wrapper around the same dom element.
ElemWrap.prototype.get_id = function() {
    return this.element
}

// Returns true if this and the other ElemWrap represent the same dom element.
ElemWrap.prototype.equals = function(other) {
    if (this.get_id() == other.get_id()) {
        return true
    }
    return false
}

// Wrap type.
ElemWrap.prototype.wrap_type = {
        text: 0,
        image: 1,
        input: 2,
        select: 3,
}

// Direction.
ElemWrap.prototype.direction = {
        left: 0,
        right: 1,
        up: 2,
        down: 3,
}

// Update all internal state.
ElemWrap.prototype.update = function() {
    this.page_box.set_from_elem_wrap(this)
}

//----------------------------------------------------------------------------------------
//Element geometry.
//----------------------------------------------------------------------------------------

ElemWrap.prototype.get_client_rect = function() {
    return this.element.getBoundingClientRect()
}

//Returns true if the outer element contains the inner element.
ElemWrap.prototype.contains = function(elem_wrap) {
  var inner_box = new PageBox(elem_wrap.get_client_rect())
  return this.page_box.contains(inner_box)
}

ElemWrap.prototype.intersects = function(elem_wrap) {
  var other_box = new PageBox(elem_wrap.get_client_rect())
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
//Element Identification. (This is usually only valid during one browsing session.)
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

//----------------------------------------------------------------------------------------
//Element Shifting.
//----------------------------------------------------------------------------------------

//Checks to make sure this is nothing on top of us, according to the getter (image,text,input,select).
ElemWrap.prototype.is_top_along_vertical = function(getter, x) {
    var delta = 1
    for (var y=this.page_box.top+delta; y<this.page_box.bottom; y+=delta) {
        var top = g_page_wrap.get_first_elem_wrap_at(getter, x, y)
        if (top && !top.equals(this)) {
            return false
        }
    }
    return true
}

ElemWrap.prototype.get_next_vertical = function(getter, x, start_y, end_y) {
    // Determine our y increment.
    var delta = 1
    if (start_y > end_y) {
        delta = -1
    }
    // Our speed.
    var speed = 1
    // Our candidates
    var ids_seen = [this.get_id()]
    // Start loop.
    var y = start_y
    while ((delta * y) <= (delta*end_y)) {
        var elem_wrap = g_page_wrap.get_first_elem_wrap_at(getter, x, y)
        if (elem_wrap && (ids_seen.indexOf(elem_wrap.get_id())<0)) {
            ids_seen.push(elem_wrap.get_id())
            if (elem_wrap.is_top_along_vertical(getter, x)) {
                return elem_wrap
            }
        }
        // Increment y.
        y += delta*speed
    }
    return null
}

ElemWrap.prototype.shift_vertical = function(getter, start_y, end_y) {
    // Todo: refactor this to use an array of samples instead of just 3.
    //       we want to shift to the element which intersects the most number of sample rays.
    
    //  We scan three lines at once along left_x, mid_x and right_x.
    var mid_x = this.page_box.get_mid_x()
    var left_x = (this.page_box.left + mid_x) / 2.0
    var right_x = (this.page_box.right + mid_x) / 2.0
    // Get the elem wraps.
    var left = this.get_next_vertical(getter, left_x, start_y, end_y)
    var mid = this.get_next_vertical(getter, mid_x, start_y, end_y)
    var right = this.get_next_vertical(getter, right_x, start_y, end_y)
    
    if (left)
    console.log('left: ' + left.get_xpath())
    if (mid)
    console.log('mid: ' + mid.get_xpath())
    if (right)
    console.log('right: ' + right.get_xpath())
    
    // Find the closest to the start_y.
    if (start_y > end_y) {
        var bottoms = []
        if (left) {
            bottoms.push(left.page_box.bottom)
        }
        if (mid) {
            bottoms.push(mid.page_box.bottom)
        }
        if (right) {
            bottoms.push(right.page_box.bottom)
        }
        var max = Math.max(...bottoms)
        if (left && left.page_box.bottom == max) {
            this.element = left.element
            return true
        }
        if (mid && mid.page_box.bottom == max) {
            this.element = mid.element
            return true
        }
        if (right && right.page_box.bottom == max) {
            this.element = right.element
            return true
        }
    } else {
        var tops = []
        if (left) {
            tops.push(left.page_box.top)
        }
        if (mid) {
            tops.push(mid.page_box.top)
        }
        if (right) {
            tops.push(right.page_box.top)
        }
        var min = Math.min(...tops)
        if (left && left.page_box.top == min) {
            this.element = left.element
            return true
        }
        if (mid && mid.page_box.top == min) {
            this.element = mid.element
            return true
        }
        if (right && right.page_box.top == min) {
            this.element = right.element
            return true
        }
    }
    return false
}

ElemWrap.prototype.shift_up = function(getter) {
    var start_y = this.page_box.top
    var end_y = 0
    this.shift_vertical(getter, start_y, end_y)
}

ElemWrap.prototype.shift_down = function(getter) {
    var start_y = this.page_box.bottom
    var end_y = g_page_wrap.get_height()
    this.shift_vertical(getter, start_y, end_y)
}

ElemWrap.prototype.shift = function(dir, wrap_type) {
    // Find the proper getter for the type we're shifting to.
    var getter = null
    switch(wrap_type) {
        case this.wrap_type.text:
            getter = this.get_text
            break
        case this.wrap_type.image:
            getter = this.get_image
            break
        case this.wrap_type.input:
            getter = this.is_input
            break
        case this.wrap_type.select:
            getter = this.is_select
            break
    }
    // Find the proper scanner for the direction we're searching in.
    var scanner = null
    switch(dir) {
        case this.direction.left:
            scanner = this.shift_left
            break
        case this.direction.right: 
            scanner = this.shift_right
            break
        case this.direction.up: 
            scanner = this.shift_up
            break
        case this.direction.down: 
            scanner = this.shift_down
            break
    }
    // Now call the function.
    return scanner.call(this, getter)
}

//----------------------------------------------------------------------------------------
//Element Behavioral Properties.
//----------------------------------------------------------------------------------------

//Returns true if the element has horizontal scroll bars.
ElemWrap.prototype.has_horizontal_scroll_bar = function() {
    if (!(this.element.scrollWidth > this.element.clientWidth)) {
        return false
    }
    
    // Check the style.
    var style = window.getComputedStyle(this.element, null);
    if (style.overflow == "scroll" || style.overflow == "auto" ||
            style.overflowX == "scroll" || style.overflowX == "auto") {
        return true
    }
    return false
}

//Returns true if the element has vertical scroll bars.
ElemWrap.prototype.has_vertical_scroll_bar = function() {
    if (!(this.element.scrollHeight > this.element.clientHeight)) {
        return false
    }
    
    // Check the style.
    var style = window.getComputedStyle(this.element, null);
    if (style.overflow == "scroll" || style.overflow == "auto" ||
            style.overflowY == "scroll" || style.overflowY == "auto") {
        return true
    }
    return false
}

ElemWrap.prototype.has_any_scroll_bar = function() {
    return this.has_vertical_scroll_bar() || this.has_horizontal_scroll_bar()
}

//This is detects the back ground element of web pages and also popup divs.
//It's heuristic is to detect fully opaque elements.
//Note this is just a heuristic and may need changes in the future.
ElemWrap.prototype.is_back_plate = function() {
    bg = this.get_background_color()
    if (bg == 'rgba(0, 0, 0, 0)') {
        return false
    }
    opacity = this.get_opacity()
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

ElemWrap.prototype.get_parent = function() {
    if (!this.element.parentElement) {
        return null
    }
    return new ElemWrap(this.element.parentElement)
}

//Returns a parent elem wrap with scrollbars, if one exists.
//Otherwise returns null.
ElemWrap.prototype.get_parent_with_scroll_bars = function() {
    var parent = this.get_parent()
    while (parent) {
        // If we hit the top document body, we return that.
        // We consider it to have scroll bars even if it doesn't.
        if (parent.element == document.body) {
            return parent
        }
        // If we find any scroll bars, we've found it.
        if (parent.has_horizontal_scroll_bar() || (parent.has_vertical_scroll_bar())) {
            return parent
        }
        parent = parent.get_parent()
    }
    return parent
}

//----------------------------------------------------------------------------------------
//Element Properties/Values.
//----------------------------------------------------------------------------------------

//Retrieves the image value directly on an element in the dom hierarchy.
//Note that there may multiple images (ie overlapping image) however they
//will always be returned as one string from this function.
ElemWrap.prototype.get_image = function() {
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

    function get_image_from_style(style) {
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
        result = get_image_from_style(after_style)
    }
    // If it has no image, then check the actual element.
    if (!result) {
        if (style) {
            result = get_image_from_style(style)
        }
    }
    return result
}

//Retrieves the text value directly under an element in the dom hierarchy.
//Note that there may be multiple texts (ie muliple paragraphs) however they
//will always be returned as one string from this function.
ElemWrap.prototype.get_text = function() {
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

ElemWrap.prototype.get_text_or_image = function() {
    var text = this.get_text()
    if (text) {
        return text
    }
    var image = this.get_image()
    if (image) {
        return image
    }
}

ElemWrap.prototype.get_tag_name = function() {
    return this.element.tagName.toLowerCase()
}

ElemWrap.prototype.is_input = function() {
    return (this.element.tagName.toLowerCase() == 'input')
}

ElemWrap.prototype.is_select = function() {
    return (this.element.tagName.toLowerCase() == 'select')
}


//Retrieves the opacity value directly on an element in the dom hierarchy.
ElemWrap.prototype.get_opacity = function() {
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

ElemWrap.prototype.get_background_color = function() {
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

