#pragma once
#include <base/utils/utils_export.h>

// Std.
#include <type_traits>

namespace ngs {


#define CHECK_FUNDAMENTAL_OR_ENUM_TYPE()\
  static_assert(std::is_fundamental<DataType>::value || std::is_enum<DataType>::value, "Fundamental Type is Required.");

}
