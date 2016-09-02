// Note this file is symlinked into chromeextension/background and chromeextension/content.
// The real file lives in controller.

export const enum RequestType {
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
    kPerformAction,
}

export const enum ActionType {
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

export class BaseMessage {
    construtor() { }
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
        } else {
            let resp = new ResponseMessage()
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

    is_request(): boolean {
        return false
    }
}

export class RequestMessage extends BaseMessage{
    request: RequestType
    xpath: string
    args: any // a key value dict of arguments
    constructor(request: RequestType, args: any = {}, xpath: string = "") {
        super()
        this.request = request
        this.args = args
        this.xpath = xpath
    }
    is_request(): boolean {
        return true
    }
}

export class ResponseMessage extends BaseMessage{
    success: boolean
    value: any
    constructor(success: boolean = false, value: any = 0) {
        super()
        this.success = success
        this.value = value
    }
    is_request(): boolean {
        return false
    }
}