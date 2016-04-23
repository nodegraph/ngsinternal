#pragma once
#include <base/device/device_export.h>
#include <OpenEXR/half.h>


namespace ngs {

enum ElementID {
	// 32 bits
	FloatElement,
	IntElement,
	UIntElement,
	// 8 bits
	CharElement,
	UCharElement,
	// 16 bits
	ShortElement,
	UShortElement,
	HalfElement
};

template <class T>
DEVICE_EXPORT bool is_nan(const T& value);



// -------------------------------------------------------
// ElementID to Type
// -------------------------------------------------------

template<ElementID ID>
struct ElementIDToType {
	typedef int Type;
};
template<>
struct ElementIDToType<FloatElement> {
	typedef float Type;
};
template<>
struct ElementIDToType<IntElement> {
	typedef int Type;
};
template<>
struct ElementIDToType<UIntElement> {
	typedef unsigned int Type;
};
template<>
struct ElementIDToType<CharElement> {
	typedef char Type;
};
template<>
struct ElementIDToType<UCharElement> {
	typedef unsigned char Type;
};
template<>
struct ElementIDToType<ShortElement> {
	typedef short Type;
};
template<>
struct ElementIDToType<UShortElement> {
	typedef unsigned short Type;
};
template<>
struct ElementIDToType<HalfElement> {
	typedef half Type;
};

// -------------------------------------------------------
// Type to ElementID
// -------------------------------------------------------

template<class T>
struct TypeToElementID {
	static const ElementID id = FloatElement;
};
template<>
struct TypeToElementID<float> {
	static const ElementID id = FloatElement;
};
template<>
struct TypeToElementID<int> {
	static const ElementID id = IntElement;
};
template<>
struct TypeToElementID<unsigned int> {
	static const ElementID id = UIntElement;
};
template<>
struct TypeToElementID<char> {
	static const ElementID id = CharElement;
};
template<>
struct TypeToElementID<unsigned char> {
	static const ElementID id = UCharElement;
};
template<>
struct TypeToElementID<short> {
	static const ElementID id = ShortElement;
};
template<>
struct TypeToElementID<unsigned short> {
	static const ElementID id = UShortElement;
};
template<>
struct TypeToElementID<half> {
	static const ElementID id = HalfElement;
};











}
