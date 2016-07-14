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

// Returns true is this is the element along a ray over the element.
// Start must be a starting a point contained within the element.
// Delta is the increment to advance from the start point.
ElemWrap.prototype.is_top_along_ray = function(getter, start, delta, min_coverage = 0.8) {
    var debug_method = true
    if (debug_method && !this.page_box.contains_point(start.x, start.y)) {
        console.log("XXXXXXXXXXXXXX: error!")
        console.log('start: ' + start.x + "," + start.y)
        console.log('bounds: ' + this.page_box.left + "," + this.page_box.right + "," + this.page_box.bottom + "," + this.page_box.top + ",")
    }
    
    // Loop start state.
    var hit = 0
    var miss = 0
    var pos = new Point(start)
    
    // Loop along positive delta until we're off the element.
    while (this.page_box.contains_point(pos.x, pos.y)) {
        var elem_wrap = g_page_wrap.get_first_elem_wrap_at(getter, pos.x, pos.y)
        if (elem_wrap && elem_wrap.equals(this)) {
            hit += 1
        } else {
            miss +=1
        }
        pos.increment(delta)
    }
    
    // Loop along negative delta until we're off the element.
    pos.assign(start)
    pos.decrement(delta)
    while (this.page_box.contains_point(pos.x, pos.y)) {
        var elem_wrap = g_page_wrap.get_first_elem_wrap_at(getter, pos.x, pos.y)
        if (elem_wrap && elem_wrap.equals(this)) {
            hit += 1
        } else {
            miss +=1
        }
        pos.decrement(delta)
    }
    
    // Check coverage amount.
    if (hit / (hit + miss) >= min_coverage) {
        return true
    }
    return false
}

//Get next element along a line.
ElemWrap.prototype.scan = function(getter, start, end) {
    // Determine the delta increment.
    var delta = end.subtract(start)
    var max = Math.max(Math.abs(delta.x), Math.abs(delta.y))
    delta.divide_elements(max)
    console.log('delta: ' + delta.x + ',' + delta.y)
    // Our candidates.
    var ids_seen = [this.get_id()]
    // Start loop.
    var pos = new Point(start)
    while ((delta.x < 0 && pos.x > end.x) || (delta.x > 0 && pos.x < end.x) ||
            (delta.y < 0 && pos.y > end.y) || (delta.y > 0 && pos.y < end.y)) {
        console.log('+')
        var elem_wrap = g_page_wrap.get_first_elem_wrap_at(getter, pos.x, pos.y)
        if (elem_wrap && (ids_seen.indexOf(elem_wrap.get_id())<0)) {
            ids_seen.push(elem_wrap.get_id())
            if (elem_wrap.is_top_along_ray(getter, pos, delta)) {
                return elem_wrap
            }
        }
        // Increment y.
        pos.increment(delta)
    }
    return null
}

ElemWrap.prototype.shift_vertical = function(getter, start_y, end_y, num_samples = 11) {
    // num_samples = num_segments + 1
    // First sample is on the left edge and the last sample is on the right edge.
    // We scan vertically along all samples.
    var width = this.page_box.get_width()
    var delta = width/(num_samples-1)
    var elem_wraps = []
    var elem_wrap_ids = []
    for (var i=0; i<num_samples; i++) {
        var x = this.page_box.left + i*delta
        var start = new Point(x, start_y)
        var end = new Point(x, end_y)
        var elem_wrap = this.scan(getter, start, end)
        if (elem_wrap && (elem_wrap_ids.indexOf(elem_wrap.get_id()) <0) && (elem_wrap.get_horizontal_overlap(this)>0)) {
            // Sometimes elements will just touch along the left or right edges.
            // The overlap will check above will skip these. We want to have at least some overlap.
            elem_wraps.push(elem_wrap)
            elem_wrap_ids.push(elem_wrap.get_id())
        }
    }
    console.log('num elem wraps: ' + elem_wraps.length)
    
    // Find the edge closest to start point.
    var edges = []
    var edge = null
    if (start_y > end_y) {
        // When searching upwards we're interested in the element's bottom.
        for (var i=0; i<elem_wraps.length; i++) {
            edges.push(elem_wraps[i].page_box.bottom)
        }
        // We want the lowest one.
        edge = Math.max(...edges)
    } else {
        // When searching downwards we're interested in the element's top.
        for (var i=0; i<elem_wraps.length; i++) {
            edges.push(elem_wraps[i].page_box.top)
        }
        // We want the highest one.
        edge = Math.min(...edges)
    }
    
    // Of the elements that are on the edge (within sigma) find the one with most coverage.
    // Find the candidate with the most coverage.
    var max_overlap = 0
    var max_candidate = null
    var sigma = 1.0
    for (var i=0; i<edges.length; i++) {
        if (edges[i]<(edge-sigma) || edges[i] > edge+sigma) {
            continue
        }
        var overlap = elem_wraps[i].get_horizontal_overlap(this)
        console.log('overlap is ' + overlap)
        if (overlap > max_overlap) {
            max_candidate = elem_wraps[i]
            max_overlap = overlap
        }
    }
    if (max_candidate) {
        this.element = max_candidate.element
        console.log('element set to: ' + max_candidate.get_xpath())
        return true
    }
    return false
}

//Generates samples within this element along a line passing through the middle
//of this element. Angle is in radians.
ElemWrap.prototype.generate_starting_samples(angle, num_samples) {
    // Our center.
    var mid = new Point(this.page_box.get_mid_x(),this.page_box.get_mid_y())
    
    // Find the magnitude of a vector from our center to one of the edges of this element.
    var mag = 0
    var c = Math.abs(Math.cos(angle));
    var s = Math.abs(Math.sin(angle));
    var width = this.page_box.get_width()
    var height = this.page_box.get_height()
    if (width*s <= height*c)
    {
        mag= width/2/c;
    }
    else
    {
        mag= height/2/s;
    }

    // The dir along which we take samples.
    var dir = new Point(Math.cos(angle), Math.sin(angle))
    // Our starting point.
    var pos = new Point(mid.x - dir.x*mag, mid.y - dir.y*mag)
    // Our step size.
    var step_size = mag/(num_samples-1)
    var step = new Point(dir)
    step.multiply_elements(step_size)
    // Now generate the sample.
    var samples = []
    for (var i=0; i<num_samples; i++) {
        var p = new Point(pos)
        samples.push(p)
        pos.increment(step)
    }
    return samples
}

// This will shift our element to next matching element according to the getter.
// The samples will be taken along the horizontal or vertical axis of the element,
// depending on which direction is more orthogonal to the scanning dir = (end - start).
ElemWrap.prototype.shift = function(getter, start, end, num_samples = 3) {
    // num_samples = num_segments + 1
    // First sample is on the left edge and the last sample is on the right edge.
    // We scan vertically along all samples.
    var width = this.page_box.get_width()
    var delta = width/(num_samples-1)
    var elem_wraps = []
    var elem_wrap_ids = []
    for (var i=0; i<num_samples; i++) {
        var x = this.page_box.left + i*delta
        var start = new Point(x, start_y)
        var end = new Point(x, end_y)
        var elem_wrap = this.scan(getter, start, end)
        if (elem_wrap && (elem_wrap_ids.indexOf(elem_wrap.get_id()) <0) && (elem_wrap.get_horizontal_overlap(this)>0)) {
            // Sometimes elements will just touch along the left or right edges.
            // The overlap will check above will skip these. We want to have at least some overlap.
            elem_wraps.push(elem_wrap)
            elem_wrap_ids.push(elem_wrap.get_id())
        }
    }
    console.log('num elem wraps: ' + elem_wraps.length)
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

