class Point {
    x: number
    y: number
    constructor(point: IPoint) {
        this.x = point.x
        this.y = point.y
    }
    to_string(): string {
        return '(' + this.x + ',' + this.y + ')'
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
    
    assign(other: IPoint) : void {
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

    increment(other: IPoint) : void {
        this.x += other.x
        this.y += other.y
    }

    decrement(other: IPoint) : void{
        this.x -= other.x
        this.y -= other.y
    }

    add(other: IPoint) : Point {
        return new Point({x: this.x + other.x, y: this.y + other.y})
    }

    subtract(other: IPoint) : Point{
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

    lt(other: IPoint) : boolean {
        return (this.x < other.x) && (this.y < other.y)
    }

    lte(other: IPoint) : boolean {
        return (this.x <= other.x) && (this.y <= other.y)
    }

    gt(other: IPoint) : boolean {
        return (this.x > other.x) && (this.y > other.y)
    }
    
    gte(other: IPoint) : boolean {
        return (this.x >= other.x) && (this.y >= other.y)
    }


}










