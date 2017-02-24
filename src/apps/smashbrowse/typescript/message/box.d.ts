declare interface IBox {
    left: number
    right: number
    top: number
    bottom: number
}

declare class Box {
    left: number
    right: number
    top: number
    bottom: number
    constructor(box: IBox)

    reset(): void
    add_offset(p: IPoint): void
    subtract_offset(p: IPoint): void

    get_as_string(): string

    get_area(): number

    get_center(): Point

    get_width(): number

    get_height(): number

    get_mid_x(): number

    get_mid_y(): number

    get_relative_point(p: Point): Point

    to_client_space(local_win: Window): void
    to_page_space(local_win: Window): void

    contains(inner: IBox): boolean
    contains_point(page_pos: Point): boolean
    intersects(other: IBox): boolean
    is_oriented_on(side: DirectionType, other: IBox): number
    get_beam(side: DirectionType, page_dim: Box) : Box
    get_extreme(side: DirectionType) : number
}