#pragma once
#include <components/computes/computes_export.h>
#include <base/utils/enumutil.h>

namespace ngs {

enum class ParamType: int {
  kMap,
  kList,

  kString,
  kStringList,

  kInt,
  kIntList,

  kDouble,
  kDoubleList,

  kBoolean,
  kBooleanList,
};

enum class JSType: int {
  kUndefined,
  kNull,
  kObject,
  kArray,
  kString,
  kNumber,
  kBoolean,
};

enum class HintType: int {
  kEnum, // Whether an int represents some type of enum.
  kJSType, // This is used for array/object types and describes the JS type of elements inside the array/object.
  kDescription, // A description of the parameter.
  kResizable, // Whether an array or object parameter is adjustable in terms of the number of elements it contains.
  kMin,
  kMax,
};

// These the strings associated with HintTypes.
// These are used in QVariantMaps as the keys (strings).
struct COMPUTES_EXPORT Hint {
  static const char* get_as_string(HintType type);
  static const char* kEnum;
  static const char* kMin;
  static const char* kMax;
};

enum class EnumHint: int {
  kNotEnumType,
  kMessageType,
  kInfoType,
  kMouseActionType,
  kTextActionType,
  kElementActionType,
  kWrapType,
  kDirectionType,
};

}
