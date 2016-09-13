

const enum RequestType {
    kUnknownRequest,

    // Chrome BG Requests.
    kClearAllCookies,
    kGetAllCookies,
    kSetAllCookies,
    kGetZoom,

    // Browser Requests.
    kShutdown,
    kCheckBrowserIsOpen,
    kResizeBrowser,
    kOpenBrowser,
    kCloseBrowser,

    // Web Page Requests.
    kBlockEvents,
    kUnblockEvents,
    kNavigateTo,
    kNavigateBack,
    kNavigateForward,
    kNavigateRefresh,
    kSwitchIFrame,

    // Page Content Set Requests.
    kPerformAction,
    kUpdateOveralys,
    kCreateSetFromMatchValues,
    kCreateSetFromWrapType,
    kDeleteSet,
    kShiftSet,
    kExpandSet,
    kMarkSet,
    kUnmarkSet,
    kMergeMarkedSets,
    kShrinkSetToMarked,
    kShrinkSet,

    // Info Request.
    kGetCrosshairInfo,
    kGetOverlayXPath,
}

const enum ActionType {
    kSendClick,
    kMouseOver,
    kSendText,
    kSendEnter,
    kGetText,
    kSelectOption,
    kScrollDown,
    kScrollUp,
    kScrollRight,
    kScrollLeft
}

const enum InfoType {
    kPageIsLoading,
    kPageIsReady,
    kBgIsConnected,
    kShowWebActionMenu
}

const enum MessageType {
    kUnformedMessage,
    kRequestMessage,
    kResponseMessage,
    kInfoMessage
}

class BaseMessage {
    msg_type: MessageType
    id: Number // RequestMessage and ResponseMessage id should match. (-1 means we don't care/track about it.)
    iframe: string // The iframe identified by a path of indices. eg /1/0/3/2

    constructor(id: Number, iframe: string) {
        this.msg_type = MessageType.kUnformedMessage
        this.id = id
        this.iframe = iframe
    }

    static create_from_string(s: string): BaseMessage {
        let obj = JSON.parse(s)
        return BaseMessage.create_from_obj(obj)
    }

    static create_from_obj(obj: any): BaseMessage {
        // Now we create the right message and copy properties from obj.
        // Note that in ES6, we can use Object.assign.
        let iframe: string = obj.iframe
        let id: Number = obj.id
        switch (obj.msg_type) {
            case MessageType.kRequestMessage: {
                let req = new RequestMessage(id, iframe, RequestType.kUnknownRequest)
                if (obj.hasOwnProperty('request')) {
                    req.request = obj.request
                }
                if (obj.hasOwnProperty('args')) {
                    req.args = obj.args
                }
                if (obj.hasOwnProperty('xpath')) {
                    req.xpath = obj.xpath
                }
                return req
            }
            case MessageType.kResponseMessage: {
                let resp = new ResponseMessage(id, iframe, false)
                if (obj.hasOwnProperty('success')) {
                    resp.success = obj.success
                }
                if (obj.hasOwnProperty('value')) {
                    resp.value = obj.value
                }
                return resp
            }
            case MessageType.kInfoMessage: {
                let info = new InfoMessage(id, iframe, <InfoType>obj.info)
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
    xpath: string
    args: any // a key value dict of arguments
    constructor(id: Number, iframe: string, request: RequestType, args: any = {}, xpath: string = "") {
        super(id, iframe)
        this.msg_type = MessageType.kRequestMessage
        this.request = request
        this.args = args
        this.xpath = xpath
    }
}

class ResponseMessage extends BaseMessage {
    success: boolean
    value: any
    constructor(id: Number, iframe: string, success: boolean, value: any = 0) {
        super(id, iframe)
        this.msg_type = MessageType.kResponseMessage
        this.success = success
        this.value = value
    }
}

class InfoMessage extends BaseMessage {
    info: InfoType
    value: any
    constructor(id: Number, iframe: string, info: InfoType, value: any = 0) {
        super(id, iframe)
        this.msg_type = MessageType.kInfoMessage
        this.info = info
        this.value = value
    }
}

