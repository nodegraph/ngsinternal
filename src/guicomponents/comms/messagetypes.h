#pragma once
namespace ngs {

// Note we don't use the Q_DECLARE_METATYPE() or the Q_ENUM() macro because we may eventually
// move away from qml to javascript/typescript oriented gui framework, where we want our json
// message to only contain pure javascript primitive types.

enum class ReceiverType : int  {
  Unknown = 0,
  WebDriver = 1,
  Chrome = 2,
  Platform = 3,
  Firebase = 4,
};

enum class WebDriverRequestType : int {
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

  // Element Overlays. These simply act as highlights in the browser.
  kHighlightElements = 12,
  kUpdateElementHighlights = 13,
  kClearElementHighlights = 14,

  // Scrolling.
  kScrollElementIntoView = 15,
  kScrollElement = 16,

  // Others.
  kGetAllElements = 17,
  kUpdateFrameOffsets = 18,

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
  kChooseOption = 0
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
  kCollectFrameIndexPaths = 6,
  kCollectFrameOffsets = 7,
  kDistributeFrameOffsets = 8
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
