import {Point} from "./point"

// Direction.
export const enum Direction {
    left,
    right,
    up,
    down
}

//This interface represents the data shape for box.
export interface BoxInterface {
    left: number
    right: number
    top: number
    bottom: number
}

//This class represents a box.
export class Box {
    left: number
    right: number
    top: number
    bottom: number
    constructor(box: BoxInterface = {left: 0, right:0, top:0, bottom:0}) {
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

    //Assumes we are in page space and converts to client space.
    to_client_space(): void {
        this.left -= window.scrollX
        this.right -= window.scrollX
        this.top -= window.scrollY
        this.top -= window.scrollY
    }

    // Assumes we are in client space and convert to page space.
    to_page_space(): void {
        this.left += window.scrollX
        this.right += window.scrollX
        this.top += window.scrollY
        this.bottom += window.scrollY
    }

    //Returns true if and only if we contain the inner entirely.
    contains(inner: BoxInterface): boolean {
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
    contains_point(page_pos: Point, sigma = 1.0): boolean {
        if ((page_pos.x >= (this.left - sigma)) && (page_pos.x <= (this.right + sigma)) &&
            (page_pos.y >= (this.top - sigma)) && (page_pos.y <= (this.bottom + sigma))) {
            return true
        }
        return false
    }

    //Returns true if and only if we intersect the other page box.
    intersects(other: BoxInterface): boolean {
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
    is_oriented_on(side: Direction, other: BoxInterface): number {
        switch (side) {
            case Direction.left:
                if (this.right < other.right) {
                    return other.right - this.right
                }
                break
            case Direction.right:
                if (this.left > other.left) {
                    return this.left - other.left
                }
                break
            case Direction.up:
                if (this.bottom < other.bottom) {
                    return other.bottom - this.bottom
                }
                break
            case Direction.down:
                if (this.top > other.top) {
                    return this.top - other.top
                }
                break
        }
        return 0
    }

    //Returns a beam to one side of us.
    get_beam(side: Direction, page_dim: Box) : Box {
        let beam = new Box(this)
        switch (side) {
            case Direction.left:
                beam.left = 0
                break
            case Direction.right:
                beam.right = page_dim.get_width()
                break
            case Direction.up:
                beam.top = 0
                break
            case Direction.down:
                beam.bottom = page_dim.get_height()
                break
            default:
                console.log('Error: get_beam was passed an invalid side argument')
        }
        return beam
    }

    //Returns a side value.
    get_extreme(side: Direction) : number {
        switch (side) {
            case Direction.left:
                return this.left
            case Direction.right:
                return this.right
            case Direction.up:
                return this.top
            case Direction.down:
                return this.bottom
            default:
                console.log("Error: Box.get_extreme(side) was given an unknown side argument: " + side)
        }
        return 0
    }
}











    


    


    


