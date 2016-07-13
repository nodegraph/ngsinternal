
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
OverlaySet.prototype.contains_page_point = function(page_x, page_y) {
    for (var i=0; i<this.overlays.length; i++) {
        if (this.overlays[i].contains(page_x, page_y)) {
            return true
        }
    }
}

//Shift.
OverlaySet.prototype.shift = function(dir, wrap_type) {
    for (var i=0; i<this.overlays.length; i++) {
        this.overlays[i].elem_wrap.shift(dir, wrap_type)
    }
}

