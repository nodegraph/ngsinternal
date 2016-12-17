//This class represents a box.
class Box {
    left: number
    right: number
    top: number
    bottom: number
    constructor(box: IBox) {
        this.left = box.left
        this.right = box.right
        this.top = box.top
        this.bottom = box.bottom
    }

    //Reset the box to the origin (top left) with zero width and height.
    reset(): void {
        this.left = 0
        this.right = 0
        this.top = 0
        this.bottom = 0
    }

    get_as_string(): string {
        return this.left + ',' + this.right + ',' + this.top + ',' + this.bottom
    }

    get_area(): number {
        return this.get_width() * this.get_height()
    }

    get_center(): Point {
        return new Point({ x: this.get_mid_x(), y: this.get_mid_y() })
    }

    get_width(): number {
        return this.right - this.left
    }

    get_height(): number {
        return this.bottom - this.top
    }

    get_mid_x(): number {
        return (this.left + this.right) / 2.0
    }

    get_mid_y(): number {
        return (this.top + this.bottom) / 2.0
    }

    get_relative_point(p: Point): Point {
        let rel = new Point({x: 0, y: 0})
        rel.x = p.x - this.left
        rel.y = p.y - this.top
        return rel
    }

    // Our definitions.
    // Local client space: The origin is at the top left of our local frame or window.
    // Global client space: The origin is at the top left of our top window in the browser.
    // Local page space: The origin is at the top left of our local document page, and is agnostic to scroll.
    // Global page space: Is not used, as many parenting frames can be scrolled and gets complicated.

    // Assumes we are in local client space.
    to_global_client_space(local_win: Window): void {
        this.to_global_client_space_helper(local_win, true)
    }
    // Assumes we are in global client space.
    to_local_client_space(local_win: Window): void {
        this.to_global_client_space_helper(local_win, false)
    }
    // Local to global client space transform helper.
    private to_global_client_space_helper(local_win: Window, to: boolean): void {
        let win = local_win
        let factor = 1
        if (!to) {
            factor = -1
        }
        while (win.parent != win) {
            var iframes = win.parent.document.getElementsByTagName('iframe');
            let found = false
            for (let i = 0; i < iframes.length; i++) {
                if (iframes[i].contentWindow === win) {
                    this.left += factor * iframes[i].getBoundingClientRect().left
                    this.right += factor * iframes[i].getBoundingClientRect().left
                    this.top += factor * iframes[i].getBoundingClientRect().top
                    this.bottom += factor * iframes[i].getBoundingClientRect().top
                    found = true
                    break;
                }
            }
            if (!found) {
                console.error('Error box::to_global_client_space did not find parenting iframe')
            }
            win = win.parent
        }
    }

    //Assumes we are in local page space and converts to local client space.
    to_client_space(local_win: Window): void {
        this.left -= local_win.scrollX
        this.right -= local_win.scrollX
        this.top -= local_win.scrollY
        this.bottom -= local_win.scrollY
    }

    // Assumes we are in local client space and convert to local page space.
    to_page_space(local_win: Window): void {
        this.left += local_win.scrollX
        this.right += local_win.scrollX
        this.top += local_win.scrollY
        this.bottom += local_win.scrollY
    }

    //Returns true if and only if we contain the inner entirely.
    contains(inner: IBox): boolean {
        if ((inner.left >= this.left) &&
            (inner.right <= this.right) &&
            (inner.top >= this.top) &&
            (inner.bottom <= this.bottom)
        ) {
            return true
        }
        return false
    }

    //Returns true if and only if we contain the given page point.
    //Due the document.elementsFromPoint() precision on x and y,
    //we need to do a containment test with a sigma of 1.0 by default.
    contains_point(page_pos: Point): boolean {
        const sigma = 1.0
        if ((page_pos.x >= (this.left - sigma)) && (page_pos.x <= (this.right + sigma)) &&
            (page_pos.y >= (this.top - sigma)) && (page_pos.y <= (this.bottom + sigma))) {
            return true
        }
        return false
    }

    //Returns true if and only if we intersect the other page box.
    intersects(other: IBox): boolean {
        if (this.right <= other.left) {
            return false
        } else if (this.left >= other.right) {
            return false
        } else if (this.bottom <= other.top) {
            return false
        } else if (this.top >= other.bottom) {
            return false
        }
        return true
    }

    //Returns true if this page box is on one side of another page box.
    is_oriented_on(side: DirectionType, other: IBox): number {
        switch (side) {
            case DirectionType.left:
                if (this.right < other.right) {
                    return other.right - this.right
                }
                break
            case DirectionType.right:
                if (this.left > other.left) {
                    return this.left - other.left
                }
                break
            case DirectionType.up:
                if (this.bottom < other.bottom) {
                    return other.bottom - this.bottom
                }
                break
            case DirectionType.down:
                if (this.top > other.top) {
                    return this.top - other.top
                }
                break
        }
        return 0
    }

    //Returns a beam to one side of us.
    get_beam(side: DirectionType, page_dim: Box) : Box {
        let beam = new Box(this)
        switch (side) {
            case DirectionType.left:
                beam.left = 0
                break
            case DirectionType.right:
                beam.right = page_dim.get_width()
                break
            case DirectionType.up:
                beam.top = 0
                break
            case DirectionType.down:
                beam.bottom = page_dim.get_height()
                break
            default:
                console.log('Error: get_beam was passed an invalid side argument')
        }
        return beam
    }

    //Returns a side value.
    get_extreme(side: DirectionType) : number {
        switch (side) {
            case DirectionType.left:
                return this.left
            case DirectionType.right:
                return this.right
            case DirectionType.up:
                return this.top
            case DirectionType.down:
                return this.bottom
            default:
                console.log("Error: Box.get_extreme(side) was given an unknown side argument: " + side)
        }
        return 0
    }
}











    


    


    


