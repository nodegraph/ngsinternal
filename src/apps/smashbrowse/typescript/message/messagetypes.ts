



// ******** This file is autogenerated. *********



// Note we don't use the Q_DECLARE_METATYPE() or the Q_ENUM() macro because we may eventually
// move away from qml to javascript/typescript oriented gui framework, where we want our json
// message to only contain pure javascript primitive types.
// Note these enums have to be kept in sync with the typescript file, socketmessage.ts.
const enum RequestType {
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

const enum MouseActionType {
  kSendClick,
  kSendCtrlClick,
  kMouseOver,
}

const enum TextActionType {
  kSendText,
  kSendEnter,
}

const enum ElementActionType {
  kGetText,
  kChooseOption,
  kScroll
}

const enum InfoType {
  kPageIsLoading,
  kPageIsReady,
  kBgIsConnected,
  kShowWebActionMenu,
  kFirebaseChanged,
}

const enum MessageType {
  kUnformedMessage,
  kRequestMessage,
  kResponseMessage,
  kInfoMessage
}

const enum WrapType {
  text,
  image,
  input,
  select,
}

const enum DirectionType {
  left,
  right,
  up,
  down
}

const enum HTTPSendType {
  kGet,
  kPost,
  kPut,
  kDelete,
  kPatch
}


