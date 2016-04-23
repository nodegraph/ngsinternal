#pragma once

#include <base/device/device_export.h>
#include <base/device/packedbuffers/chunkgeometry.h>
#include <base/device/packedbuffers/elementid.h>

#include <vector>
#include <string>


struct FIBITMAP;

/*!
 To have opengl load data from a data pointer that you own set: \n
 -borrow_data to false and \n
 -data to your data pointer.\n
 */

namespace ngs {

template <class T>
bool is_nan(T* value);

// T is the element type: eg float, short, etc
class DEVICE_EXPORT PackedTexture {
public:
	PackedTexture(ElementID id);
	explicit PackedTexture(const ChunkGeometry& geometry);
	explicit PackedTexture(const char* src_data, const  ChunkGeometry* src_data_geometry);
	PackedTexture(const PackedTexture& other); // copy constructor.
	~PackedTexture();

	// Composite Objects.
	ChunkGeometry& get_chunk_geometry();
	const ChunkGeometry& get_chunk_geometry() const;

	// Set data.
	void set_data(const char* src_data, const  ChunkGeometry* src_data_geometry);
	const char* get_data() const;
	char* get_data();

	// Memory.
	bool reallocate();
	bool reallocate(int width, int height, int depth, int num_channels);
	char* get_pixel(int col = 0, int row = 0, int plane = 0) const;

	// Operators.
	bool operator==(const PackedTexture& other) const;
	bool operator!=(const PackedTexture& other) const;
	PackedTexture& operator=(const PackedTexture& other);

  // Conversion routines.
  static PackedTexture* convert_to_float_packed_texture(PackedTexture* packed_chunk);
  static PackedTexture* _convert_to_float_packed_texture(PackedTexture* packed_chunk, bool swizzle_channels);

private:



	// Equality check parameterized for the different element types.
	template<class T>
	bool equals(const PackedTexture& right_texture) const;

	// The raw data.
	char* _data;

	// Our chunk geometry.
	ChunkGeometry _chunk_geometry;
};

}
