
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
