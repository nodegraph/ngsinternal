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
    this.page_box.set_from_client_rect(this.get_client_rect())
}

// Get our wrap type.
ElemWrap.prototype.get_wrap_type = function() {
    if (this.get_text()) {
        return this.wrap_type.text
    }
    if (this.get_image()) {
        return this.wrap_type.image
    }
    if (this.is_input()) {
        return this.wrap_type.input
    }
    if (this.is_select()) {
        return this.wrap_type.select
    }
    return -1
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

//Note this containment test uses a sigma of 1.0.
ElemWrap.prototype.contains_point = function(page_x, page_y) {
  return this.page_box.contains_point(page_x, page_y)
}

ElemWrap.prototype.get_horizontal_overlap = function(elem_wrap) {
    var left = Math.max(this.page_box.left, elem_wrap.page_box.left)
    var right = Math.min(this.page_box.right, elem_wrap.page_box.right)
    return Math.max(0,right-left)
}

ElemWrap.prototype.get_vertical_overlap = function(elem_wrap) {
    var top = Math.max(this.page_box.top, elem_wrap.page_box.top)
    var bottom = Math.min(this.page_box.bottom, elem_wrap.page_box.bottom)
    return Math.max(0,bottom-top)
}

ElemWrap.prototype.is_visible = function() {
    if ((this.element.offsetWidth == 0) || (this.element.offsetHeight == 0)) {
        return false
    }
    if (this.page_box.get_width() == 0) {
        return false
    }
    if (this.page_box.get_height() == 0) {
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

//Returns true if this element is on top for most of its surface area.
//This is used to detect elements that the user can interact with, or elements to shift to.
//- num_samples_xy indicates how fine to sample within the element.
//- min_coverage indicates how many samples need to hit ourself to register as being topmost.
ElemWrap.prototype.is_topmost = function(getter, min_coverage = 0.9, num_samples_xy = new Point(5,5)) {
    if ((this.page_box.get_width() <= 2) && (this.page_box.get_height() <= 2)) {
        return true
    }
    
    //console.log('element bounds: ' + this.page_box.left + ',' + this.page_box.right + ',' + this.page_box.top + ',' + this.page_box.bottom)
    
    // Samples a num_samples_xy sized grid that is pushed into the element a bit.
    // In other words it won't sample exactly along the edges of the element, but a bit inside.
    // We push in by at least one pixel from each edge along with 1/num_segments of revised width and height.
    var delta_x = (this.page_box.get_width()-2) / (num_samples_xy.x + 1)
    var delta_y = (this.page_box.get_height()-2) / (num_samples_xy.y + 1)
    
    // Loop over the grid of samples.
    var hit = 0
    var miss = 0
    for (var i=0; i<num_samples_xy.x; i++) {
        for (var j=0; j<num_samples_xy.y; j++) {
            var x = this.page_box.left + 1 + (i+1) * delta_x
            var y = this.page_box.top + 1 + (j+1) * delta_y
            var elem_wrap = g_page_wrap.get_first_elem_wrap_at(getter, x, y)
            if (elem_wrap) {
                if (elem_wrap.equals(this)) {
                    hit += 1
                } else {
                    miss +=1
                }
            }
        }
    }
    
    // Check coverage amount.
    //console.log('coverage: ' + (hit/(hit+miss)) + ' hit: ' + hit + ' miss: ' + miss)
    if (hit / (hit + miss) >= min_coverage) {
        return true
    }
    return false
}

// Returns the next topmost element on one side of us.
ElemWrap.prototype.get_neighboring_elem_wrap = function(getter, side) {
    var beam = this.page_box.get_beam(side)
    
    // Get all elem wraps intersecting the beam.
    var elem_wraps = g_page_wrap.get_all_elem_wraps_intersecting_page_box(beam)
    
    // Select out those elem wraps returning any value with getter.
    var candidates = []
    for (var i=0; i<elem_wraps.length; i++) {
        var value = getter.call(elem_wraps[i])
        if (value) {
            candidates.push(elem_wraps[i])
        }
    }
    
    // Find the closest candidate to us.
    var min_dist = 99999999
    var min_candidate = null
    for (var i=0; i<candidates.length; i++) {
        var dist = candidates[i].page_box.is_oriented_on(side,this.page_box)
        // A dist greater than zero means it's properly oriented.
        if (dist > 0) {
            // Is this closer than any other candidate.
            if (!min_candidate || dist < min_dist) {
                // Finally make sure this is a top most element, that the user can actually interact with.
                if (candidates[i].is_topmost(getter)) {
                    min_candidate = candidates[i]
                    min_dist = dist
                }
            }
        }
    }
    
    // Return our result.
    return min_candidate
}

ElemWrap.prototype.get_getter = function(wrap_type) {
    var getter = null
    switch(wrap_type) {
        case ElemWrap.prototype.wrap_type.text:
            getter = ElemWrap.prototype.get_text
            break
        case ElemWrap.prototype.wrap_type.image:
            getter = ElemWrap.prototype.get_image
            break
        case ElemWrap.prototype.wrap_type.input:
            getter = ElemWrap.prototype.is_input
            break
        case ElemWrap.prototype.wrap_type.select:
            getter = ElemWrap.prototype.is_select
            break
    }
    return getter
}

ElemWrap.prototype.shift = function(dir, wrap_type) {
    // Find the proper getter for the type we're shifting to.
    var getter = this.get_getter(wrap_type)
    var elem_wrap = this.get_neighboring_elem_wrap(getter, dir)
    if (elem_wrap) {
        this.element = elem_wrap.element
        console.log('element shifted to: ' + this.get_xpath())
    }
}


//Returns the next topmost element on one side of us.
ElemWrap.prototype.get_similar_neighbors = function(side, match_criteria) {
    var beam = this.page_box.get_beam(side)
    
    // Get all elem wraps intersecting the beam.
    var elem_wraps = g_page_wrap.get_all_elem_wraps_intersecting_page_box(beam)
    //console.log('num elem wraps: ' + elem_wraps.length)
    
    // Select out those elem wraps returning any value with getter.
    var getter = this.get_getter(this.get_wrap_type())
    var candidates = []
    for (var i=0; i<elem_wraps.length; i++) {
        var value = getter.call(elem_wraps[i])
        if (value) {
            candidates.push(elem_wraps[i])
        }
    }
    //console.log('num candidates: ' + candidates.length)
    
    // Find the candidates which match us.
    var matches = []
    for (var i=0; i<candidates.length; i++) {
        if (match_criteria.matches(this, candidates[i])) {
            matches.push(candidates[i])
        }
    }
    //console.log('num matches: ' + matches.length)
    
    // Return our result.
    return matches
}


//----------------------------------------------------------------------------------------
//Element Behavioral Properties.
//----------------------------------------------------------------------------------------

ElemWrap.prototype.get_scroll_amount = function() {
    return this.element.scrollTop
}

ElemWrap.prototype.set_scroll_amount = function(amount) {
    this.element.scrollTop = amount
}

ElemWrap.prototype.get_scroll_amounts = function() {
    var bars = this.get_parenting_scroll_bars()
    console.log("num parenting bars: " + bars.length)
    var amounts = []
    for (var i=0; i<bars.length; i++) {
        var scroll = bars[i].get_scroll_amount()
        console.log('scroll: ' + scroll)
        amounts.push(scroll)
    }
    return amounts
}

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
ElemWrap.prototype.get_parenting_scroll_bars = function() {
    var elem_wraps = []
    var parent = this.get_parent()
    while (parent) {
        // The topmost scrollbar will be the document.body.
        // We need to record the scroll on the document.body as well.
        if (parent.element == document.body) {
            elem_wraps.push(parent)
            return elem_wraps
        }
        // If we find any scroll bars, add it.
        if (parent.has_horizontal_scroll_bar() || (parent.has_vertical_scroll_bar())) {
            elem_wraps.push(parent)
        }
        // Check our next parent.
        parent = parent.get_parent()
    }
    return elem_wraps
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

