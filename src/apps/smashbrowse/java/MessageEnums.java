import java.util.HashMap;

public class MessageEnums {




// ******** This file is autogenerated. *********



// Note we don't use the Q_DECLARE_METATYPE() or the Q_ENUM() macro because we may eventually
// move away from qml to javascript/typescript oriented gui framework, where we want our json
// message to only contain pure javascript primitive types.


static enum ReceiverType {
  Unknown(0),
  WebDriver(1),
  Chrome(2),
  Platform(3),
  Firebase(4),;

  private final int value;
  ReceiverType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, ReceiverType> map = new HashMap<Integer, ReceiverType>();
  static {for (ReceiverType e : ReceiverType.values()) {map.put(e.value, e);}}
  public static ReceiverType get_enum(int value) {return map.get(value);}
}


static enum WebDriverRequestType {
  kUnknownWebDriverRequest(0),

  // Service Requests.
  kStartService(1),
  kStopService(2),

  // Browser Requests.
  kIsBrowserOpen(3),
  kOpenBrowser(4),
  kOpenBrowserPost(5),
  kCloseBrowser(6),
  kReleaseBrowser(7),
  kSetBrowserSize(8),
  kGetBrowserSize(9),

  // Web Page Navigation.
  kNavigateTo(10),
  kNavigateBack(11),
  kNavigateForward(12),
  kNavigateRefresh(13),
  kGetCurrentURL(14),

  // Web Page Tabs.
  kUpdateCurrentTab(15),
  kDestroyCurrentTab(16),

  // Actions.
  kPerformMouseAction(17),
  kPerformTextAction(18),
  kPerformElementAction(19),
  kPerformImageAction(20),;

  private final int value;
  WebDriverRequestType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, WebDriverRequestType> map = new HashMap<Integer, WebDriverRequestType>();
  static {for (WebDriverRequestType e : WebDriverRequestType.values()) {map.put(e.value, e);}}
  public static WebDriverRequestType get_enum(int value) {return map.get(value);}
}


static enum ChromeRequestType {
  kUnknownChromeRequest(0),

  // Cookies.
  kClearAllCookies(1),
  kGetAllCookies(2),
  kSetAllCookies(3),

  // Zoom.
  kGetZoom(4),
  kSetZoom(5),

  // Event Control.
  kBlockEvents(6),
  kUnblockEvents(7),
  kWaitUntilLoaded(8),

  // Tabs.
  kUpdateCurrentTab(9),
  kOpenTab(10),
  kGetActiveTabTitle(11),

  // Current element on Page.
  kUpdateElement(12),
  kClearElement(13),
  kGetElement(14),
  kSetElement(15),
  kHasElement(16),

  kGetAllElements(17),

  // Extract info from element.
  kGetElementValues(18),

  // Scroll an element.
  kScrollElementIntoView(19),
  kScrollElement(20),

  // Find the current element.
  kFindElementByPosition(21),
  kFindElementByValues(22),
  kFindElementByType(23),

  // Shift the current element.
  kShiftElementByType(24),
  kShiftElementByValues(25),
  kShiftElementByTypeAlongRows(26),
  kShiftElementByValuesAlongRows(27),
  kShiftElementByTypeAlongColumns(28),
  kShiftElementByValuesAlongColumns(29),

  // Info Request.
  kGetCrosshairInfo(30),
  kGetDropDownInfo(31),
  kUpdateFrameOffsets(32),
;

  private final int value;
  ChromeRequestType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, ChromeRequestType> map = new HashMap<Integer, ChromeRequestType>();
  static {for (ChromeRequestType e : ChromeRequestType.values()) {map.put(e.value, e);}}
  public static ChromeRequestType get_enum(int value) {return map.get(value);}
}


static enum PlatformRequestType {
  kUnknownPlatformRequest(0),
  kAcceptSaveDialog(1),
  kDownloadVideo(2),;

  private final int value;
  PlatformRequestType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, PlatformRequestType> map = new HashMap<Integer, PlatformRequestType>();
  static {for (PlatformRequestType e : PlatformRequestType.values()) {map.put(e.value, e);}}
  public static PlatformRequestType get_enum(int value) {return map.get(value);}
}


static enum FirebaseRequestType {
    kFirebaseInit(0),
    kFirebaseDestroy(1),
    kFirebaseSignIn(2),
    kFirebaseSignOut(3),
    kFirebaseWriteData(4),
    kFirebaseReadData(5),
    kFirebaseSubscribe(6),
    kFirebaseUnsubscribe(7);

