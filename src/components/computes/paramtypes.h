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

enum class HintType: int {
  kEnum,
  kDescription,
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
