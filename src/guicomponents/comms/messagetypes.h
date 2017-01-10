
namespace ngs {

// Note we don't use the Q_DECLARE_METATYPE() or the Q_ENUM() macro because we may eventually
// move away from qml to javascript/typescript oriented gui framework, where we want our json
// message to only contain pure javascript primitive types.
// Note these enums have to be kept in sync with the typescript file, socketmessage.ts.
enum class RequestType : int {
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

  // Web Page Event Control.
  kBlockEvents,
  kUnblockEvents,
  kWaitUntilLoaded,

  // Web Page Navigation.
  kNavigateTo,
  kNavigateBack,
  kNavigateForward,
  kNavigateRefresh,
  kGetCurrentURL,

  // Web Page Tabs.
  kUpdateCurrentTab,
  kDestroyCurrentTab,

  // Page Content Set Requests.
  kUpdateElement,
  kClearElement,
  kGetElement,
  kSetElement,
  kHasElement,
  kScrollElementIntoView,
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
};

enum class MouseActionType : int {
  kSendClick,
  kSendCtrlClick,
  kMouseOver,
};

enum class TextActionType : int {
  kSendText,
  kSendEnter,
};

enum class ElementActionType : int {
  kGetText,
  kChooseOption,
  kScroll
};

enum class InfoType : int {
  kPageIsLoading,
  kPageIsReady,
  kBgIsConnected,
  kShowWebActionMenu,
  kFirebaseChanged,
};

enum class MessageType : int {
  kUnformedMessage,
  kRequestMessage,
  kResponseMessage,
  kInfoMessage
};

enum class WrapType : int {
  text,
  image,
  input,
  select,
};

enum class DirectionType : int {
  left,
  right,
  up,
  down
};

enum class HTTPSendType: int {
  kGet,
  kPost,
  kPut,
  kDelete,
  kPatch
};

}
