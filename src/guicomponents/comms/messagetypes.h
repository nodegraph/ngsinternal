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
  kSetBrowserSize = 7,
  kGetBrowserSize = 8,

  // Web Page Navigation.
  kNavigateTo = 9,
  kNavigateBack = 10,
  kNavigateForward = 11,
  kNavigateRefresh = 12,
  kGetCurrentURL = 13,

  // Web Page Tabs.
  kUpdateCurrentTab = 14,
  kDestroyCurrentTab = 15,

  // Actions.
  kPerformMouseAction = 16,
  kPerformTextAction = 17,
  kPerformElementAction = 18,
  kPerformImageAction = 19,
};

enum class ChromeRequestType : int {
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

  // Act on element.
  kScrollElementIntoView = 18,

  // Find the current element.
  kFindElementByPosition = 19,
  kFindElementByValues = 20,
  kFindElementByType = 21,

  // Shift the current element.
  kShiftElementByType = 22,
  kShiftElementByValues = 23,
  kShiftElementByTypeAlongRows = 24,
  kShiftElementByValuesAlongRows = 25,

  // Info Request.
  kGetCrosshairInfo = 26,
  kGetDropDownInfo = 27,
  kUpdateIFrameOffsets = 28,
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

enum class ImageActionType : int {
  kGetImageURL = 0,
  kDownloadImage = 1,
};

enum class InfoType : int {
  kPageIsLoading = 0,
  kPageIsReady = 1,
  kBgIsConnected = 2,
  kShowWebActionMenu = 3,
  kTabCreated = 4,
  kTabDestroyed = 5,
  kCollectIFrameIndexPaths = 6,
  kCollectIFrameOffsets = 7,
  kDistributeIFrameOffsets = 8
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