  private final int value;
  FirebaseRequestType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, FirebaseRequestType> map = new HashMap<Integer, FirebaseRequestType>();
  static {for (FirebaseRequestType e : FirebaseRequestType.values()) {map.put(e.value, e);}}
  public static FirebaseRequestType get_enum(int value) {return map.get(value);}
}


static enum MouseActionType {
  kSendClick(0),
  kSendCtrlClick(1),
  kMouseOver(2);

  private final int value;
  MouseActionType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, MouseActionType> map = new HashMap<Integer, MouseActionType>();
  static {for (MouseActionType e : MouseActionType.values()) {map.put(e.value, e);}}
  public static MouseActionType get_enum(int value) {return map.get(value);}
}


static enum TextActionType {
  kSendText(0),
  kSendEnter(1);

  private final int value;
  TextActionType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, TextActionType> map = new HashMap<Integer, TextActionType>();
  static {for (TextActionType e : TextActionType.values()) {map.put(e.value, e);}}
  public static TextActionType get_enum(int value) {return map.get(value);}
}


static enum ElementActionType {
  kGetText(0),
  kChooseOption(1);

  private final int value;
  ElementActionType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, ElementActionType> map = new HashMap<Integer, ElementActionType>();
  static {for (ElementActionType e : ElementActionType.values()) {map.put(e.value, e);}}
  public static ElementActionType get_enum(int value) {return map.get(value);}
}


static enum ImageActionType {
  kGetImageURL(0),
  kDownloadImage(1),;

  private final int value;
  ImageActionType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, ImageActionType> map = new HashMap<Integer, ImageActionType>();
  static {for (ImageActionType e : ImageActionType.values()) {map.put(e.value, e);}}
  public static ImageActionType get_enum(int value) {return map.get(value);}
}


static enum InfoType {
  kPageIsLoading(0),
  kPageIsReady(1),
  kBgIsConnected(2),
  kShowWebActionMenu(3),
  kTabCreated(4),
  kTabDestroyed(5),
  kCollectFrameIndexPaths(6),
  kCollectFrameOffsets(7),
  kDistributeFrameOffsets(8);

  private final int value;
  InfoType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, InfoType> map = new HashMap<Integer, InfoType>();
  static {for (InfoType e : InfoType.values()) {map.put(e.value, e);}}
  public static InfoType get_enum(int value) {return map.get(value);}
}


static enum MessageType {
  kUnformedMessage(0),
  kRequestMessage(1),
  kResponseMessage(2),
  kInfoMessage(3);

  private final int value;
  MessageType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, MessageType> map = new HashMap<Integer, MessageType>();
  static {for (MessageType e : MessageType.values()) {map.put(e.value, e);}}
  public static MessageType get_enum(int value) {return map.get(value);}
}


static enum WrapType {
  text(0),
  image(1),
  input(2),
  select(3);

  private final int value;
  WrapType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, WrapType> map = new HashMap<Integer, WrapType>();
  static {for (WrapType e : WrapType.values()) {map.put(e.value, e);}}
  public static WrapType get_enum(int value) {return map.get(value);}
}


static enum DirectionType {
  left(0),
  right(1),
  up(2),
  down(3);

  private final int value;
  DirectionType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, DirectionType> map = new HashMap<Integer, DirectionType>();
  static {for (DirectionType e : DirectionType.values()) {map.put(e.value, e);}}
  public static DirectionType get_enum(int value) {return map.get(value);}
}


static enum HTTPSendType {
  kGet(0),
  kPost(1),
  kPut(2),
  kDelete(3),
  kPatch(4);

  private final int value;
  HTTPSendType(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, HTTPSendType> map = new HashMap<Integer, HTTPSendType>();
  static {for (HTTPSendType e : HTTPSendType.values()) {map.put(e.value, e);}}
  public static HTTPSendType get_enum(int value) {return map.get(value);}
}





static enum LicenseEdition {
  kLite(0),
  kPro(1);

  private final int value;
  LicenseEdition(final int newValue) {value = newValue;}
  public int get_value() {return value;}
  
  private static HashMap<Integer, LicenseEdition> map = new HashMap<Integer, LicenseEdition>();
  static {for (LicenseEdition e : LicenseEdition.values()) {map.put(e.value, e);}}
  public static LicenseEdition get_enum(int value) {return map.get(value);}
}



}