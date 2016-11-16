



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
  kMouseOver,
  kStartMouseHover,
  kStopMouseHover,
}

const enum TextActionType {
  kSendText,
  kSendEnter,
}

const enum ElementActionType {
  kGetText,
  kSelectOption,
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
  iframe
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

