#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>
#include <base/device/packedbuffers/chunkgeometry.h>


namespace ngs {

/*
 These defaults will load packed data from the system memory into gpu memory.
 These defaults expect that the underlying data type for the element doesn't change
 between the system and gpu memory. For example chars or ints will get loaded
 as chars or ints into gpu memory.

 We also parameterize this unpacking based on whether we want to access the
 texture values as normalized values in glsl or not.

 This has leads to special cases for float, int, and unsigned int textures.
 When floats are loaded to floats, they are always accessed as unnormalized values.
 When u/ints are loaded to u/ints, they are always accessed as unnormalized values also.
 This is what I have gathered from looking at the available constants from the glew.h
 header file.

 For other bit sizes we have:
 gl_rgba16 -> means normalized unsigned integer. (0 to 1)
 gl_rgba16_snorm -> means normalized signed integer. (-1 to 1)

 But not for 32 bit.

 I also thought about then allowing floats to get uploaded to gpu for int like access
 or ints to get uploaded to gpu as floats for normalized access of ints.
 However many of these conversions lead to invalid operation errors when loading
 or downloading from the gpu.

 So if you want to access int data as normalized value you should convert it to a
 float point number between 0 and 1 before loading. 
 This is not a big deal as they are both using 32 bits.

 If you want to access float data as normalize you should just normalize it before
 loading to the gpu.

 */

// ----------------------------------------------------------------------
// Holds default formats for use when loading packed chunks into unpacked chunks.
// ----------------------------------------------------------------------
template<class PackedElementType>
class DEVICE_EXPORT DefaultUnpacking {
 public:
  // Deduce default packed formats.
  static GLenum get_default_packed_format(int num_channels,
                                          bool normalized_access);
  static GLenum get_default_packed_type();

  // Deduce unpacked default formats.
  // Gets the recommended unpacked formats for opengl.  
  // Unpack refers to the configuration in gpu memory.
  static GLenum get_default_unpacked_format(int num_channels,
                                            bool normalized_access);
  static GLenum get_default_unpacked_target(int num_dimensions);

  // TextureObject and BufferObject settings.
  static GLint get_default_mip_map_level() {
    return 0;
  }
  static GLenum get_default_buffer_usage() {
    return GL_STREAM_DRAW;
  }
  ;
};

}
