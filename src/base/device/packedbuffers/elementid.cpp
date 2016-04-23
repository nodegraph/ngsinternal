#include <base/device/packedbuffers/elementid.h>
#include <cmath>

namespace ngs {

template<>
DEVICE_EXPORT bool is_nan(const half& value) {
	return value.isNan();
}

template <class T>
bool is_nan(const T& value) {
	return std::isnan((double)value);
}

// template instantiations.

template DEVICE_EXPORT bool is_nan(const float& value);
template DEVICE_EXPORT bool is_nan(const int& value);
template DEVICE_EXPORT bool is_nan(const unsigned int& value);
template DEVICE_EXPORT bool is_nan(const short& value);
template DEVICE_EXPORT bool is_nan(const unsigned short& value);
template DEVICE_EXPORT bool is_nan(const char& value);
template DEVICE_EXPORT bool is_nan(const unsigned char& value);

}
