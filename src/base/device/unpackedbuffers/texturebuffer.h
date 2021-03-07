#pragma once
#include <base/device/device_export.h>


/*
 TextureBuffers are sometimes called "Buffer Textures". 
 These are actually one dimensional textures which get their storage from buffer objects.
 Note they are not buffer objects! They get their storage from buffer objects.

 In this sense the TextureBuffer is definitely not a Buffer and not quite a texture.
 This is why this class doesn't inherit from anything.

 TextureBuffers can be access from GLSL kind of like regular textures except there is not mip mapping, or interpoled
 access to the buffer. Also the normalized indexing regular textures is unavailable. TextureBuffers are just
 accessed like 1-D arrays with integral indexes.
 */

/*
 In GLSL, buffer textures can only be accessed with the texelFetch function. 
 This function takes pixel offsets into the texture rather than normalized texture coordinates. 
 The sampler types for buffer textures are gsamplerBuffer. 
 */

namespace ngs {

class Buffer;

#if GLES_MAJOR_VERSION >= 100

class TextureBuffer {
 public:
  TextureBuffer(GLenum unpacked_format);
  virtual ~TextureBuffer();

  virtual void set_unpacked_format(GLenum unpacked_format);
  virtual void load(unsigned int texture_unit_number, Buffer& buffer);

  virtual void bind(unsigned int texture_unit_number);
  virtual void unbind(unsigned int texture_unit_number);

 private:

  bool name_is_valid(GLuint name);
  void create_name();
  void remove_name();
  void load_helper(Buffer& buffer);

  // Name of object.
  GLuint _name;

  // Binding target.
  GLenum _target;

  // Unpacked internal format
  GLenum _unpacked_format;

};

#endif

}
