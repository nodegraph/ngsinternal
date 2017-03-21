#include <guicomponents/comms/messagetypestrings.h>
#include <cassert>

namespace ngs {

const char* receiver_type_to_string(ReceiverType t) {
  switch(t) {
    case ReceiverType::kUnknown: return "kUnknown";
    case ReceiverType::kWebDriver: return "kWebDriver";
    case ReceiverType::kChrome: return "kChrome";
    case ReceiverType::kPlatform: return "kPlatform";
    case ReceiverType::kFirebase: return "kFirebase";

    default: {
      assert(false);
    }
  }
  return "";
}

const char* web_driver_request_type_to_string(WebDriverRequestType t) {
  switch(t) {
    case WebDriverRequestType::kUnknownWebDriverRequest: return "kUnknownWebDriverRequest";

    // Service Requests.
    case WebDriverRequestType::kStartService: return "kStartService";
    case WebDriverRequestType::kStopService: return "kStopService";

    // Browser Requests.
    case WebDriverRequestType::kIsBrowserOpen: return "kIsBrowserOpen";
    case WebDriverRequestType::kOpenBrowser: return "kOpenBrowser";
    case WebDriverRequestType::kOpenBrowserPost: return "kOpenBrowserPost";
    case WebDriverRequestType::kCloseBrowser: return "kCloseBrowser";
    case WebDriverRequestType::kReleaseBrowser: return "kReleaseBrowser";
    case WebDriverRequestType::kSetBrowserSize: return "kSetBrowserSize";
    case WebDriverRequestType::kGetBrowserSize: return "kGetBrowserSize";

    // Web Page Navigation.
    case WebDriverRequestType::kNavigateTo: return "kNavigateTo";
    case WebDriverRequestType::kNavigateBack: return "kNavigateBack";
    case WebDriverRequestType::kNavigateForward: return "kNavigateForward";
    case WebDriverRequestType::kNavigateRefresh: return "kNavigateRefresh";
    case WebDriverRequestType::kGetCurrentURL: return "kGetCurrentURL";

    // Web Page Tabs.
    case WebDriverRequestType::kUpdateCurrentTab: return "kUpdateCurrentTab";
    case WebDriverRequestType::kDestroyCurrentTab: return "kDestroyCurrentTab";

    // Actions.
    case WebDriverRequestType::kPerformMouseAction: return "kPerformMouseAction";
    case WebDriverRequestType::kPerformTextAction: return "kPerformTextAction";
    case WebDriverRequestType::kPerformElementAction: return "kPerformElementAction";
    case WebDriverRequestType::kDownloadImage: return "kPerformImageAction";

    default: {
      assert(false);
    }
  }
  return "";
}

const char* chrome_request_type_to_string(ChromeRequestType t) {
  switch(t) {
    case ChromeRequestType::kUnknownChromeRequest: return "kUnknownChromeRequest";

    // Cookies.
    case ChromeRequestType::kClearAllCookies: return "kClearAllCookies";
    case ChromeRequestType::kGetAllCookies: return "kGetAllCookies";
    case ChromeRequestType::kSetAllCookies: return "kSetAllCookies";

    // Zoom.
    case ChromeRequestType::kGetZoom: return "kGetZoom";
    case ChromeRequestType::kSetZoom: return "kSetZoom";

    // Event Control.
    case ChromeRequestType::kBlockEvents: return "kBlockEvents";
    case ChromeRequestType::kUnblockEvents: return "kUnblockEvents";
    case ChromeRequestType::kWaitUntilLoaded: return "kWaitUntilLoaded";

    // Tabs.
    case ChromeRequestType::kUpdateCurrentTab: return "kUpdateCurrentTab";
    case ChromeRequestType::kOpenTab: return "kOpenTab";
    case ChromeRequestType::kGetActiveTabTitle: return "kGetActiveTabTitle";

    // Element Overlays..
    case ChromeRequestType::kUpdateElementHighlights: return "kUpdateElementHighlights";
    case ChromeRequestType::kClearElementHighlights: return "kClearElementHighlights";
    case ChromeRequestType::kHighlightElements: return "kHighlightElements";

    // Scroll an element.
    case ChromeRequestType::kScrollElementIntoView: return "kScrollElementIntoView";
    case ChromeRequestType::kScrollElement: return "kScrollElement";

    // Info Request.
    case ChromeRequestType::kGetAllElements: return "kGetAllElements";
    case ChromeRequestType::kUpdateFrameOffsets: return "kUpdateFrameOffsets";
    case ChromeRequestType::kReleaseBrowser: return "kReleaseBrowser";

    default: {
      assert(false);
    }
  }
  return "";
}


const char* platform_request_type_to_string(PlatformRequestType t) {
  switch(t) {
    case PlatformRequestType::kUnknownPlatformRequest: return "kUnknownPlatformRequest";
    case PlatformRequestType::kAcceptSaveDialog: return "kAcceptSaveDialog";
    case PlatformRequestType::kDownloadVideo: return "kDownloadVideo";
    default: {
      assert(false);
    }
  }
  return "";
}

const char* firebase_request_type_to_string(FirebaseRequestType t) {
  switch(t) {
    case FirebaseRequestType::kFirebaseInit: return "kFirebaseInit";
    case FirebaseRequestType::kFirebaseDestroy: return "kFirebaseDestroy";
    case FirebaseRequestType::kFirebaseSignIn: return "kFirebaseSignIn";
    case FirebaseRequestType::kFirebaseSignOut: return "kFirebaseSignOut";
    case FirebaseRequestType::kFirebaseWriteData: return "kFirebaseWriteData";
    case FirebaseRequestType::kFirebaseReadData: return "kFirebaseReadData";
    case FirebaseRequestType::kFirebaseSubscribe: return "kFirebaseSubscribe";
    case FirebaseRequestType::kFirebaseUnsubscribe: return "kFirebaseUnsubscribe";

    default: {
      assert(false);
    }
  }
  return "";
}

const char* mouse_action_type_to_string(MouseActionType t) {
  switch(t) {
    case MouseActionType::kSendClick: return "kSendClick";
    case MouseActionType::kSendCtrlClick: return "kSendCtrlClick";
    case MouseActionType::kMouseOver: return "kMouseOver";
    default: {
      assert(false);
    }
  }
  return "";
}

const char* text_action_type_to_string(TextActionType t) {
  switch(t) {
    case TextActionType::kSendText: return "kSendText";
    case TextActionType::kSendEnter: return "kSendEnter";

    default: {
      assert(false);
    }
  }
  return "";
}

const char* element_action_type_to_string(ElementActionType t) {
  switch(t) {
    case ElementActionType::kChooseOption: return "kChooseOption";

    default: {
      assert(false);
    }
  }
  return "";
}

const char* info_type_to_string(InfoType t) {
  switch (t) {
    case InfoType::kPageIsLoading: return "kPageIsLoading";
    case InfoType::kPageIsReady: return "kPageIsReady";
    case InfoType::kBgIsConnected: return "kBgIsConnected";
    case InfoType::kShowWebActionMenu: return "kShowWebActionMenu";
    case InfoType::kTabCreated: return "kTabCreated";
    case InfoType::kTabDestroyed: return "kTabDestroyed";
    case InfoType::kCollectFrameIndexPaths: return "kCollectFrameIndexPaths";
    case InfoType::kCollectFrameOffsets: return "kCollectFrameOffsets";
    case InfoType::kDistributeFrameOffsets: return "kDistributeFrameOffsets";

    default: {
      assert(false);
    }
  }
  return "";
}

const char* message_type_to_string(MessageType t) {
  switch (t) {
    case MessageType::kUnformedMessage: return "kUnformedMessage";
    case MessageType::kRequestMessage: return "kRequestMessage";
    case MessageType::kResponseMessage: return "kResponseMessage";
    case MessageType::kInfoMessage: return "kInfoMessage";

    default: {
      assert(false);
    }
  }
  return "";
}

const char* element_type_to_string(ElementType t) {
  switch (t) {
    case ElementType::kText: return "kText";
    case ElementType::kImage: return "kImage";
    case ElementType::kInput: return "kInput";
    case ElementType::kSelect: return "kSelect";

    default: {
      assert(false);
    }
  }
  return "";
}

const char* direction_type_to_string(DirectionType t) {
  switch (t) {
    case DirectionType::kLeft: return "kLeft";
    case DirectionType::kRight: return "kRight";
    case DirectionType::kUp: return "kUp";
    case DirectionType::kDown: return "kDown";

    default: {
      assert(false);
    }
  }
  return "";
}

const char* http_send_type_to_string(HTTPSendType t) {
  switch (t) {
    case HTTPSendType::kGet: return "kGet";
    case HTTPSendType::kPost: return "kPost";
    case HTTPSendType::kPut: return "kPut";
    case HTTPSendType::kDelete: return "kDelete";
    case HTTPSendType::kPatch: return "kPatch";

    default: {
      assert(false);
    }
  }
  return "";
}

const char* license_edition_to_string(LicenseEdition t) {
  switch (t) {
    case LicenseEdition::kLite: return "kLite";
    case LicenseEdition::kPro: return "kPro";

    default: {
      assert(false);
    }
  }
  return "";
}



}
