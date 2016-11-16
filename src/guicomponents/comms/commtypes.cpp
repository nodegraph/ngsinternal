#include <guicomponents/comms/commtypes.h>

namespace ngs {

const char* js_type_to_string(JSType type) {
#undef COMPONENT_ENTRY1
#undef COMPONENT_ENTRY2
#define COMPONENT_ENTRY1(NAME) case JSType::k##NAME: return #NAME;
#define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  switch (type) {
    JSTYPE_ENTRIES()
  }
  return "unknown js type";
}

}
