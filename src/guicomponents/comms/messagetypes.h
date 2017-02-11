#pragma once
namespace ngs {

// Note we don't use the Q_DECLARE_METATYPE() or the Q_ENUM() macro because we may eventually
// move away from qml to javascript/typescript oriented gui framework, where we want our json
// message to only contain pure javascript primitive types.

enum class WebDriverRequestType : int {
  kUnknownWebDriverRequest = 0,

  // Service Requests.
  kStartService = 1,
  kStopService = 2,

  // Browser Requests.
  kIsBrowserOpen = 3,
  kOpenBrowser = 4,
  kCloseBrowser = 5,
  kReleaseBrowser = 6,
  kResizeBrowser = 7,

  // Web Page Navigation.
  kNavigateTo = 8,
  kNavigateBack = 9,
  kNavigateForward = 10,
  kNavigateRefresh = 11,
  kGetCurrentURL = 12,

  // Web Page Tabs.
  kUpdateCurrentTab = 13,
  kDestroyCurrentTab = 14,

  // Actions.
  kPerformMouseAction = 15,
  kPerformTextAction = 16,
  kPerformElementAction = 17,
};

enum class ChromeRequestType : int {
  kUnknownChromeRequest = 0,

  // Cookies.
  kClearAllCookies = 1,
  kGetAllCookies = 2,
  kSetAllCookies = 3,

  // Zoom.
  kGetZoom = 4,

  // Event Control.
  kBlockEvents = 5,
  kUnblockEvents = 6,
  kWaitUntilLoaded = 7,

  // Tabs.
  kUpdateCurrentTab = 8,
  kOpenTab = 9,
  kGetActiveTabTitle = 10,

  // Current element on Page.
  kUpdateElement = 11,
  kClearElement = 12,
  kGetElement = 13,
  kSetElement = 14,
  kHasElement = 15,

  // Extract info from element.
  kGetElementValues = 16,

  // Act on element.
  kScrollElementIntoView = 17,

  // Find the current element.
  kFindElementByPosition = 18,
  kFindElementByValues = 19,
  kFindElementByType = 20,

  // Shift the current element.
  kShiftElementByType = 21,
  kShiftElementByValues = 22,

  // Info Request.
  kGetCrosshairInfo = 23,
  kGetDropDownInfo = 24
};

enum class PlatformRequestType : int {
  kUnknownPlatformRequest = 0,
  kAcceptSaveDialog = 1,
  kDownloadVideo = 2,
};

enum class FirebaseRequestType : int {
    kFirebaseInit = 0,
    kFirebaseDestroy = 1,
    kFirebaseSignIn = 2,
    kFirebaseSignOut = 3,
    kFirebaseWriteData = 4,
    kFirebaseReadData = 5,
    kFirebaseSubscribe = 6,
    kFirebaseUnsubscribe = 7
};

enum class MouseActionType : int {
  kSendClick = 0,
  kSendCtrlClick = 1,
  kMouseOver = 2
};

enum class TextActionType : int {
  kSendText = 0,
  kSendEnter = 1
};

enum class ElementActionType : int {
  kGetText = 0,
  kChooseOption = 1,
  kScroll = 2
};

enum class InfoType : int {
  kPageIsLoading = 0,
  kPageIsReady = 1,
  kBgIsConnected = 2,
  kShowWebActionMenu = 3,
  kFirebaseChanged = 4,
  kTabCreated = 5,
  kTabDestroyed = 6
};

enum class MessageType : int {
  kUnformedMessage = 0,
  kRequestMessage = 1,
  kResponseMessage = 2,
  kInfoMessage = 3
};

enum class WrapType : int {
  text = 0,
  image = 1,
  input = 2,
  select = 3
};

enum class DirectionType : int {
  left = 0,
  right = 1,
  up = 2,
  down = 3
};

enum class HTTPSendType: int {
  kGet = 0,
  kPost = 1,
  kPut = 2,
  kDelete = 3,
  kPatch = 4
};

enum class LicenseEdition: int {
  kLite = 0,
  kPro = 1
};

}
