// The data shape for box.
interface IBox {
    left: number
    right: number
    top: number
    bottom: number
}

interface IPoint {
    x: number
    y: number
}

// Info about an html element in the browser. 
interface IElementInfo {
    // Frame info.
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

interface IPoint{
    x: number
    y: number
}

interface IClickInfo extends IElementInfo {
    // Click pos.
    global_mouse_position: IPoint // In global client space.
    local_mouse_position: IPoint // In local space relative to the element of interest. (Calculated by subtracting the click pos from the element pos in page space coordinates.)
}

interface IDropDownInfo {
    option_values: string[]
    option_texts: string[]
}

class BaseMessage {
    receiver_type: ReceiverType
    msg_type: MessageType
    id: Number // RequestMessage and ResponseMessage id should match. (-1 means we don't care/track about it.)

    constructor(id: Number) {
        this.receiver_type = ReceiverType.kChrome
        this.msg_type = MessageType.kUnformedMessage
        this.id = id
    }

    static create_from_string(s: string): BaseMessage {
        let obj = JSON.parse(s)
        return BaseMessage.create_from_obj(obj)
    }

    static create_from_obj(obj: any): BaseMessage {
        // Now we create the right message and copy properties from obj.
        // Note that in ES6, we can use Object.assign.
        let id: Number = obj.id
        switch (obj.msg_type) {
            case MessageType.kRequestMessage: {
                let req = new RequestMessage(id, WebDriverRequestType.kUnknownWebDriverRequest)
                if (obj.hasOwnProperty('request')) {
                    req.request = obj.request
                }
                if (obj.hasOwnProperty('args')) {
                    req.args = obj.args
                }
                return req
            }
            case MessageType.kResponseMessage: {
                let resp = new ResponseMessage(id, false)
                if (obj.hasOwnProperty('success')) {
                    resp.success = obj.success
                }
                if (obj.hasOwnProperty('value')) {
                    resp.value = obj.value
                }
                return resp
            }
            case MessageType.kInfoMessage: {
                let info = new InfoMessage(id, <InfoType>obj.info)
                if (obj.hasOwnProperty('value')) {
                    info.value = obj.value
                }
                return info
            }
            default: {
                console.error('Error: Attempt to create message from obj without a message type: ' + JSON.stringify(obj))
                return null
            }
        }
    }

    to_string(): string {
        return JSON.stringify(this)
    }

    get_msg_type(): MessageType {
        return this.msg_type
    }
}

class RequestMessage extends BaseMessage {
    request: WebDriverRequestType
    args: any // a key value dict of arguments
    constructor(id: Number, request: WebDriverRequestType, args: any = {}) {
        super(id)
        this.msg_type = MessageType.kRequestMessage
        this.request = request
        this.args = args
    }
}

class ResponseMessage extends BaseMessage {
    success: boolean // Whether the app should continue sending more requests in this sequence. False means some unrecoverable error has occured.
    value: any // The return value in response to a previous request.
    constructor(id: Number, success: boolean, value: any = {}) {
        super(id)
        this.msg_type = MessageType.kResponseMessage
        this.success = success
        this.value = value
    }
}

class InfoMessage extends BaseMessage {
    info: InfoType
    value: any
    constructor(id: Number, info: InfoType, value: any = {}) {
        super(id)
        this.msg_type = MessageType.kInfoMessage
        this.info = info
        this.value = value
    }
}

// Converts a string composed of numbers separated by / into an array of numbers.
// Usually used to convert an FW or FE index path into an array of numbers.
function get_array_from_index_path(path: string) {
    let arr: number[] = []
    let splits = path.split('/')
    for (let i=0; i<splits.length; i++) {
        // Note when empty strings are split on '/', you get an array with one element which is an empty string.
        if (splits[i] === '') {
            continue
        }
        // Get the frame index as a number.
        arr.push(Number(splits[i]))
    }
    return arr
}
