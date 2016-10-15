#pragma once
#include <base/utils/utils_export.h>
#include <type_traits>

namespace ngs {

template<class E>
typename std::underlying_type<E>::type to_underlying(E e) {
   return static_cast<typename std::underlying_type<E>::type>(e);
}

}
