
// This class represents the collection of overlay sets being manipulated and constrained
// in the target web page.
// There is currently one global instance of this named g_overlay_sets.

interface MarkedSetInfo {
    marked: number[] 
    unmarked: number[]
}

interface SetOverlayIndex{
    set_index: number,
    overlay_index: number
}

class OverlaySets {
    //Our data is just an array of overlay set's.
    private sets: OverlaySet[]
    
    constructor() {
        this.sets = []
    }

    get_num_sets() {
        return this.sets.length
    }

    get_set(index: number) {
        return this.sets[index]
    }
    
    destroy(): void {
        for (let i = 0; i < this.sets.length; i++) {
            this.destroy_set_by_index(i)
            i -= 1
        }
        this.sets.length = 0
    }

    // Update all internal state.
    update(): void {
        // Update sets and trim invalid ones.
        for (let i = 0; i < this.sets.length; i++) {
            let set = this.sets[i]
            set.update()
            if (!set.is_valid()) {
                set.destroy()
                this.sets.splice(i,1)
                i -= 1
            }
        }
    }

    // Add an overlay set.
    add_set(os: OverlaySet): void {
        this.sets.push(os)
    }

    // Replace an overlay set at specified index.
    replace_set(set_index: number, os: OverlaySet): void {
        this.sets[set_index] = os
    }

    // Finds the set index which has an overlay of the smallest area containing the point.
    // If match_marked_state is true, then it will only consider sets with a marked set equal to the marked state argument.
    find_set_overlay_index(page_pos: Point, match_marked_state: boolean = false, marked_state: boolean = false): SetOverlayIndex {
        let min_overlay_area: number = -1
        let min_soi: SetOverlayIndex = {set_index: -1, overlay_index: -1}
        for (let i = 0; i < this.sets.length; i++) {
            let oset = this.sets[i]
            let overlay_index = oset.find_overlay_index(page_pos)
            if (overlay_index.index >= 0) {
                if (match_marked_state && oset.is_marked() !== marked_state) {
                    continue
                }
                if (min_overlay_area === -1 || overlay_index.area < min_overlay_area) {
                    min_overlay_area = overlay_index.area
                    min_soi.set_index = i
                    min_soi.overlay_index = overlay_index.index
                }
            }
        }
        return min_soi
    }

    // Mark the first unmarked set under the mouse.
    mark_set(set_index: number, mark: boolean): void {
        if (set_index < 0) {
            return
        }
        this.sets[set_index].mark(mark)
    }

    // Unmark all sets.
    unmark_all(): void {
        for (let i = 0; i < this.sets.length; i++) {
            this.sets[i].mark(false)
        }
    }

    // Destroy a set.
    destroy_set(page_pos: Point): void {
        let soi = this.find_set_overlay_index(page_pos)
        this.destroy_set_by_index(soi.set_index)
    }

    // Shift.
    shift(set_index: number, direction: Direction, wrap_type: WrapType, page_wrap: PageWrap): void {
        if (set_index < 0) {
            return
        }
        this.sets[set_index].shift(direction, wrap_type, page_wrap)
        this.sets[set_index].update()
    }

    // Expand.
    expand(set_index: number, direction: Direction, match_criteria: MatchCriteria, page_wrap: PageWrap) {
        if (set_index < 0) {
            return
        }
        this.sets[set_index].expand(direction, match_criteria, page_wrap)
        this.sets[set_index].update()
    }

    get_marked_sets(): MarkedSetInfo {
        let marked: number[] = []
        let unmarked: number[] = []
        for (let i = 0; i < this.sets.length; i++) {
            if (this.sets[i].is_marked()) {
                marked.push(i)
            } else {
                unmarked.push(i)
            }
        }
        return {marked: marked, unmarked: unmarked}
    }

    merge_marked_sets(): void {
        let result = this.get_marked_sets()
        let marked = result.marked
        if (marked.length < 2) {
            return
        }
        // Merge all the marked sets into the first one.
        let set = this.sets[marked[0]]
        for (let i = 1; i < marked.length; i++) {
            let other_set = this.sets[marked[i]]
            set.merge(other_set)
            // Mark sets to destroy by adding a property.
            other_set.cleanup = true
        }
        // Destroy all all the sets marked for destruction.
        for (let i = 0; i < this.sets.length; i++) {
            if (this.sets[i].cleanup) {
                this.destroy_set_by_index(i)
                i -= 1
            }
        }
    }

    // Shrink to marked set.
    //- sides is an array of sides
    shrink_set_to_marked(set_index: number, sides: Direction[]): void {
        // Find the marked and unmarked sets.
        let result = this.get_marked_sets()
        let marked = result.marked
        let unmarked = [set_index]

        // Make sure the set_index is not in the marked set.
        if (marked.indexOf(set_index) >= 0) {
            console.log('Error: attempt to shrink a marked set against other marked sets.')
            return
        }

        // Cumulative list of all the beams on the specified sides of all the marked sets.
        let beams: Box[] = []
        for (let i = 0; i < marked.length; i++) {
            beams = beams.concat(this.sets[marked[i]].get_beams(sides))
        }

        // Loop through the unmarked sets pruning all elements not in any beam.
        for (let i = 0; i < unmarked.length; i++) {
            let set = this.sets[unmarked[i]]
            set.intersect_with_beams(beams)
        }

        // Remove any empty sets.
        for (let i = 0; i < this.sets.length; i++) {
            if (this.sets[i].get_num_overlays() == 0) {
                this.sets.splice(i, 1)
                i -= 1
            }
        }
    }

    // Shrink set to an extreme on a given side.
    shrink_to_extreme(set_index: number, side: Direction): void {
        if (set_index < 0) {
            return
        }
        this.sets[set_index].shrink_to_extreme(side)
    }

    // Get xpath.
    get_xpath(set_index: number, overlay_index: number): string {
        if (set_index < 0 || overlay_index < 0) {
            return
        }
        return this.sets[set_index].get_overlay(overlay_index).get_elem_wrap().get_xpath()
    }

    scroll_down(set_index: number, overlay_index: number): void {
        let scroller = this.sets[set_index].get_overlay(overlay_index).get_elem_wrap().get_closest_scroll(true)
        scroller.scroll_down()
    }

    scroll_up(set_index: number, overlay_index: number): void {
        let scroller = this.sets[set_index].get_overlay(overlay_index).get_elem_wrap().get_closest_scroll(true)
        scroller.scroll_up()
    }

    scroll_right(set_index: number, overlay_index: number): void {
        let scroller = this.sets[set_index].get_overlay(overlay_index).get_elem_wrap().get_closest_scroll(false)
        scroller.scroll_right()
    }

    scroll_left(set_index: number, overlay_index: number): void {
        let scroller = this.sets[set_index].get_overlay(overlay_index).get_elem_wrap().get_closest_scroll(false)
        scroller.scroll_left()
    }

    // -------------------------------------------------------------------------
    // Private methods.
    //-------------------------------------------------------------------------

    // Destroy a set by index.
    destroy_set_by_index(set_index: number): void {
        if (set_index < 0 || set_index >= this.sets.length) {
            return
        }

        this.sets[set_index].destroy()
        // Splice the destroyed set out.
        this.sets.splice(set_index, 1);
    }

}



