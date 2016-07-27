// This class represents the collection of overlay sets being manipulated and constrained
// in the target web page.
// There is currently one global instance of this named g_overlay_sets.

//Our data is just an array of overlay set's.
var OverlaySets = function() {
    this.sets = [] // An array of OverlaySet's.
}

OverlaySets.prototype.destroy = function() {
    for (var i=0; i<this.sets.length; i++) {
        this.destroy_set_by_index(i)
    }
    this.sets.length = 0
}

//Update all internal state.
OverlaySets.prototype.update = function() {
    for (var i=0; i<this.sets.length; i++) {
        this.sets[i].update()
    }
}

//Add an overlay set.
OverlaySets.prototype.add_set = function(set) {
    this.sets.push(set)
}

//Replace an overlay set at specified index.
OverlaySets.prototype.replace_set = function(set_index, set) {
    this.sets[set_index] = set
}

//Finds the first set index under the mouse matching the marked state.
//If marked is null, then the marked state will not be considered.
OverlaySets.prototype.find_set_index = function(page_x, page_y, marked=null) {
    // When there are multiple we get the first one which is not already marked.
    for (var i=0; i<this.sets.length; i++) {
        var set = this.sets[i]
        if (set.contains_point(page_x, page_y)) {
            if (marked === null) {
                return i
            }
            if (set.marked === marked) {
                return i
            }
        }
    }
    return -1
}

//Mark the first unmarked set under the mouse.
OverlaySets.prototype.mark_set = function(set_index, mark) {
    if (set_index < 0) {
        return
    }
    this.sets[set_index].mark(mark)
}

//Unmark all sets.
OverlaySets.prototype.unmark_all = function () {
    for (var i=0; i<this.sets.length; i++) {
        this.sets[i].unmark()
    }
}

//Destroy a set.
OverlaySets.prototype.destroy_set = function(page_x, page_y) {
    var set_index = this.find_set_index(page_x, page_y, null)
    this.destroy_set_by_index(set_index)
}

//Shift.
OverlaySets.prototype.shift = function(set_index, direction, wrap_type) {
    if (set_index < 0) {
        return
    }
    this.sets[set_index].shift(direction, wrap_type)
    this.sets[set_index].update()
}

// Expand.
OverlaySets.prototype.expand = function(set_index, direction, match_criteria) {
    if (set_index < 0) {
        return
    }
    this.sets[set_index].expand(direction, match_criteria)
    this.sets[set_index].update()
}

OverlaySets.prototype.get_marked_sets = function() {
    var marked = []
    var unmarked = []
    for (var i=0; i<this.sets.length; i++) {
        if (this.sets[i].marked) {
            marked.push(i)
        } else {
            unmarked .push(i)
        }
    }
    return {marked: marked, unmarked: unmarked}
}

OverlaySets.prototype.merge_marked_sets = function() {
    var result = this.get_marked_sets()
    var marked = result.marked
    if (marked.length < 2) {
        return
    }
    // Merge all the marked sets into the first one.
    var set = this.sets[marked[0]]
    for (var i=1; i<marked.length; i++) {
        var other_set = this.sets[marked[i]]
        set.merge(other_set)
        // Mark sets to destroy by adding a property.
        other_set.cleanup = true
    }
    // Destroy all all the sets marked for destruction.
    for (var i=0; i<this.sets.length; i++) {
        if (this.sets[i].cleanup) {
            this.sets[i].destroy()
            this.sets.splice(i,1)
            i -= 1
        }
    }
}

// Shrink to marked set.
//- sides is an array of sides
OverlaySets.prototype.shrink_set_to_marked = function(set_index, sides) {
    // Find the marked and unmarked sets.
    var result = this.get_marked_sets()
    var marked = result.marked
    var unmarked = [set_index]
    
    // Make sure the set_index is not in the marked set.
    if (marked.indexOf(set_index) >= 0) {
        console.log('Error: attempt to shrink a marked set against other marked sets.')
        return
    }
    
    // Cumulative list of all the beams on the specified sides of all the marked sets.
    var beams = []
    for (var i=0; i<marked.length; i++) {
        beams = beams.concat(this.sets[marked[i]].get_beams(sides))
    }
    
    // Loop through the unmarked sets pruning all elements not in any beam.
    for (var i=0; i<unmarked.length; i++) {
        var set = this.sets[unmarked[i]]
        set.intersect_with_beams(beams)
    }
    
    // Remove any empty sets.
    for (var i=0; i<this.sets.length; i++) {
        if (this.sets[i].overlays.length == 0) {
            this.sets.splice(i,1)
            i -= 1
        }
    }
}

// Shrink set to an extreme on a given side.
OverlaySets.prototype.shrink_to_extreme = function(set_index, side) {
    if (set_index < 0) {
        return
    }
    this.sets[set_index].shrink_to_extreme(side)
}

// Get xpath.
OverlaySets.prototype.get_xpath = function(set_index, overlay_index) {
    if (set_index < 0 || overlay_index < 0) {
        return
    }
    return this.sets[set_index].overlays[overlay_index].elem_wrap.get_xpath()
}

OverlaySets.prototype.scroll_down = function(set_index, overlay_index) {
    var scroller = this.sets[set_index].overlays[overlay_index].elem_wrap.get_closest_scroll(true)
    scroller.scroll_down()
}

OverlaySets.prototype.scroll_up = function(set_index, overlay_index) {
    var scroller = this.sets[set_index].overlays[overlay_index].elem_wrap.get_closest_scroll(true)
    scroller.scroll_up()
}

OverlaySets.prototype.scroll_right = function(set_index, overlay_index) {
    var scroller = this.sets[set_index].overlays[overlay_index].elem_wrap.get_closest_scroll(false)
    scroller.scroll_right()
}

OverlaySets.prototype.scroll_left = function(set_index, overlay_index) {
    var scroller = this.sets[set_index].overlays[overlay_index].elem_wrap.get_closest_scroll(false)
    scroller.scroll_left()
}

// -------------------------------------------------------------------------
// Private methods.
//-------------------------------------------------------------------------

//Destroy a set by index.
OverlaySets.prototype.destroy_set_by_index = function(set_index) {
    if (set_index < 0) {
        return
    }
    
    this.sets[set_index].destroy()
    // Splice the destroyed set out.
    this.sets.splice(set_index, 1);
}

//Global instance.
var g_overlay_sets = new OverlaySets()
