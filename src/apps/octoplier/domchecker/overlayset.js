
var OverlaySet = function(elem_wraps) {
    this.marked = false
    this.overlays = [] // An array of Overlays
    
    var color_data = g_distinct_colors.obtain_color()
    this.color = color_data.color
    this.color_index = color_data.index
    
    for (var i=0; i<elem_wraps.length; i++) {
        var overlay = new Overlay('smash_browse_selected', this.color, this.color_index, elem_wraps[i])
        this.overlays.push(overlay)
    }
}

// Update all internal state.
OverlaySet.prototype.update = function() {
    for (var i=0; i<this.overlays.length; i++) {
        this.overlays[i].update()
    }
}

OverlaySet.prototype.mark = function() {
    this.marked = true
    for (var i=0; i<this.overlays.length; i++) {
        this.overlays[i].mark()
    }
}

OverlaySet.prototype.unmark = function() {
    this.marked = false
    for (var i=0; i<this.overlays.length; i++) {
        this.overlays[i].unmark()
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
OverlaySet.prototype.expand = function(side, match_critera) {
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
        var overlay = new Overlay('smash_browse_selected', this.color, this.color_index, similar_elem_wraps[i])
        this.overlays.push(overlay)
    }
}

OverlaySet.prototype.get_beams = function(sides) {
    var beams = []
    for (var i=0; i<this.overlays.length; i++) {
        console.log('+')
        for (var j=0; j<sides.length; j++) {
            console.log('*')
            var beam = this.overlays[i].elem_wrap.page_box.get_beam(sides[j])
            console.log('beam: ' + beam.get_as_string())
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
