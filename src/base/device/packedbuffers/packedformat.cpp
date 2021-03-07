#include <base/device/deviceheadersgl.h>
#include <base/device/packedbuffers/packedformat.h>
#include <base/device/unpackedbuffers/defaultunpacking.h>

#include <cassert>

namespace ngs {

PackedFormat::PackedFormat() {
	_normalized_access = false;
	_packed_format = GL_RGBA;
	_packed_type = GL_INT;
}

// Set packed defaults.
void PackedFormat::set_packed_defaults(ElementID id, int num_channels, bool normalized_access) {
  _normalized_access = normalized_access;

	switch (id) {
	case FloatElement: {
		_packed_format = DefaultUnpacking<float>::get_default_packed_format(
				num_channels, normalized_access);
		_packed_type = DefaultUnpacking<float>::get_default_packed_type();
	}
		break;
	case HalfElement: {
		_packed_format = DefaultUnpacking<half>::get_default_packed_format(
				num_channels, normalized_access);
		_packed_type = DefaultUnpacking<half>::get_default_packed_type();
	}
		break;
	case IntElement: {
		_packed_format = DefaultUnpacking<int>::get_default_packed_format(
				num_channels, normalized_access);
		_packed_type = DefaultUnpacking<int>::get_default_packed_type();
	}
		break;
	case UIntElement: {
		_packed_format = DefaultUnpacking<unsigned int>::get_default_packed_format(
				num_channels, normalized_access);
		_packed_type = DefaultUnpacking<unsigned int>::get_default_packed_type();
	}
		break;
	case ShortElement: {
		_packed_format = DefaultUnpacking<short>::get_default_packed_format(
				num_channels, normalized_access);
		_packed_type = DefaultUnpacking<short>::get_default_packed_type();
	}
		break;
	case UShortElement: {
		_packed_format = DefaultUnpacking<unsigned short>::get_default_packed_format(
				num_channels, normalized_access);
		_packed_type = DefaultUnpacking<unsigned short>::get_default_packed_type();
	}
		break;
	case CharElement: {
		_packed_format = DefaultUnpacking<char>::get_default_packed_format(
				num_channels, normalized_access);
		_packed_type = DefaultUnpacking<char>::get_default_packed_type();
	}
		break;
	case UCharElement: {
		_packed_format = DefaultUnpacking<unsigned char>::get_default_packed_format(
				num_channels, normalized_access);
		_packed_type = DefaultUnpacking<unsigned char>::get_default_packed_type();
	}
		break;
	default: {
		assert(false && "unknown element type encountered");
	}
		break;
	}

}

void PackedFormat::set_packed_format(GLenum packed_format) {
  _packed_format = packed_format;
}

void PackedFormat::set_packed_type(GLenum packed_type) {
  _packed_type = packed_type;
}

// Packed setting getters.

bool PackedFormat::uses_normalized_access() {
  return _normalized_access;
}

GLenum PackedFormat::get_packed_format() {
  return _packed_format;
}

GLenum PackedFormat::get_packed_type() {
  return _packed_type;
}

// template instantiations

}
