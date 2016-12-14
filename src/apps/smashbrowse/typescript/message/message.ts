// The data shape for box.
interface IBox {
    left: number
    right: number
    top: number
    bottom: number
}

// Info about an html element in the browser. 
interface IElementInfo {
    iframe_index_path: string
    xpath: string
    box?: IBox // These are the bounds in global client space.
    weight?: number // A weight used to sort the elements.
}

interface IPoint{
    x: number
    y: number
}

interface ICrosshairInfo {
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

class BaseMessage {
    msg_type: MessageType
    id: Number // RequestMessage and ResponseMessage id should match. (-1 means we don't care/track about it.)
    iframe: string // The iframe identified by a path of indices. eg /1/0/3/2

    constructor(id: Number) {
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
                let req = new RequestMessage(id, RequestType.kUnknownRequest)
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
    request: RequestType
    args: any // a key value dict of arguments
    constructor(id: Number, request: RequestType, args: any = {}) {
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

