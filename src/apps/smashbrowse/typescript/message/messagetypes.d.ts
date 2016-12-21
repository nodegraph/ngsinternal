



// ******** This file is autogenerated. *********



// Note we don't use the Q_DECLARE_METATYPE() or the Q_ENUM() macro because we may eventually
// move away from qml to javascript/typescript oriented gui framework, where we want our json
// message to only contain pure javascript primitive types.
// Note these enums have to be kept in sync with the typescript file, socketmessage.ts.
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

  // Page Content Set Requests.
  kUpdateElement,
  kClearElement,
  kGetElement,
  kSetElement,
  kHasElement,
  kGetElementValues,
  kFindElementByPosition,
  kFindElementByValues,
  kFindElementByType,
  kPerformMouseAction,
  kPerformTextAction,
  kPerformElementAction,
  kShiftElementByType,
  kShiftElementByValues,

  // Info Request.
  kGetCrosshairInfo,
  kGetDropDownInfo,

  // Firebase.
  kFirebaseInit,
  kFirebaseDestroy,
  kFirebaseSignIn,
  kFirebaseSignOut,
  kFirebaseWriteData,
  kFirebaseReadData,
  kFirebaseSubscribe,
  kFirebaseUnsubscribe
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
  kChooseOption,
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

declare const enum WrapType {
  text,
  image,
  input,
  select,
}

declare const enum DirectionType {
  left,
  right,
  up,
  down
}

declare const enum HTTPSendType {
  kGet,
  kPost,
  kPut,
  kDelete,
  kPatch
}


