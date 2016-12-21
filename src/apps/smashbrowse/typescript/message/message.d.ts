



declare interface IElementInfo {
    // Frame and element info.
    frame_index_path: string
    xpath: string
    // Dimension and stacking order.
    box: IBox // These are the bounds in global client space.
    z_index: number
}

declare interface IClickInfo {
    // Frame and element info.
    frame_index_path: string,
    xpath: string,
    // Click pos.
    global_mouse_position: IPoint, // In global client space.
    local_mouse_position: IPoint, // In local space relative to the element of interest. (Calculated by subtracting the click pos from the element pos in page space coordinates.)
    // Text and image values under click.
    text_values: string[],
    image_values: string[]
}

interface IDropDownInfo {
    option_values: string[],
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