#include <entities/componentids.h>

namespace ngs {

const char* get_compute_name(ComponentDID did) {
  if (did == ComponentDID::kDotNodeCompute) {
    return "dot";
  } else if (did == ComponentDID::kInputCompute) {
    return "input internal";
  } else if (did == ComponentDID::kOutputCompute) {
    return "output internal";
  } else if (did == ComponentDID::kGroupNodeCompute) {
    return "group";
  } else if (did == ComponentDID::kInputNodeCompute) {
    return "input";
  } else if (did == ComponentDID::kOutputNodeCompute) {
    return "output";
  } else if (did == ComponentDID::kScriptNodeCompute) {
    return "script";
  } else if (did == ComponentDID::kMockNodeCompute) {
    return "mock";
  } else if (did == ComponentDID::kOpenBrowserCompute) {
    return "open browser";
  } else if (did == ComponentDID::kCloseBrowserCompute) {
    return "close browser";
  } else if (did == ComponentDID::kCheckBrowserIsOpenCompute) {
    return "check browser is open";
  } else if (did == ComponentDID::kCheckBrowserSizeCompute) {
    return "check browser size";
  } else if (did == ComponentDID::kNavigateToCompute) {
    return "navigate to";
  } else if (did == ComponentDID::kNavigateRefreshCompute) {
    return "navigate refresh";
  } else if (did == ComponentDID::kSwitchToIFrameCompute) {
    return "switch to iframe";
  } else if (did == ComponentDID::kCreateSetFromValuesCompute) {
    return "create set from values";
  } else if (did == ComponentDID::kCreateSetFromTypeCompute) {
    return "create set from types";
  } else if (did == ComponentDID::kDeleteSetCompute) {
    return "delete set";
  } else if (did == ComponentDID::kShiftSetCompute) {
    return "shift set";
  } else if (did == ComponentDID::kMouseActionCompute) {
    return "mouse action";
  } else if (did == ComponentDID::kStartMouseHoverActionCompute) {
    return "start hover";
  } else if (did == ComponentDID::kStopMouseHoverActionCompute) {
    return "stop hover";
  } else if (did == ComponentDID::kTextActionCompute) {
    return "text action";
  } else if (did == ComponentDID::kElementActionCompute) {
    return "element action";
  } else if (did == ComponentDID::kExpandSetCompute) {
    return "expand set";
  } else if (did == ComponentDID::kMarkSetCompute) {
    return "mark set";
  } else if (did == ComponentDID::kUnmarkSetCompute) {
    return "unmark set";
  } else if (did == ComponentDID::kMergeSetsCompute) {
    return "merge sets";
  } else if (did == ComponentDID::kShrinkSetToSideCompute) {
    return "shrink set to side";
  } else if (did == ComponentDID::kShrinkAgainstMarkedCompute) {
    return "shrink set against marked";
  }
  return "unknown compute type";
}

}
