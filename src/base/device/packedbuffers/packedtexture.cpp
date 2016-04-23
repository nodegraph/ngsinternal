#include <base/device/packedbuffers/packedtexture.h>
#include <base/device/unpackedbuffers/defaultunpacking.h>
#include <base/memoryallocator/taggednew.h>

#include <iostream>
#include <cmath>
#include <cstring>
#include <cassert>

//#include <FreeImage.h>

namespace ngs {

// -----------------------------------------------------------------------------------------------------------------------------------
// Constructors which create internal data that we own. 
// -----------------------------------------------------------------------------------------------------------------------------------
PackedTexture::PackedTexture(ElementID id)
    : _chunk_geometry(id),
      _data(NULL) {
  reallocate();
}

PackedTexture::PackedTexture(const ChunkGeometry& geometry)
    : _chunk_geometry(geometry),
      _data(NULL) {
  reallocate();
}

PackedTexture::PackedTexture(const char* src_data, const ChunkGeometry* src_data_geometry)
    : _chunk_geometry(*src_data_geometry),
      _data(NULL) {
  set_data(src_data, src_data_geometry);
}

PackedTexture::PackedTexture(const PackedTexture& other):
  _chunk_geometry(other._chunk_geometry),
  _data(NULL){
  set_data(other.get_data(), &other._chunk_geometry);
}

PackedTexture::~PackedTexture() {
	delete_array_ff(_data)
	;
}

ChunkGeometry& PackedTexture::get_chunk_geometry() {
	return _chunk_geometry;
}

const ChunkGeometry& PackedTexture::get_chunk_geometry() const {
	return _chunk_geometry;
}

void PackedTexture::set_data(const char* src_data, const  ChunkGeometry* src_data_geometry) {
  // Make sure the geometries match.
  if ((!_data) || (_chunk_geometry!=*src_data_geometry)) {
    _chunk_geometry=*src_data_geometry;
    reallocate();
  }

	// If the data is not empty we should copy it.
	if (src_data) {
		memcpy(_data, src_data, _chunk_geometry.get_num_bytes());
	}
}

const char* PackedTexture::get_data() const {
	return _data;
}

char* PackedTexture::get_data() {
	return _data;
}

bool PackedTexture::reallocate() {
	// Delete the old data.
	delete_array_ff(_data);
	_data = NULL;

	// Here we create our data.
	_data = new_ff char[_chunk_geometry.get_num_bytes()]();
	return true;
}

bool PackedTexture::reallocate(int width, int height, int depth,
		int num_channels) {

	// Set the next dimension.
	_chunk_geometry.set_width(width);
	_chunk_geometry.set_height(height);
	_chunk_geometry.set_depth(depth);
	_chunk_geometry.set_num_channels(num_channels);

	return reallocate();
}

char* PackedTexture::get_pixel(int col, int row, int plane) const {
	return _data + _chunk_geometry.get_byte_offset(col, row, plane);
}

PackedTexture& PackedTexture::operator=(const PackedTexture& other) {
  set_data(other.get_data(),&other.get_chunk_geometry());
  return *this;
}

bool PackedTexture::operator!=(const PackedTexture& other) const {
	return !operator==(other);
}

bool PackedTexture::operator==(const PackedTexture& right_texture) const {
	switch (_chunk_geometry.get_element_id()) {
	case FloatElement:
		return equals<float>(right_texture);
	case HalfElement:
		return equals<half>(right_texture);
	case IntElement:
		return equals<int>(right_texture);
	case UIntElement:
		return equals<unsigned int>(right_texture);
	case ShortElement:
		return equals<short>(right_texture);
	case UShortElement:
		return equals<unsigned short>(right_texture);
	case CharElement:
		return equals<char>(right_texture);
	case UCharElement:
		return equals<unsigned char>(right_texture);
	default:
	  std::cerr << "error: unknown element type was encountered\n";
		assert(false);
	}
	return false;
}

template<class T>
bool PackedTexture::equals(const PackedTexture& right_texture) const {
	// ChunkGeometrys should be equal.
	if (_chunk_geometry != (right_texture.get_chunk_geometry())) {
		return false;
	}

	// Check the pixel data.
	for (int d = 0; d < _chunk_geometry.get_depth(); d++) {
		for (int h = 0; h < _chunk_geometry.get_height(); h++) {
			// Get pointers to the start of the rows.
			T* my_pixel = reinterpret_cast<T*>(_data
					+ _chunk_geometry.get_byte_offset(0, h, d));
			const T* your_pixel =
					reinterpret_cast<const T*>(right_texture.get_data()
							+ right_texture.get_chunk_geometry().get_byte_offset(
									0, h, d));
			// Loop thru pixels in the rows.
			for (int w = 0; w < _chunk_geometry.get_width() * _chunk_geometry.get_num_channels(); w++) {
				// consider two nan's to be equal.
				if (is_nan(my_pixel[w]) && is_nan(your_pixel[w])) {
					continue;
				}
				if (my_pixel[w] != your_pixel[w]) {
					std::cerr << "pixel diff at: [" << w << "," << h << "] values: " << my_pixel[w] << " vs " << your_pixel[w] << " diff " << (my_pixel[w] - your_pixel[w]) << "\n";
					assert(false);
					return false;
				}
			}
		}
	}
	return true;
}

PackedTexture* PackedTexture::convert_to_float_packed_texture(PackedTexture* packed_texture)
{
	return _convert_to_float_packed_texture(packed_texture,false);
}

namespace {

template <class FromType, class ToType>
void transform_element_type(PackedTexture* src_packed, int max_depth, PackedTexture* dest_packed, bool swizzle_channels, float factor){
    // Convert the chunk data to float.
	FromType* src;
    ToType* dest;
    for(int z=0; z<max_depth; z++)
    {
        for(int y=0; y<src_packed->get_chunk_geometry().get_height(); y++)
        {
            // Update pointer to beginning of row.
            src=reinterpret_cast<FromType*>(src_packed->get_pixel(0,y,z));
            dest=reinterpret_cast<ToType*>(dest_packed->get_pixel(0,y,z));

            // Fill in the row.
            int num_channels=src_packed->get_chunk_geometry().get_num_channels();

            if(swizzle_channels && ( (num_channels==3)||(num_channels==4) ) )
            {
                if(num_channels==3)
                {
                    for(int x=0; x<src_packed->get_chunk_geometry().get_width(); x++)
                    {
                        // BGR -> RGB.
                        dest[0]=static_cast<ToType>(src[2])/factor;
                        dest[1]=static_cast<ToType>(src[1])/factor;
                        dest[2]=static_cast<ToType>(src[0])/factor;
                        // Update pointers.
                        dest+=3;
                        src+=3;
                    }
                }
                else if(num_channels==4)
                {
                    for(int x=0; x<src_packed->get_chunk_geometry().get_width(); x++)
                    {
                        // BGRA -> RGBA.
                        dest[0]=static_cast<ToType>(src[2])/factor;
                        dest[1]=static_cast<ToType>(src[1])/factor;
                        dest[2]=static_cast<ToType>(src[0])/factor;
                        dest[3]=static_cast<ToType>(src[3])/factor;
                        // Update pointers.
                        dest+=4;
                        src+=4;
                    }
                }
            }
            else
            {
                for(int x=0; x<src_packed->get_chunk_geometry().get_width()*num_channels; x++)
                {
                    *dest=static_cast<ToType>(*src)/factor;
                    ++dest;
                    ++src;
                }
            }
        }
    }
}

}


// If swizzle channels is true the following channels are moved around.
// Low memory address -->  High memory address.
// input: BGRA (u/short/char) -> output: RGBA (float)
// input: BGR  (u/short/char) -> output: RGB (float)


PackedTexture* PackedTexture::_convert_to_float_packed_texture(PackedTexture* src_packed,bool swizzle_channels)
{
	// Return right away if the src packed texture is already float.
	if(src_packed->get_chunk_geometry().get_element_id() == FloatElement) {
		return NULL;
	}

    // Create the float packed chunk.
    PackedTexture* dest_packed=new_ff PackedTexture(FloatElement);
    dest_packed->get_chunk_geometry().set_x(src_packed->get_chunk_geometry().get_x());
    dest_packed->get_chunk_geometry().set_y(src_packed->get_chunk_geometry().get_y());
    dest_packed->get_chunk_geometry().set_width(src_packed->get_chunk_geometry().get_width());
    dest_packed->get_chunk_geometry().set_height(src_packed->get_chunk_geometry().get_height());
    dest_packed->get_chunk_geometry().set_depth(src_packed->get_chunk_geometry().get_depth());
    dest_packed->get_chunk_geometry().set_num_channels(src_packed->get_chunk_geometry().get_num_channels());
    dest_packed->get_chunk_geometry().set_alignment_bytes(src_packed->get_chunk_geometry().get_alignment_bytes());


    // Generalize the depth for looping.
    int depth(0);
    if(src_packed->get_chunk_geometry().get_depth()==0)
    {
        depth=1;
    }

    // Get the integer to float conversion factor.
    float factor(1);
    if(src_packed->get_chunk_geometry().element_is_signed())
    {
        factor=pow(2.0f, src_packed->get_chunk_geometry().get_element_size_bytes()*8-1) - 1;
    }
    else
    {
        factor=pow(2.0f, src_packed->get_chunk_geometry().get_element_size_bytes()*8) - 1;
    }

	// Convert the chunk data to float.
	switch (src_packed->get_chunk_geometry().get_element_id()) {
	case FloatElement:
		delete_ff(dest_packed);
		assert(false); // This case should never get hit.
		return NULL;
		break;
	case HalfElement:
		transform_element_type<half,float>(src_packed, depth, dest_packed,
				swizzle_channels, factor);
		break;
	case IntElement:
		transform_element_type<int,float>(src_packed, depth, dest_packed,
				swizzle_channels, factor);
		break;
	case UIntElement:
		transform_element_type<unsigned int,float>(src_packed, depth,
				dest_packed, swizzle_channels, factor);
		break;
	case ShortElement:
		transform_element_type<short,float>(src_packed, depth, dest_packed,
				swizzle_channels, factor);
		break;
	case UShortElement:
		transform_element_type<unsigned short,float>(src_packed, depth,
				dest_packed, swizzle_channels, factor);
		break;
	case CharElement:
		transform_element_type<char,float>(src_packed, depth, dest_packed,
				swizzle_channels, factor);
		break;
	case UCharElement:
		transform_element_type<unsigned char,float>(src_packed, depth,
				dest_packed, swizzle_channels, factor);
		break;
	}


    return dest_packed;
}

}
