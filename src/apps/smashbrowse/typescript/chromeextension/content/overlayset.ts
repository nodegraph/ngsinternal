
class OverlaySet {
    distinct_colors: DistinctColors
    color: string
    color_index: number
    marked: boolean
    overlays: Overlay[]
    
    // Hack temp usage:
    cleanup: boolean
    
    constructor(elem_wraps: ElemWrap[], distinct_colors: DistinctColors) {
        // Our members.
        this.distinct_colors = distinct_colors
        this.marked = false
        let color_data = this.distinct_colors.obtain_color()
        this.color = color_data.color
        this.color_index = color_data.index
        this.overlays = []
        this.cleanup = false
        //console.log('num elem wraps in set: ' + elem_wraps.length)
        for (let i = 0; i < elem_wraps.length; i++) {
            //console.log('adding elem: ' + elem_wraps[i].get_xpath())
            let overlay = new Overlay('smash_browse_selected', this.color, this.color_index, this.marked, elem_wraps[i])
            this.overlays.push(overlay)
        }
    }
    
    // Update all internal state.
    update(): void {
        for (let i = 0; i < this.overlays.length; i++) {
            this.overlays[i].update(this.color, this.color_index)
        }
    }

    mark(mark: boolean): void {
        this.marked = mark
        for (let i = 0; i < this.overlays.length; i++) {
            this.overlays[i].mark(this.marked)
        }
    }

    destroy(): void {
        // Release the color.
        this.distinct_colors.release_color(this.color)
        // Destroy the dom elements.
        for (let i = 0; i < this.overlays.length; i++) {
            this.overlays[i].destroy()
        }
    }

    //Finds the set index under the mouse position.
    contains_point(page_pos: Point): boolean {
        for (let i = 0; i < this.overlays.length; i++) {
            if (this.overlays[i].contains_point(page_pos)) {
                return true
            }
        }
        return false
    }

    find_overlay_index(page_pos: Point): number {
        for (let i = 0; i < this.overlays.length; i++) {
            let overlay = this.overlays[i]
            if (overlay.contains_point(page_pos)) {
                    return i
            }
        }
        return -1
    }

    //Shift.
    shift(side: Direction, wrap_type: WrapType, page_wrap: PageWrap): void {
        for (let i = 0; i < this.overlays.length; i++) {
            this.overlays[i].elem_wrap.shift(side, wrap_type, page_wrap)
        }
    }

    static find_element(elem_wraps: ElemWrap[], element: HTMLElement): number {
        for(var i=0; i<elem_wraps.length; i++) {
            if(elem_wraps[i].element === element) {
                return i;
            }
        }
        return -1
    }

    //Expand
    expand(side: Direction, match_criteria: MatchCriteria, page_wrap: PageWrap): void {
        // Cache the elem_wraps already in this set.
        let existing_elem_wraps: ElemWrap[] = []
        for (let i = 0; i < this.overlays.length; i++) {
            existing_elem_wraps.push(this.overlays[i].elem_wrap)
        }

        // Loop through each elem wrap, looking for neighbors.
        let similar_elem_wraps: ElemWrap[] = []
        for (let i = 0; i < this.overlays.length; i++) {
            let neighbors = this.overlays[i].elem_wrap.get_similar_neighbors(side, match_criteria, page_wrap)
            for (let j = 0; j < neighbors.length; j++) {
                // If the neighbor isn't already in the existing set or the similar set, then add it.
                if ((OverlaySet.find_element(similar_elem_wraps, neighbors[j].element) < 0) &&
                    (OverlaySet.find_element(existing_elem_wraps, neighbors[j].element) < 0)) {
                    similar_elem_wraps.push(neighbors[j])
                }
            }
        }

        for (let i = 0; i < similar_elem_wraps.length; i++) {
            let overlay = new Overlay('smash_browse_selected', this.color, this.color_index, this.marked, similar_elem_wraps[i])
            this.overlays.push(overlay)
        }
    }

    //Merge another sets elements into ourself.
    merge(other: OverlaySet): void {
        for (let i = 0; i < other.overlays.length; i++) {
            let elem_wrap = other.overlays[i].elem_wrap
            let overlay = new Overlay('smash_browse_selected', this.color, this.color_index, this.marked, elem_wrap)
            overlay.mark(this.marked)
            this.overlays.push(overlay)
        }
    }

    get_beams(sides: Direction[]): Box[] {
        let beams: Box[] = []
        for (let i = 0; i < this.overlays.length; i++) {
            for (let j = 0; j < sides.length; j++) {
                let ew: ElemWrap = this.overlays[i].elem_wrap
                let beam: Box = ew.page_box.get_beam(sides[j], PageWrap.get_bounds())
                beams.push(beam)
            }
        }
        return beams
    }

    intersect_with_beams(beams: Box[]): void {
        for (let i = 0; i < this.overlays.length; i++) {
            let intersects = false
            for (let j = 0; j < beams.length; j++) {
                if (beams[j].intersects(this.overlays[i].elem_wrap.page_box)) {
                    intersects = true
                    break
                }
            }
            if (!intersects) {
                this.overlays[i].destroy()
                this.overlays.splice(i, 1)
                i -= 1
            }
        }
    }

    shrink_to_extreme(side: Direction): void {
        let extreme: number
        let extreme_index: number
        for (let i = 0; i < this.overlays.length; i++) {
            let overlay = this.overlays[i]
            let page_box = overlay.elem_wrap.page_box
            if (i == 0) {
                // Initialize extreme values.
                extreme = page_box.get_extreme(side)
                extreme_index = 0
            } else {
                // Update extreme values.
                let value = page_box.get_extreme(side)
                if (Utils.value_is_more_extreme(value, extreme, side)) {
                    extreme = value
                    extreme_index = i
                }
            }
        }

        // Create our next overlays array, which contains all the overlays at the extreme value.
        let next_overlays: Overlay[] = []
        for (let i = 0; i < this.overlays.length; i++) {
            let overlay = this.overlays[i]
            let page_box = overlay.elem_wrap.page_box
            let value = page_box.get_extreme(side)
            if (value == extreme) {
                // Add to next overlays.
                next_overlays.push(overlay)
                // Splice our from current overlays.
                this.overlays.splice(i, 1)
                i -= 1
            }
        }

        // Destroy all the other overlays.
        for (let i = 0; i < this.overlays.length; i++) {
            this.overlays[i].destroy()
        }

        // Update our overlays array.
        this.overlays = next_overlays
    }

}



