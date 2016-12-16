
declare interface IBox {
    left: number
    right: number
    top: number
    bottom: number
}

declare interface IElementInfo {
    iframe_index_path: string
    xpath: string
    box: IBox
}

declare interface IPoint{
    x: number
    y: number
}

declare interface IClickInfo {
    // Click pos.
    click_pos: IPoint,
    nearest_rel_click_pos: IPoint,
    overlay_rel_click_pos: IPoint,
    // Text and image values under click.
    text_values: string[],
    image_values: string[],
    // Select/Dropdown option texts.
    option_texts: string[]
}

declare class BaseMessage {
    msg_type: MessageType
    id: number
    constructor(id: Number)
    static create_from_string(s: string): BaseMessage
    static create_from_obj(obj: any): BaseMessage

    to_string(): string
    get_msg_type(): MessageType
}

declare class RequestMessage extends BaseMessage {
    request: RequestType
    args: any // a key value dict of arguments
    constructor(id: Number, request: RequestType, args?: any)
}

declare class ResponseMessage extends BaseMessage {
    success: boolean // Whether the app should continue sending more requests in this sequence. False means some unrecoverable error has occured.
    value: any // The return value in response to a previous request.
    constructor(id: Number, success: boolean, value?: any)
}

declare class InfoMessage extends BaseMessage {
    info: InfoType
    value: any
    constructor(id: Number, info: InfoType, value?: any)
}