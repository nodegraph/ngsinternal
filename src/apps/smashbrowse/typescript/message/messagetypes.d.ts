



// ******** This file is autogenerated. *********



// Note we don't use the Q_DECLARE_METATYPE() or the Q_ENUM() macro because we may eventually
// move away from qml to javascript/typescript oriented gui framework, where we want our json
// message to only contain pure javascript primitive types.

declare const enum ReceiverType {
  Unknown = 0,
  WebDriver = 1,
  Chrome = 2,
  Platform = 3,
  Firebase = 4,
}

declare const enum WebDriverRequestType {
  kUnknownWebDriverRequest = 0,

  // Service Requests.
  kStartService = 1,
  kStopService = 2,

  // Browser Requests.
  kIsBrowserOpen = 3,
  kOpenBrowser = 4,
  kOpenBrowserPost = 5,
  kCloseBrowser = 6,
  kReleaseBrowser = 7,
  kSetBrowserSize = 8,
  kGetBrowserSize = 9,

  // Web Page Navigation.
  kNavigateTo = 10,
  kNavigateBack = 11,
  kNavigateForward = 12,
  kNavigateRefresh = 13,
  kGetCurrentURL = 14,

  // Web Page Tabs.
  kUpdateCurrentTab = 15,
  kDestroyCurrentTab = 16,

  // Actions.
  kPerformMouseAction = 17,
  kPerformTextAction = 18,
  kPerformElementAction = 19,
  kPerformImageAction = 20,
}

declare const enum ChromeRequestType {
  kUnknownChromeRequest = 0,

  // Cookies.
  kClearAllCookies = 1,
  kGetAllCookies = 2,
  kSetAllCookies = 3,

  // Zoom.
  kGetZoom = 4,
  kSetZoom = 5,

  // Event Control.
  kBlockEvents = 6,
  kUnblockEvents = 7,
  kWaitUntilLoaded = 8,

  // Tabs.
  kUpdateCurrentTab = 9,
  kOpenTab = 10,
  kGetActiveTabTitle = 11,

  // Current element on Page.
  kUpdateElement = 12,
  kClearElement = 13,
  kGetElement = 14,
  kSetElement = 15,
  kHasElement = 16,

  // Extract info from element.
  kGetElementValues = 17,

  // Scroll an element.
  kScrollElementIntoView = 18,
  kScrollElement = 19,

  // Find the current element.
  kFindElementByPosition = 20,
  kFindElementByValues = 21,
  kFindElementByType = 22,

  // Shift the current element.
  kShiftElementByType = 23,
  kShiftElementByValues = 24,
  kShiftElementByTypeAlongRows = 25,
  kShiftElementByValuesAlongRows = 26,
  kShiftElementByTypeAlongColumns = 27,
  kShiftElementByValuesAlongColumns = 28,

  // Info Request.
  kGetCrosshairInfo = 29,
  kGetDropDownInfo = 30,
  kUpdateFrameOffsets = 31,

}

declare const enum PlatformRequestType {
  kUnknownPlatformRequest = 0,
  kAcceptSaveDialog = 1,
  kDownloadVideo = 2,
}

declare const enum FirebaseRequestType {
    kFirebaseInit = 0,
    kFirebaseDestroy = 1,
    kFirebaseSignIn = 2,
    kFirebaseSignOut = 3,
    kFirebaseWriteData = 4,
    kFirebaseReadData = 5,
    kFirebaseSubscribe = 6,
    kFirebaseUnsubscribe = 7
}

declare const enum MouseActionType {
  kSendClick = 0,
  kSendCtrlClick = 1,
  kMouseOver = 2
}

declare const enum TextActionType {
  kSendText = 0,
  kSendEnter = 1
}

declare const enum ElementActionType {
  kGetText = 0,
  kChooseOption = 1
}

declare const enum ImageActionType {
  kGetImageURL = 0,
  kDownloadImage = 1,
}

declare const enum InfoType {
  kPageIsLoading = 0,
  kPageIsReady = 1,
  kBgIsConnected = 2,
  kShowWebActionMenu = 3,
  kTabCreated = 4,
  kTabDestroyed = 5,
  kCollectFrameIndexPaths = 6,
  kCollectFrameOffsets = 7,
  kDistributeFrameOffsets = 8
}

declare const enum MessageType {
  kUnformedMessage = 0,
  kRequestMessage = 1,
  kResponseMessage = 2,
  kInfoMessage = 3
}

declare const enum WrapType {
  text = 0,
  image = 1,
  input = 2,
  select = 3
}

declare const enum DirectionType {
  left = 0,
  right = 1,
  up = 2,
  down = 3
}

declare const enum HTTPSendType {
  kGet = 0,
  kPost = 1,
  kPut = 2,
  kDelete = 3,
  kPatch = 4
}




declare const enum LicenseEdition {
  kLite = 0,
  kPro = 1
}


