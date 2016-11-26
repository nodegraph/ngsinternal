#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/utils/enumutil.h>

namespace ngs {

#define COMPONENT_ENTRY1(NAME) k##NAME,
#define COMPONENT_ENTRY2(NAME, VALUE) k##NAME = VALUE,

#define HINTKEY_ENTRIES()\
COMPONENT_ENTRY1(EnumHint)\
COMPONENT_ENTRY1(MinHint)\
COMPONENT_ENTRY1(MaxHint)\
COMPONENT_ENTRY1(ElementJSTypeHint)\
COMPONENT_ENTRY1(ElementEnumHint)\
COMPONENT_ENTRY1(DescriptionHint)\
COMPONENT_ENTRY1(MultiLineHint)\

// Hints for numbers.
// kEnumHint: If the js type is a number, then this represents the type of enum represented by an number javascript type.
// kMinHint
// kMaxHint
//
// Hints for elements in objects and arrays.
// kElementJSTypeHint: This represents the javascript type of elements inside an array/object.
// kElementEnumHint: This represents the type of enum represented by number elements inside an array/object.
//
// User info about the parameter.
// kDescriptionHint: A helpful description of the parameter.
//
// Gui hints.
// kMultiLineHint: Whether to use a single of multiline editor.


enum class HintKey: int {
  HINTKEY_ENTRIES()
};

COMMS_EXPORT const char* hint_key_to_string(HintKey x);

// ----------------------------------------------------------------------------------------------------------

#define ENUMHINTVALUE_ENTRIES()\
COMPONENT_ENTRY1(NotEnumType)\
COMPONENT_ENTRY1(MessageType)\
COMPONENT_ENTRY1(InfoType)\
COMPONENT_ENTRY1(MouseActionType)\
COMPONENT_ENTRY1(TextActionType)\
COMPONENT_ENTRY1(ElementActionType)\
COMPONENT_ENTRY1(WrapType)\
COMPONENT_ENTRY1(DirectionType)\
COMPONENT_ENTRY1(HTTPSendType)\
COMPONENT_ENTRY1(JSType)\

enum class EnumHintValue: int {
  ENUMHINTVALUE_ENTRIES()
};

COMMS_EXPORT const char* enum_hint_value_to_string(EnumHintValue x);

}
