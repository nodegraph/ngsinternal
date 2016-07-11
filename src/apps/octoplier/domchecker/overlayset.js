
var OverlaySet = function(element_wrappers) {
    this.marked = false
    this.overlays = [] // An array of Overlays
    
    var color_data = g_distinct_colors.obtain_color()
    this.color = color_data.color
    this.color_index = color_data.index
    
    for (var i=0; i<element_wrappers.length; i++) {
        var overlay = new Overlay('smash_browse_selected', this.color, this.color_index, element_wrappers[i])
        this.overlays.push(overlay)
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

