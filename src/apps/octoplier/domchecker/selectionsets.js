// This class represents the selection sets being manipulated and constrained
// in the target web page.
// There is currently one global selection set named g_selection_sets.

var SelectionSets = function() {
    this.sets = [] // An array of array of overlays
}

SelectionSets.prototype.create_set = function(elements) {
    var page_boxes = this.get_page_boxes(elements)
    var set = this.create_set_from_page_boxes(page_boxes)
    this.sets.push(set)
}

SelectionSets.prototype.update_set = function(set_index, elements) {
    var page_boxes = get_page_boxes(elements)
    var set = this.create_set_from_page_boxes(page_boxes)
    this.sets[set_index] = set
}

SelectionSets.prototype.unmark_all = function () {
    for (var i=0; i<this.sets.length; i++) {
        var set = this.sets[i]
        for (var j=0; j<set.length; j++) {
            set[j].unmark()
        }
    }
}

// Finds the set index under the mouse position.
SelectionSets.prototype.get_set_index = function(page_x, page_y) {
    // When there are multiple we just use the first one.
    for (var i=0; i<this.sets.length; i++) {
        var set = this.sets[i]
        for (var j=0; j<set.length; j++) {
            if (set[j].contains(page_x, page_y)) {
                return i
            }
        }
    }
    return -1
}

SelectionSets.prototype.mark = function(page_x, page_y) {
    // Clear previously marked overlays.
    this.unmark_all()
    
    // Find the set index under the mouse. 
    // When there are multiple we just use the first one.
    var set_index = this.get_set_index(page_x, page_y)
    
    // Now mark the overlays in the set.
    if (set_index >= 0) {
        this.unmark_all()
        var set = this.sets[set_index]
        for (var j=0; j<set.length; j++) {
            set[j].mark()
        }
    }
}

SelectionSets.prototype.destroy_set = function(page_x, page_y) {
    // Find the set index under the mouse. 
    // When there are multiple we just use the first one.
    var set_index = this.get_set_index(page_x, page_y)

    // Now mark the overlays in the set.
    this.destroy_set_by_index(set_index)
}

// Private methods.

SelectionSets.prototype.destroy_set_by_index = function(set_index) {
    var set = this.sets[set_index]
    // Release the color.
    g_distinct_colors.release_color(set[0].color)
    // Destroy the dom elements.
    for (var i=0; i<set.length; i++) {
        set[i].destroy()
    }
    // Splice the destroyed set out.
    this.sets.splice(set_index, 1);
}

SelectionSets.prototype.get_page_boxes = function(elements) {
    var page_boxes = []
    for (var i=0; i<elements.length; i++) {
        var element = elements[i]
        var rect = element.getBoundingClientRect()
        page_boxes.push(get_page_box_from_client_rect(rect))
    }
    return page_boxes
}

SelectionSets.prototype.create_set_from_page_boxes = function(page_boxes) {
    var set = []
    var color_index = g_distinct_colors.obtain_color()
    var color = color_index.color
    var enlarge = color_index.index
    
    for (var i=0; i<page_boxes.length; i++) {
        console.log('creating overlay')
        var page_box = page_boxes[i]
        var overlay = new Overlay('smash_browse_selected', page_box, color, enlarge)
        set.push(overlay)
    }
    return set
}

var g_selection_sets = new SelectionSets()
