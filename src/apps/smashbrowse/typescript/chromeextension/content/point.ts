//This interface represents the data shape for a point.
interface PointInterface {
    x: number
    y: number
}

class Point {
    x: number
    y: number
    constructor(point: PointInterface) {
        this.x = point.x
        this.y = point.y
    }
    to_string() {
        return '(' + this.x + ',' + this.y + ')'
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
    private to_global_client_space_helper(local_win: Window, to: boolean) {
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
                    this.x += factor * iframes[i].getBoundingClientRect().left
                    this.y += factor * iframes[i].getBoundingClientRect().top
                    found = true
                    break;
                }
            }
            if (!found) {
                console.error('Error Point::to_global_client_space did not find parenting iframe')
            }
            win = win.parent
        }
    }

    // Assumes we are in page space and converts to client space.
    to_client_space(local_win: Window) : void {
        this.x -= local_win.scrollX
        this.y -= local_win.scrollY
    }

    // Assumes we are in client space and convert to page space.
    to_page_space(local_win: Window) : void {
        this.x += local_win.scrollX
        this.y += local_win.scrollY
    }
    
    assign(other: PointInterface) : void {
        this.x = other.x
        this.y = other.y
    }

    reset() : void{
        this.x = 0
        this.y = 0
    }

    normalize() : void{
        let mag = this.get_magnitude()
        this.divide_elements(mag)
    }
    
    get_magnitude() : number {
        return Math.sqrt(this.x * this.x + this.y * this.y)
    }

    increment(other: PointInterface) : void {
        this.x += other.x
        this.y += other.y
    }

    decrement(other: PointInterface) : void{
        this.x -= other.x
        this.y -= other.y
    }

    add(other: PointInterface) : Point {
        return new Point({x: this.x + other.x, y: this.y + other.y})
    }

    subtract(other: PointInterface) : Point{
        return new Point({x: this.x - other.x, y: this.y - other.y})
    }

    divide_elements(factor: number) : void {
        this.x /= factor
        this.y /= factor
    }

    multiply_elements(factor: number) : void {
        this.x *= factor
        this.y *= factor
    }

    lt(other: PointInterface) : boolean {
        return (this.x < other.x) && (this.y < other.y)
    }

    lte(other: PointInterface) : boolean {
        return (this.x <= other.x) && (this.y <= other.y)
    }

    gt(other: PointInterface) : boolean {
        return (this.x > other.x) && (this.y > other.y)
    }
    
    gte(other: PointInterface) : boolean {
        return (this.x >= other.x) && (this.y >= other.y)
    }


}










