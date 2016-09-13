
declare const enum RequestType {
    kUnknownRequest,
    kShowWebActionMenu,
    
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

declare const enum ActionType {
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

declare const enum InfoType {
    kPageIsLoading,
    kPageIsReady,
    kBgIsConnected,
    kShowWebActionMenu
}

declare const enum MessageType {
    kUnformedMessage,
    kRequestMessage,
    kResponseMessage,
    kInfoMessage
}

declare class BaseMessage {
    msg_type: MessageType
    id: number
    iframe: string
    constructor(id: Number, iframe: string)
    static create_from_string(s: string): BaseMessage
    static create_from_obj(obj: any): BaseMessage

    to_string(): string
    get_msg_type(): MessageType
}

declare class RequestMessage extends BaseMessage {
    request: RequestType
    xpath: string
    args: any // a key value dict of arguments
    constructor(id: Number, iframe: string, request: RequestType, args?: any, xpath?: string)
}

declare class ResponseMessage extends BaseMessage {
    success: boolean
    value: any
    constructor(id: Number, iframe: string, success: boolean, value?: any)
}

declare class InfoMessage extends BaseMessage {
    info: InfoType
    value: any
    constructor(id: Number, iframe: string, info: InfoType, value?: any)
}