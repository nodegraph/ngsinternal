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
    // Assumes we are in page space and converts to client space.
    to_client_space() : void {
        this.x -= window.scrollX
        this.y -= window.scrollY
    }

    // Assumes we are in client space and convert to page space.
    to_page_space() : void {
        this.x += window.scrollX
        this.y += window.scrollY
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










