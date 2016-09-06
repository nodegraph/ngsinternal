
declare const enum RequestType {
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

declare const enum ActionType {
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

declare const enum InfoType {
    kPageIsLoading,
    kPageIsReady,
}

declare const enum MessageType {
    kUnformedMessage,
    kRequestMessage,
    kResponseMessage,
    kInfoMessage
}

declare class BaseMessage {
    msg_type: MessageType
    construtor(): void
    static create_from_string(s: string): BaseMessage
    static create_from_obj(obj: any): BaseMessage

    to_string(): string
    get_msg_type(): MessageType
}

declare class RequestMessage extends BaseMessage {
    request: RequestType
    xpath: string
    args: any // a key value dict of arguments
    constructor(request: RequestType, args?: any, xpath?: string)
}

declare class ResponseMessage extends BaseMessage {
    success: boolean
    value: any
    constructor(success: boolean, value?: any)
}

declare class InfoMessage extends BaseMessage {
    info: InfoType
    constructor(info: InfoType)
}