
var OverlaySet = function(elem_wraps, color=null, marked=false) {
    // Our members.
    this.color = null
    this.color_index = -1
    this.marked = marked
    this.overlays = [] // An array of Overlays
    
    if (color == null) {
        var color_data = g_distinct_colors.obtain_color()
        this.color = color_data.color
        this.color_index = color_data.index
    } else {
        this.color = color
        this.color_index = g_distinct_colors.request_color(this.color)
        if (this.color_index < 0) {
            console.log("Error in OverlaySet: requesting color: " +this.color)
        }
    }
    
    console.log('num elem wraps in set: ' + elem_wraps.length)
    for (var i=0; i<elem_wraps.length; i++) {
        console.log('adding elem: ' + elem_wraps[i].get_xpath())
        var overlay = new Overlay('smash_browse_selected', this.color, this.color_index, this.marked, elem_wraps[i])
        this.overlays.push(overlay)
    }
}

//Serialize to a JSON object.
OverlaySet.prototype.serializeToJsonObj = function() {
    var obj = {}
    obj.color = this.color
    obj.marked = this.marked
    obj.overlays = []
    
    for (var i=0; i<this.overlays.length; i++) {
        var overlay = this.overlays[i]
        obj.overlays.push(overlay.serializeToJsonObj())
    }
    return obj
}

// Update all internal state.
OverlaySet.prototype.update = function() {
    console.log('overlay set update with color index: ' + this.color_index)
    for (var i=0; i<this.overlays.length; i++) {
        this.overlays[i].update(this.color, this.color_index, this.marked)
    }
}

OverlaySet.prototype.mark = function(mark) {
    this.marked = mark
    for (var i=0; i<this.overlays.length; i++) {
        this.overlays[i].mark(this.marked)
    }
}

OverlaySet.prototype.destroy = function() {
    // Release the color.
    g_distinct_colors.release_color(this.color)
    // Destroy the dom elements.
    for (var i=0; i<this.overlays.length; i++) {
        this.overlays[i].destroy()
    }
}

//Finds the set index under the mouse position.
OverlaySet.prototype.contains_point = function(page_x, page_y) {
    for (var i=0; i<this.overlays.length; i++) {
        if (this.overlays[i].contains_point(page_x, page_y)) {
            return true
        }
    }
}

//Shift.
OverlaySet.prototype.shift = function(side, wrap_type) {
    for (var i=0; i<this.overlays.length; i++) {
        this.overlays[i].elem_wrap.shift(side, wrap_type)
    }
}

//Expand
OverlaySet.prototype.expand = function(side, match_criteria) {
    // Cache the elem_wraps already in this set.
    var existing_elem_wraps = []
    for (var i=0; i<this.overlays.length; i++) {
        existing_elem_wraps.push(this.overlays[i].elem_wrap)
    }
    
    // Loop through each elem wrap, looking for neighbors.
    var similar_elem_wraps = []
    for (var i=0; i<this.overlays.length; i++) {
        var neighbors = this.overlays[i].elem_wrap.get_similar_neighbors(side, match_criteria)
        for (var j=0; j<neighbors.length; j++) {
            // If the neighbor isn't already in the existing set or the similar set, then add it.
            if ( (find_in_array_by_property(similar_elem_wraps,'element',neighbors[j].element) < 0) &&
                    (find_in_array_by_property(existing_elem_wraps,'element',neighbors[j].element) < 0)){
                similar_elem_wraps.push(neighbors[j])
            }
        }
    }
    
    for (var i=0; i<similar_elem_wraps.length; i++) {
        var overlay = new Overlay('smash_browse_selected', this.color, this.color_index, this.marked, similar_elem_wraps[i])
        this.overlays.push(overlay)
    }
}

//Merge another sets elements into ourself.
OverlaySet.prototype.merge = function(other) {
    for (var i=0; i<other.overlays.length; i++) {
        var elem_wrap = other.overlays[i].elem_wrap
        var overlay = new Overlay('smash_browse_selected', this.color, this.color_index, this.marked, elem_wrap)
        if (this.marked) {
            overlay.mark()
        }
        this.overlays.push(overlay)
    }
}

OverlaySet.prototype.get_beams = function(sides) {
    var beams = []
    for (var i=0; i<this.overlays.length; i++) {
        for (var j=0; j<sides.length; j++) {
            var beam = this.overlays[i].elem_wrap.page_box.get_beam(sides[j])
            beams.push(beam)
        }
    }
    return beams
}

OverlaySet.prototype.intersect_with_beams = function(beams) {
    for (var i=0; i<this.overlays.length; i++) {
        var intersects = false
        for (var j=0; j<beams.length; j++) {
            if (beams[j].intersects(this.overlays[i].elem_wrap.page_box)) {
                intersects = true
                break
            }
        }
        if (!intersects) {
            this.overlays[i].destroy()
            this.overlays.splice(i,1)
            i -= 1
        }
    }
}

//OverlaySet.prototype.trim_to_side = function(beams, side) {
//    for (var b=0; b<beams.length; b++) {
//        var page_box = beams[b]
//        for (var i=0; i<this.overlays.length; i++) {
//            var overlay = this.overlays[i]
//            var out_of_bounds = false
//            switch(side) {
//                case ElemWrap.prototype.direction.left:
//                    if (overlay.right > page_box.right) {
//                        out_of_bounds = true
//                    }
//                    break
//                case ElemWrap.prototype.direction.right:
//                    if (overlay.left < page_box.left) {
//                        out_of_bounds = true
//                    }
//                    break
//                case ElemWrap.prototype.direction.up:
//                    if (overlay.bottom > page_box.bottom) {
//                        out_of_bounds = true
//                    }
//                    break
//                case ElemWrap.prototype.direction.down:
//                    if (overlay.top < page_box.top) {
//                        out_of_bounds = true
//                    }
//                    break
//            }
//            // Remove an out of bounds overlay.
//            if (out_of_bounds) {
//                this.overlays[i].destroy()
//                this.overlays.splice(i,1)
//                i -= 1
//            }
//        }
//    }
//}

OverlaySet.prototype.shrink_to_extreme = function(side) {
    var extreme = null
    var extreme_index = null
    for (var i=0; i<this.overlays.length; i++) {
        var overlay = this.overlays[i]
        var page_box = overlay.elem_wrap.page_box
        if (i == 0) {
            // Initialize extreme values.
            extreme = page_box.get_extreme(side)
            extreme_index = 0
        } else {
            // Update extreme values.
            var value = page_box.get_extreme(side)
            if (value_is_more_extreme(value, extreme,side)) {
                extreme = value
                extreme_index = i
            }
        }
    }
    
    // Create our next overlays array, which contains all the overlays at the extreme value.
    var next_overlays = []
    for (var i=0; i<this.overlays.length; i++) {
        var overlay = this.overlays[i]
        var page_box = overlay.elem_wrap.page_box
        var value = page_box.get_extreme(side)
        if (value == extreme) {
            // Add to next overlays.
            next_overlays.push(overlay)
            // Splice our from current overlays.
            this.overlays.splice(i,1)
            i -= 1
        }
    }
    
    // Destroy all the other overlays.
    for (var i=0; i<this.overlays.length; i++) {
        this.overlays[i].destroy()
    }
    
    // Update our overlays array.
    this.overlays = next_overlays
}


