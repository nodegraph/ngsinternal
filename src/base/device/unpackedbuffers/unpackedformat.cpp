#include <base/device/deviceheadersgl.h>
#include <base/device/unpackedbuffers/unpackedformat.h>
#include <base/device/unpackedbuffers/defaultunpacking.h>

#include <cassert>



namespace ngs {

UnpackedFormat::UnpackedFormat() {
	_target = GL_TEXTURE_2D;
	_unpacked_format = GL_RGBA;
	_normalized_access = false;
	_num_samples = 0;
	_fixed_sample_locations = true;
}

// Sets default format and type.
void UnpackedFormat::set_unpacked_defaults(ElementID element_id,
		int num_channels, int num_dimensions, bool normalized_access) {
	_normalized_access = normalized_access;

	switch (element_id) {
	case FloatElement: {
		_unpacked_format = DefaultUnpacking<float>::get_default_unpacked_format(
				num_channels, normalized_access);
		_target = DefaultUnpacking<float>::get_default_unpacked_target(
				num_dimensions);
	}
		break;
	case HalfElement: {
		_unpacked_format = DefaultUnpacking<half>::get_default_unpacked_format(
				num_channels, normalized_access);
		_target = DefaultUnpacking<half>::get_default_unpacked_target(
				num_dimensions);
	}
		break;
	case IntElement: {
		_unpacked_format = DefaultUnpacking<int>::get_default_unpacked_format(
				num_channels, normalized_access);
		_target = DefaultUnpacking<int>::get_default_unpacked_target(
				num_dimensions);
	}
		break;
	case UIntElement: {
		_unpacked_format =
				DefaultUnpacking<unsigned int>::get_default_unpacked_format(
						num_channels, normalized_access);
		_target = DefaultUnpacking<unsigned int>::get_default_unpacked_target(
				num_dimensions);
	}
		break;
	case ShortElement: {
		_unpacked_format = DefaultUnpacking<short>::get_default_unpacked_format(
				num_channels, normalized_access);
		_target = DefaultUnpacking<short>::get_default_unpacked_target(
				num_dimensions);
	}
		break;
	case UShortElement: {
		_unpacked_format =
				DefaultUnpacking<unsigned short>::get_default_unpacked_format(
						num_channels, normalized_access);
		_target = DefaultUnpacking<unsigned short>::get_default_unpacked_target(
				num_dimensions);
	}
		break;
	case CharElement: {
		_unpacked_format = DefaultUnpacking<char>::get_default_unpacked_format(
				num_channels, normalized_access);
		_target = DefaultUnpacking<char>::get_default_unpacked_target(
				num_dimensions);
	}
		break;
	case UCharElement: {
		_unpacked_format =
				DefaultUnpacking<unsigned char>::get_default_unpacked_format(
						num_channels, normalized_access);
		_target = DefaultUnpacking<unsigned char>::get_default_unpacked_target(
				num_dimensions);
	}
		break;
	default: {
	}
		break;
	}
}

bool UnpackedFormat::uses_normalized_access() const {
	return _normalized_access;
}

// Target
void UnpackedFormat::set_unpacked_target(GLenum unpacked_target) {
	_target = unpacked_target;
}

GLenum UnpackedFormat::get_unpacked_target() const {
	return _target;
}

// Format

void UnpackedFormat::set_unpacked_format(GLenum unpacked_format) {
	_unpacked_format = unpacked_format;
}

GLenum UnpackedFormat::get_unpacked_format() const {
	return _unpacked_format;
}

// Multisampling.

void UnpackedFormat::set_num_samples(int num) {
	_num_samples = num;
}

void UnpackedFormat::set_fixed_sample_locations(bool fixed) {
	_fixed_sample_locations = fixed;
}

int UnpackedFormat::get_num_samples() const {
	return _num_samples;
}

bool UnpackedFormat::get_fixed_sample_locations() const {
	return _fixed_sample_locations;
}

}
