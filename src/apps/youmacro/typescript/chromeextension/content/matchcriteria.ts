
interface MatchCriteriaInterface {
    // Border matching.
    match_left: boolean
    match_right: boolean
    match_top: boolean
    match_bottom: boolean
    // Font matching.
    match_font: boolean
    match_font_size: boolean
}

class MatchCriteria {
    // Border matching.
    match_left: boolean
    match_right: boolean
    match_top: boolean
    match_bottom: boolean
    // Font matching.
    match_font: boolean
    match_font_size: boolean
    
    constructor(obj: MatchCriteriaInterface = {
        match_left: false,
        match_right: false,
        match_top: false,
        match_bottom: false,
        match_font: false,
        match_font_size: false
    }) {
        // Border matching.
        this.match_left = obj.match_left
        this.match_right = obj.match_right
        this.match_top = obj.match_top
        this.match_bottom = obj.match_bottom
        // Font matching.
        this.match_font = obj.match_font
        this.match_font_size = obj.match_font_size
    }
    
    //Returns true if the two elem wraps match, according to the match criteria.
    matches(a: ElemWrap, b: ElemWrap): boolean {

        if (this.match_left && (a.get_box().left != b.get_box().left)) {
            return false
        }

        if (this.match_right && (a.get_box().right != b.get_box().right)) {
            return false
        }

        if (this.match_top && (a.get_box().top != b.get_box().top)) {
            return false
        }

        if (this.match_bottom && (a.get_box().bottom != b.get_box().bottom)) {
            return false
        }

        // Match font style.
        if (this.match_font || this.match_font_size) {
            var style_a = window.getComputedStyle(a.get_element(), null)
            var style_b = window.getComputedStyle(b.get_element(), null)

            if (this.match_font && (style_a.font != style_b.font)) {
                return false
            }
            if (this.match_font_size && (style_a.fontSize != style_b.fontSize)) {
                return false
            }
        }
        return true
    }
}

