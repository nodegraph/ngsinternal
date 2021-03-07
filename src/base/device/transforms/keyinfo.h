#pragma once
#include <base/device/device_export.h>


namespace ngs {


class DEVICE_EXPORT KeyInfo {
 public:

  int key_code;

  // Modifiers.
  bool control_modifier;
  bool alt_modifier;
  bool shift_modifier;
};

}
