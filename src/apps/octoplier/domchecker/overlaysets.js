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
OverlaySets.prototype.shift = function(page_x, page_y, dir, wrap_type) {
    var set_index = this.find_set_index(page_x, page_y, null)
    if (set_index < 0) {
        return
    }
    this.sets[set_index].shift(dir, wrap_type)
    this.sets[set_index].update()
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
