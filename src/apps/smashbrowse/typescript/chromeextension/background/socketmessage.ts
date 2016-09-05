// Note this file is symlinked into chromeextension/background and chromeextension/content.
// The real file lives in commhub.


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
    kNavigateTo,
    kNavigateBack,
    kNavigateForward,
    kNavigateRefresh,
    
    // Page Content Set Requests.
    kPerformAction,
    kCreateSetFromMatchValues,
    kCreateSetFromWrapType,
    kDeleteSet,
    kShiftSet,
    kExpandSet,
    kMarkSet,
    kUnmarkSet,
    kMergeMarkedSets,
    kShrinkSetToMarked,
    kShrinkSet
}

const enum ActionType {
    kSendClick,
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
}

const enum MessageType {
    kUnformedMessage,
    kRequestMessage,
    kResponseMessage,
    kInfoMessage
}

class BaseMessage {
    msg_type: MessageType

    construtor() {
        this.msg_type = MessageType.kUnformedMessage
    }

    static create_from_string(s: string): BaseMessage {
        let obj = JSON.parse(s)
        let msg: BaseMessage

        // Now we create the right message and copy properties from obj.
        // Note that in ES6, we can use Object.assign.
        if (obj.hasOwnProperty('request')) {
            let req = new RequestMessage(RequestType.kUnknownRequest)
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
        } if (obj.hasOwnProperty('info')) {
            let info = new InfoMessage(obj.info)
            return info
        } else {
            let resp = new ResponseMessage(false)
            if (obj.hasOwnProperty('success')) {
                resp.success = obj.success
            }
            if (obj.hasOwnProperty('value')) {
                resp.value = obj.value
            }
            return resp
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
    constructor(request: RequestType, args: any = {}, xpath: string = "") {
        super()
        this.msg_type = MessageType.kRequestMessage
        this.request = request
        this.args = args
        this.xpath = xpath
    }
}

class ResponseMessage extends BaseMessage {
    success: boolean
    value: any
    constructor(success: boolean, value: any = 0) {
        super()
        this.msg_type = MessageType.kResponseMessage
        this.success = success
        this.value = value
    }
}

class InfoMessage extends BaseMessage {
    info: InfoType
    constructor(info: InfoType) {
        super()
        this.msg_type = MessageType.kInfoMessage
        this.info = info
    }
}