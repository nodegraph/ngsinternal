#include <entities/componentids.h>

namespace ngs {

const char* get_compute_name(size_t did) {
  if (did == kDotNodeCompute) {
    return "dot";
  } else if (did == kInputCompute) {
    return "input internal";
  } else if (did == kOutputCompute) {
    return "output internal";
  } else if (did == kGroupNodeCompute) {
    return "group";
  } else if (did == kInputNodeCompute) {
    return "input";
  } else if (did == kOutputNodeCompute) {
    return "output";
  } else if (did == kScriptNodeCompute) {
    return "script";
  } else if (did == kMockNodeCompute) {
    return "mock";
  } else if (did == kOpenBrowserCompute) {
    return "open browser";
  } else if (did == kCloseBrowserCompute) {
    return "close browser";
  } else if (did == kCreateSetFromValuesCompute) {
    return "create set from values";
  } else if (did == kCreateSetFromTypeCompute) {
    return "create set from types";
  } else if (did == kClickActionCompute) {
    return "click";
  }
  return "unknown compute type";
}

}
