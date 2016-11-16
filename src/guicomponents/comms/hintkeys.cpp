#include <guicomponents/comms/hintkeys.h>
#include <iostream>

namespace ngs {

const char* hint_key_to_string(HintKey x) {
#undef COMPONENT_ENTRY1
#undef COMPONENT_ENTRY2
#define COMPONENT_ENTRY1(NAME) case HintKey::k##NAME: return #NAME;
#define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  switch (x) {
    HINTKEY_ENTRIES()
  }
  return "unknown hint key";
}

const char* enum_hint_value_to_string(EnumHintValue x) {
#undef COMPONENT_ENTRY1
#undef COMPONENT_ENTRY2
#define COMPONENT_ENTRY1(NAME) case EnumHintValue::k##NAME: return #NAME;
#define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  switch (x) {
    ENUMHINTVALUE_ENTRIES()
  }
  return "unknown enum hint value";
}


}
