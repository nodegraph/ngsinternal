#pragma once
#include <components/computes/computes_export.h>
#include <base/utils/enumutil.h>

namespace ngs {

enum class HintKey: int {
  // Core hints.
  kJSTypeHint, // This represents the base javascript type of the parameter and is required for all parameters.

  // Hints for numbers.
  kEnumHint, // If the js type is a number, then this represents the type of enum represented by an number javascript type.
  kMinHint,
  kMaxHint,

  // Hints for elements in objects and arrays.
  kElementJSTypeHint, // This represents the javascript type of elements inside an array/object.
  kElementEnumHint, // This represents the type of enum represented by number elements inside an array/object.
  kElementResizableHint, // Whether an array or object parameter is adjustable in terms of the number of elements it contains.

  // User info about the parameter.
  kDescriptionHint, // A helpful description of the parameter.

  // Gui hints.
  kMultiLineHint, // Whether to use a single of multiline editor.
};

//// These the strings associated with HintTypes.
//struct COMPUTES_EXPORT Hint {
//  static const char* get_as_string(HintType type);
//  static const char* kEnum;
//  static const char* kMin;
//  static const char* kMax;
//};

enum class EnumHintValue: int {
  kNotEnumType,
  kMessageType,
  kInfoType,
  kMouseActionType,
  kTextActionType,
  kElementActionType,
  kWrapType,
  kDirectionType,
  kHTTPSendType,
  kJSType,
};

}
