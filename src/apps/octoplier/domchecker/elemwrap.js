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

//Scan a line from start in direction ndir, looking for the next element using the specified getter.
//- start is the start point of the ray
//- ndir is the normalized direction of the ray
//- a hit is found when a top element is found. A top element has no other elements on top of it.
//  so start should general generally start off of any top element you're starting to scan from.
ElemWrap.prototype.ray_cast = function(getter, start, ndir, scan_speed = 1.0) {
    // Determine our delta.
    var delta = new Point(ndir)
    delta.multiply_elements(scan_speed)
    
    // This makes sure the hit is not the same as us.
    var ids_seen = [this.get_id()]
    
    // Start loop.
    var page_bounds = g_page_wrap.get_bounds()
    var pos = new Point(start)
    while (page_bounds.contains_point(pos.x, pos.y)) {
        var elem_wrap = g_page_wrap.get_first_elem_wrap_at(getter, pos.x, pos.y)
        // Make sure the hit element is not ourself, and that it doesn't contain us.
        if (elem_wrap && (ids_seen.indexOf(elem_wrap.get_id())<0) && (!elem_wrap.contains(this)) ) {
            ids_seen.push(elem_wrap.get_id())
            if (elem_wrap.is_topmost(getter)) {
                return {ray_hit: elem_wrap, ray_start: start, ray_end: pos}
            }
        }
        // Increment y.
        pos.increment(delta)
    }
    return {ray_hit: null, ray_start: null, ray_end: null}
}

//Generates samples within this element along a line passing through the middle
//of this element. num_samples_xy is point with the number of x and y samples.
ElemWrap.prototype.generate_starting_samples = function(ndir, num_samples_xy) {
    var samples = []
    if (ndir.y != 0) {
        // Generate horizontal samples.
        var delta = new Point(this.page_box.get_width() / (num_samples_xy.x -1), 0)
        var pos = null
        if (ndir.y > 0) {
            // scanning down
            pos = new Point(this.page_box.left, this.page_box.bottom)
        } else {
            // scanning up
            pos = new Point(this.page_box.left, this.page_box.top)
        }
        for (var i=0; i<num_samples_xy.x; i++) {
            samples.push(new Point(pos))
            pos.increment(delta)
        }
    }
    if (ndir.x != 0) {
        // Generate vertical samples.
        var delta = new Point(0, this.page_box.get_height() / (num_samples_xy.y -1))
        var pos = null
        if (ndir.x > 0) {
            // scanning right
            pos = new Point(this.page_box.right, this.page_box.top)
        } else {
            // scanning left
            pos = new Point(this.page_box.left, this.page_box.top)
        }
        for (var i=0; i<num_samples_xy.y; i++) {
            samples.push(new Point(pos))
            pos.increment(delta)
        }
    }
    // Note: the corner where the x and y samples meet is duplicated.
    // This shouldn't cause any real inaccuracies except for a tiny bit of extra work.
    return samples
}

// This will shift our element to next matching element according to the getter.
// The samples will be taken along the horizontal or vertical axis of the element,
// depending on which direction is more orthogonal to the scanning dir = (end - start).
ElemWrap.prototype.shift_along_dir = function(getter, ndir, num_samples_xy) {
    // Generate staring samples.
    var samples = this.generate_starting_samples(ndir, num_samples_xy)
    //console.log('num samples: ' + samples.length)
    
    // Scan from samples.
    var elem_wraps = []
    var elem_wrap_ids = []
    var distances = []
    for (var i=0; i<samples.length; i++) {
        //console.log('sample['+i+']: '+samples[i].x+','+samples[i].y)
        var result = this.ray_cast(getter, samples[i], ndir)
        var elem_wrap = result.ray_hit
        if (elem_wrap) { //&& (elem_wrap.get_horizontal_overlap(this)>0)
            // We check to make sure the dist is not zero.
            // A dist of zero means that the element we're searching from is 
            // touching or overlapping the element that got hit.
            var dist = (result.ray_end.subtract(result.ray_start)).get_magnitude()
            if (dist!=0) {
                elem_wraps.push(elem_wrap)
                elem_wrap_ids.push(elem_wrap.get_id())
                distances.push(dist)
            }
        }
    }
    //console.log('num candidates: ' + elem_wraps.length)
    
    // Find the minimum distance of the elems.
    var min_dist = Math.min(...distances)
    
    // First find all the ids around this min distance, within sigma.
    var sigma = 1.0
    var min_ids = []
    for (var i=0; i<distances.length; i++) {
        if (distances[i]<(min_dist-sigma) || distances[i] > min_dist+sigma) {
            continue
        }
        if (min_ids.indexOf(elem_wrap_ids[i])<0) {
            min_ids.push(elem_wrap_ids[i])
        }
    }
    
    // Now find the one with the most hits.
    var max_hits = 0
    var max_element = null
    for (var i=0; i<min_ids.length; i++) {
        var num_hits = 0
        for (var j=0; j<elem_wrap_ids.length; j++) {
            if (elem_wrap_ids[j] == min_ids[i]) {
                num_hits += 1
            }
        } 
        if (num_hits > max_hits) {
            max_hits = num_hits
            max_element = min_ids[i]
        }
    }
    if (max_element) {
        this.element = max_element
        console.log('element shifted to: ' + this.get_xpath())
        return true
    }
    return false
}

ElemWrap.prototype.shift = function(dir, wrap_type) {
    //Number of ray cast string samples to take along the horizontal or vertical axis of this element.
    var dim_samples = 11
    
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
    
    // Scan in one of the 4 directions.
    switch(dir) {
        case this.direction.left:
            this.shift_along_dir(getter, new Point(-1,0), new Point(0,dim_samples)) 
            break
        case this.direction.right: 
            this.shift_along_dir(getter, new Point(1,0), new Point(0,dim_samples)) 
            break
        case this.direction.up: 
            this.shift_along_dir(getter, new Point(0,-1), new Point(dim_samples,0)) 
            break
        case this.direction.down: 
            this.shift_along_dir(getter, new Point(0,1), new Point(dim_samples,0)) 
            break
    }
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

