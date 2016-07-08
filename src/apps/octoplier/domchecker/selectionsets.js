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
    //this.update_colors(this.sets.length-1)
}

SelectionSets.prototype.update_set = function(set_index, elements) {
    var page_boxes = get_page_boxes(elements)
    var set = this.create_set_from_page_boxes(page_boxes)
    this.sets[set_index] = set
    this.update_colors(set_index)
}

SelectionSets.prototype.destroy_set = function(set_index) {
    var set = this.sets[index]
    for (var i=0; i<set.length; i++) {
        set[i].destroy()
    }
    this.sets.splice(index, 1);
    this.update_all_colors();
}

SelectionSets.prototype.update_colors = function(set_index) {
    var set = this.sets[set_index]
    for (var j=0; j<set.length; j++) {
        set[j].update_color(set_index)
    }
}

SelectionSets.prototype.update_all_colors = function() {
    for (var i=0; i<this.sets.length; i++) {
        this.update_colors(set_index)
    }
}

// Private methods.

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
    for (var i=0; i<page_boxes.length; i++) {
        console.log('creating overlay')
        var page_box = page_boxes[i]
        var overlay = new Overlay('smash_browse_text_box', page_box)
        set.push(overlay)
    }
    return set
}

var g_selection_sets = new SelectionSets()
