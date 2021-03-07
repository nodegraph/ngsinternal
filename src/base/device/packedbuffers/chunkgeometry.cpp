#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/device/packedbuffers/chunkgeometry.h>
#include <boost/math/special_functions/round.hpp>

#include <cmath>
#include <cassert>
#include <iostream>

namespace ngs {

ChunkGeometry::ChunkGeometry(ElementID id) :
		// Dimensions. - default is a 512 x 512 allocation.
		_x(0), _y(0), _width(512), _height(512), _depth(1), _num_channels(4), _dim_interpretation(2),
		// Element Type.
		_element_id(id), // float is the default
		// Size and Alignment
		_alignment_bytes(1)
{
}

int ChunkGeometry::get_element_size_bytes(ElementID elementId){
	switch (elementId) {
	case FloatElement:
		return sizeof(float);
	case HalfElement:
		return sizeof(half);
	case IntElement:
		return sizeof(int);
	case UIntElement:
		return sizeof(unsigned int);
	case ShortElement:
		return sizeof(short);
	case UShortElement:
		return sizeof(unsigned short);
	case CharElement:
		return sizeof(char);
	case UCharElement:
		return sizeof(unsigned char);
	default:
		assert(false && "an unknown element type was encountered");
	}
	return 0;
}

int ChunkGeometry::get_element_size_bytes() const {
	return get_element_size_bytes(_element_id);
}

bool ChunkGeometry::element_is_signed() const {
	switch (_element_id) {
	case FloatElement:
		return true;
	case HalfElement:
		return true;
	case IntElement:
		return true;
	case UIntElement:
		return false;
	case ShortElement:
		return true;
	case UShortElement:
		return false;
	case CharElement:
		return true;
	case UCharElement:
		return false;
	default:
		assert(false && "an unknown element type was encountered");
	}
	return false;
}


// Size related getters.
int ChunkGeometry::get_num_elements() const {
  return _width * _height * _depth * _num_channels;
}

int ChunkGeometry::get_num_bytes() const {
	return get_width_stride_bytes()*_height*_depth;
}

int ChunkGeometry::get_width_stride_bytes() const {
	if (get_element_size_bytes() >= _alignment_bytes) {
		return _num_channels * _width * get_element_size_bytes();
	}

	float a = static_cast<float>(_alignment_bytes);
	float s = static_cast<float>(get_element_size_bytes());
	float n = static_cast<float>(_num_channels);
	float l = static_cast<float>(_width);
	float stride = boost::math::round((a / s) * ceilf(n * l * s / a) * s);
	return static_cast<int>(stride);
}

int ChunkGeometry::get_height_stride_bytes() const {
	return _height * get_width_stride_bytes();
}

int ChunkGeometry::get_byte_offset(int column, int row, int plane) const {
	return (plane * get_height_stride_bytes()) + (row * get_width_stride_bytes())
			+ (column * _num_channels * get_element_size_bytes());
}

// Operators.
bool ChunkGeometry::operator!=(const ChunkGeometry& right) const {
	return !operator==(right);
}

bool ChunkGeometry::operator==(const ChunkGeometry& right) const {
	if (_x != right._x) {
		return false;
	}
	if (_y != right._y) {
		return false;
	}
	if (_width != right._width) {
		return false;
	}
	if (_height != right._height) {
		return false;
	}
	if (_depth != right._depth) {
		return false;
	}
	if (_num_channels != right._num_channels) {
		return false;
	}
	if (_dim_interpretation != right._dim_interpretation) {
		return false;
	}
	if (_element_id != right._element_id) {
		return false;
	}
	if (_alignment_bytes != right._alignment_bytes) {
		return false;
	}
	return true;
}

}

