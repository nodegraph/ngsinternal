
declare const enum RequestType {
    kUnknownRequest,
    
    // Chrome BG Requests.
    kClearAllCookies,
    kGetAllCookies,
    kSetAllCookies,
    kGetZoom,

    // Browser Requests.
    kShutdown,
    kIsBrowserOpen,
    kResizeBrowser,
    kOpenBrowser,
    kCloseBrowser,

    // Web Page Requests.
    kBlockEvents,
    kUnblockEvents,
    kWaitUntilLoaded,
    kNavigateTo,
    kNavigateBack,
    kNavigateForward,
    kNavigateRefresh,
    kSwitchIFrame,
    
    // Page Content Set Requests.
    kGetXPath,
    kPerformMouseAction,
    kPerformTextAction,
    kPerformElementAction,
    kUpdateOveralys,
    kCacheFrame,
    kLoadCachedFrame,
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

    // Firebase.
    kFirebaseSignIn,
    kFirebaseSignOut,
    kFirebaseWriteData,
    kFirebaseReadData,
    kFirebaseListenToChanges
}

declare const enum MouseActionType {
    kSendClick,
    kMouseOver,
    kStartMouseHover,
    kStopMouseHover,
}

declare const enum TextActionType {
    kSendText,
    kSendEnter,
}

declare const enum ElementActionType {
    kGetText,
    kSelectOption,
    kScroll
}

declare const enum InfoType {
    kPageIsLoading,
    kPageIsReady,
    kBgIsConnected,
    kShowWebActionMenu,
    kFirebaseChanged,
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
    args: any // a key value dict of arguments
    constructor(id: Number, iframe: string, request: RequestType, args?: any)
}

declare class ResponseMessage extends BaseMessage {
    success: boolean // Whether the app should continue sending more requests in this sequence. False means some unrecoverable error has occured.
    value: any // The return value in response to a previous request.
    constructor(id: Number, iframe: string, success: boolean, value?: any)
}

declare class InfoMessage extends BaseMessage {
    info: InfoType
    value: any
    constructor(id: Number, iframe: string, info: InfoType, value?: any)
}