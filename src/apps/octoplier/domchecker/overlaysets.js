// This class represents the collection of overlay sets being manipulated and constrained
// in the target web page.
// There is currently one global instance of this named g_overlay_sets.

//Our data is just an array of overlay set's.
var OverlaySets = function() {
    this.sets = [] // An array of OverlaySet's.
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
OverlaySets.prototype.mark = function(page_x, page_y) {
    var set_index = this.find_set_index(page_x, page_y, false)
    if (set_index < 0) {
        return
    }
    this.sets[set_index].mark()
}

//Unmark the first marked set under the mouse.
OverlaySets.prototype.unmark = function(page_x, page_y) {
    var set_index = this.find_set_index(page_x, page_y, true)
    if (set_index < 0) {
        return
    }
    this.sets[set_index].unmark()
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
    if (set_index < 0) {
        return
    }
    this.destroy_set_by_index(set_index)
}

//Shift.
OverlaySets.prototype.shift = function(page_x, page_y, side, wrap_type) {
    var set_index = this.find_set_index(page_x, page_y, null)
    if (set_index < 0) {
        return
    }
    this.sets[set_index].shift(side, wrap_type)
    this.sets[set_index].update()
}

// Expand.
OverlaySets.prototype.expand = function(page_x, page_y, side, match_criteria) {
    var set_index = this.find_set_index(page_x, page_y, null)
    if (set_index < 0) {
        return
    }
    this.sets[set_index].expand(side, match_criteria)
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

// Shrink to marked set.
//- sides is an array of sides
OverlaySets.prototype.shrink_to_marked = function(page_x, page_y, sides) {
    // Find the marked and unmarked sets.
    var result = this.get_marked_sets()
    var marked = result.marked
    var unmarked = result.unmarked
    
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

// Shrink all sets.
OverlaySets.prototype.shrink_to_extreme = function(page_x, page_y, side) {
    // Shrink down each overlay set.
    for (var i=0; i<this.sets.length; i++) {
        this.sets[i].shrink_to_extreme(side)
    }
    
    // Now find the most extreme of the shrunken sets.
    var extreme = null
    var extreme_index = null
    for (var i=0; i<this.sets.length; i++) {
        var set = this.sets[i]
        var page_box = set.overlays[0].elem_wrap.page_box
        if (i==0) {
            extreme = page_box.get_extreme(side)
            extreme_index = 0
        } else {
            var value = page_box.get_extreme(side)
            if (value_is_more_extreme(value,extreme,side)) {
                extreme = value
                extreme_index = i
            }
        }
    }
    
    // Create our next sets array, which contains all the sets at the extreme value.
    var next_sets = []
    for (var i=0; i<this.sets.length; i++) {
        var set = this.sets[i]
        var page_box = set.overlays[0].elem_wrap.page_box
        var value = page_box.get_extreme(side)
        if (value == extreme) {
            // Add to our next sets.
            next_sets.push(this.sets[i])
            // Splice out from our current sets
            this.sets.splice(i,1)
            i -= 1
        }
    }
    
    // Destroy all the other sets.
    for (var i=0; i<this.sets.length; i++) {
        this.sets[i].destroy()
    }
    
    // Update our sets array.
    this.sets = next_sets
}

// -------------------------------------------------------------------------
// Private methods.
//-------------------------------------------------------------------------

//Destroy a set by index.
OverlaySets.prototype.destroy_set_by_index = function(set_index) {
    this.sets[set_index].destroy()
    // Splice the destroyed set out.
    this.sets.splice(set_index, 1);
}

//Global instance.
var g_overlay_sets = new OverlaySets()
