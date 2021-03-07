#include "testtexture2.h"

#include <base/device/deviceheadersgl.h>

#include <base/memoryallocator/taggednew.h>

// Std headers.
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>

// Packed buffers.
#include <base/device/packedbuffers/packedtexture.h>

// Unpacked buffers.
#include <base/device/unpackedbuffers/texture.h>

#include <base/device/devicedebug.h>

using namespace ngs;

#define kWidth 2201
#define kHeight 1201
#define kNumChannels 4

TestTexture2::TestTexture2(ElementID element_id, bool normalized_access) :
		_element_id(element_id), _normalized_access(normalized_access), _packed_texture(
				NULL), _texture(NULL), _read_back_packed_texture(NULL) {
	create_packed_texture();
	create_texture();
	create_read_back_packed_texture();
	check_packed_chunk_equality_operator();
}

TestTexture2::~TestTexture2() {
	delete_ff(_packed_texture)
	;
	delete_ff(_texture)
	;
	delete_ff(_read_back_packed_texture)
	;
}

namespace {
template<class T>
void fill_bar(char* raw);
//template<class T>
//void fill_bar(char* raw) {
//	T* element = reinterpret_cast<T*>(raw);
//	for (size_t j = 0; j < 512 * 4; j++) {
//		*element = 1;
//		++element;
//	}
//}
template<>
void fill_bar<float>(char* raw) {
	float* element = reinterpret_cast<float*>(raw);
	for (size_t j = 0; j < kWidth * kNumChannels; j++) {
		*element = (float)rand()/(float)RAND_MAX - 0.5;
		++element;
	}
}
template<>
void fill_bar<half>(char* raw) {
	half* element = reinterpret_cast<half*>(raw);
	for (size_t j = 0; j < kWidth * kNumChannels; j++) {
		*element = (float)rand()/(float)RAND_MAX - 0.5;
		++element;
	}
}
template<>
void fill_bar<int>(char* raw) {
	int* element = reinterpret_cast<int*>(raw);
	for (size_t j = 0; j < kWidth * kNumChannels; j++) {
		*element =rand() - (RAND_MAX/2);
		++element;
	}
}
template<>
void fill_bar<unsigned int>(char* raw) {
	unsigned int* element = reinterpret_cast<unsigned int*>(raw);
	for (size_t j = 0; j < kWidth * kNumChannels; j++) {
		*element = rand();
		++element;
	}
}
template<>
void fill_bar<short>(char* raw) {
	short* element = reinterpret_cast<short*>(raw);
	for (size_t j = 0; j < kWidth * kNumChannels; j++) {
		*element = rand() - (RAND_MAX/2);
		++element;
	}
}
template<>
void fill_bar<unsigned short>(char* raw) {
	unsigned short* element = reinterpret_cast<unsigned short*>(raw);
	for (size_t j = 0; j < kWidth * kNumChannels; j++) {
		*element = rand();
		++element;
	}
}
template<>
void fill_bar<char>(char* raw) {
	char* element = reinterpret_cast<char*>(raw);
	for (size_t j = 0; j < kWidth * kNumChannels; j++) {
		*element = rand()-(RAND_MAX/2);
		++element;
	}
}
template<>
void fill_bar<unsigned char>(char* raw) {
	unsigned char* element = reinterpret_cast<unsigned char*>(raw);
	for (size_t j = 0; j < kWidth * kNumChannels; j++) {
		*element = rand();
		++element;
	}
}
}

void TestTexture2::create_packed_texture() {
	// Create a packed texture with a horizontal bar in it.
	_packed_texture = new_ff PackedTexture(_element_id);
	_packed_texture->reallocate(kWidth, kHeight, 1, kNumChannels);

	// Memset the elements to zero.
	char* raw = _packed_texture->get_pixel();
	memset(raw, 0, _packed_texture->get_chunk_geometry().get_num_bytes());

	// Fill in the texture.
	size_t element_size =
			_packed_texture->get_chunk_geometry().get_element_size_bytes();
	size_t row_size = element_size * kNumChannels * kWidth;
	for (size_t i = 0; i < kHeight; i++) {
		char* raw = _packed_texture->get_pixel(0, i, 0);
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

void TestTexture2::create_texture() {
	// Create our texture.
	_texture = new_ff Texture(0, *_packed_texture, _normalized_access);
	_texture->unbind(0);
}

void TestTexture2::create_read_back_packed_texture()
{
	// Download the texture to a packed texture.
	_texture->bind(0);
	_read_back_packed_texture = _texture->create_packed_texture();
}

void TestTexture2::check_packed_chunk_equality_operator()
{
	// Uncomment to force the two packed textures to be unequal.
	//*_packed_texture->get_pixel(50,50,0)= 'v';
	if (!(*_packed_texture == *_read_back_packed_texture)) {
		std::cerr << "packed textures are not equal\n";
		assert(false);
	}
}

