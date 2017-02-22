declare interface IPoint {
    x: number
    y: number
}

declare class Point {
    x: number
    y: number
    constructor(point: IPoint)
    to_string(): string
    
    //to_global_client_space(local_win: Window): void
    //to_local_client_space(local_win: Window): void
    to_client_space(local_win: Window) : void
    to_page_space(local_win: Window) : void
    
    assign(other: IPoint) : void
    reset() : void
    
    normalize() : void
    get_magnitude() : number
    
    increment(other: IPoint) : void
    decrement(other: IPoint) : void
    
    add(other: IPoint) : Point
    subtract(other: IPoint) : Point
    divide_elements(factor: number) : void
    multiply_elements(factor: number) : void

    lt(other: IPoint) : boolean
    lte(other: IPoint) : boolean
    gt(other: IPoint) : boolean
    gte(other: IPoint) : boolean
}