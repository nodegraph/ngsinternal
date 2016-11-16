#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/utils/enumutil.h>

namespace ngs {

#define COMPONENT_ENTRY1(NAME) k##NAME,
#define COMPONENT_ENTRY2(NAME, VALUE) k##NAME = VALUE,

#define JSTYPE_ENTRIES()\
COMPONENT_ENTRY1(Object)\
COMPONENT_ENTRY1(Array)\
COMPONENT_ENTRY1(String)\
COMPONENT_ENTRY1(Number)\
COMPONENT_ENTRY1(Boolean)\
COMPONENT_ENTRY1(Null)\
COMPONENT_ENTRY1(Undefined)\

enum class JSType: int {
  JSTYPE_ENTRIES()
};

COMMS_EXPORT const char* js_type_to_string(JSType type);

}
