#include "testtexture.h"

#include <base/device/deviceheadersgl.h>

#include <base/memoryallocator/taggednew.h>

// Std headers.
#include <iostream>
#include <vector>
#include <cstring>

// Packed buffers.
#include <base/device/packedbuffers/packedtexture.h>

// Unpacked buffers.
#include <base/device/unpackedbuffers/texture.h>

#include <base/device/devicedebug.h>

namespace {

const int num_channels = 4;

}

using namespace ngs;


TestTexture::TestTexture(ElementID element_id, bool normalized_access) :
		_element_id(element_id), _normalized_access(normalized_access), _packed_texture(
				NULL), _texture(NULL), _read_back_packed_texture(NULL) {
  gpu();
	create_packed_texture();
	gpu();
	create_texture();
	create_read_back_packed_texture();
	check_read_back_packed_texture();
	check_packed_chunk_equality_operator();
}

TestTexture::~TestTexture() {
	delete_ff(_packed_texture)
	;
	delete_ff(_texture)
	;
	delete_ff(_read_back_packed_texture)
	;
}

namespace {
template<class T>
void fill_bar(char* raw) {
	T* element = reinterpret_cast<T*>(raw);
	for (size_t j = 0; j < 512 * num_channels; j++) {
		*element = T(1);
		++element;
	}
}
}

void TestTexture::create_packed_texture() {
	// Create a packed texture with a horizontal bar in it.
	_packed_texture = new_ff PackedTexture(_element_id);
	_packed_texture->reallocate(512, 512, 1, num_channels);

	// Memset the elements to zero.
	char* raw = _packed_texture->get_pixel();
	memset(raw, 0, _packed_texture->get_chunk_geometry().get_num_bytes());

	// Fill in the horizontal bar.
	size_t element_size =
			_packed_texture->get_chunk_geometry().get_element_size_bytes();

	size_t row_size = element_size * num_channels * 512;
	for (size_t i = 0; i < 20; i++) {
		char* raw = _packed_texture->get_pixel(0, 256 + i, 0);
		switch (_element_id) {
		case FloatElement:
			fill_bar<float>(raw);
			break;
		case HalfElement:
			fill_bar<half>(raw);
			break;
		case IntElement:
			fill_bar<int>(raw);
			break;
		case UIntElement:
			fill_bar<unsigned int>(raw);
			break;
		case ShortElement:
			fill_bar<short>(raw);
			break;
		case UShortElement:
			fill_bar<unsigned short>(raw);
			break;
		case CharElement:
			fill_bar<char>(raw);
			break;
		case UCharElement:
			fill_bar<unsigned char>(raw);
			break;
		}
	}
}

void TestTexture::create_texture() {
  gpu();
	// Create our texture.
	_texture = new_ff Texture(0, *_packed_texture, _normalized_access);
	_texture->unbind(0);
}


void TestTexture::create_read_back_packed_texture()
{
	// Download the texture to a packed texture.
	_texture->bind(0);
	_read_back_packed_texture = _texture->create_packed_texture();
	_texture->unbind(0);
}

namespace {

template<class T>
void check_bar(char* raw);

template<class T>
void check_bar(char* raw) {
	T* element = reinterpret_cast<T*>(raw);
	for (size_t j = 0; j < 512 * num_channels; j++) {
		T expected(1);
		expected -= *element;
		if (expected != 0) { // Note we're doing an exact match even between floating point types.
			std::cerr << "Error: unexpected contents in input texture.\n";
			assert(false);
		}
		++element;
	}
}
}

void TestTexture::check_read_back_packed_texture() {

	// Check the horizontal bar.
	size_t element_size =
			_read_back_packed_texture->get_chunk_geometry().get_element_size_bytes();
	size_t row_size = element_size * num_channels * 512;

	for (size_t i = 0; i < 20; i++) {
		char* raw = _read_back_packed_texture->get_pixel(0, 256 + i, 0);
		switch (_element_id) {
		case FloatElement:
			check_bar<float>(raw);
			break;
		case HalfElement:
			check_bar<half>(raw);
			break;
		case IntElement:
			check_bar<int>(raw);
			break;
		case UIntElement:
			check_bar<unsigned int>(raw);
			break;
		case ShortElement:
			check_bar<short>(raw);
			break;
		case UShortElement:
			check_bar<unsigned short>(raw);
			break;
		case CharElement:
			check_bar<char>(raw);
			break;
		case UCharElement:
			check_bar<unsigned char>(raw);
			break;
		default:
			assert(false && "unknown element type encountered");
		}
	}
}

void TestTexture::check_packed_chunk_equality_operator()
{
	if (!(*_packed_texture == *_read_back_packed_texture)) {
		std::cerr << "packed textures are not equal\n";
		assert(false);
	}
}

