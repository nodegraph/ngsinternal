



declare interface IElementInfo {
    // Frame and element info.
    fw_index_path: string // An array of indices into nested windows (obtained by window.frames in javascript).
    fe_index_path: string // An array of indices into nested HTMLIFrameElements (obtained by document.getElementsByTagName('iframe') in javascript).
    // XPath.
    xpath: string
    // Geometry.
    box: IBox // These are the bounds in global client space.
    // Tag name.
    tag_name: string
    // Text gather elements children.
    text: string
    // Image on element.
    image: string
    // Href from parenting anchor, empty if no parenting anchor.
    href: string
    // Input text.
    input: string
    // Select options values and text.
    option_values: string[]
    option_texts: string[]
    // True if visible in the viewport. False if scrolled off viewport.
    in_viewport: boolean
}

declare interface IClickInfo extends IElementInfo {
    // Click pos.
    global_mouse_position: IPoint // In global client space.
    local_mouse_position: IPoint // In local space relative to the element of interest. (Calculated by subtracting the click pos from the element pos in page space coordinates.)
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
    request: ChromeRequestType
    args: any // a key value dict of arguments
    constructor(id: Number, request: ChromeRequestType, args?: any)
}

declare class ResponseMessage extends BaseMessage {
    success: boolean // Whether the app should continue sending more requests in this sequence. False means some unrecoverable error has occured.
    value: any // The return value in response to a previous request.
    constructor(id: Number, success: boolean, value: any)
}

declare class InfoMessage extends BaseMessage {
    info: InfoType
    value: any
    constructor(id: Number, info: InfoType, value?: any)
}

declare function get_array_from_index_path(path: string): number[];