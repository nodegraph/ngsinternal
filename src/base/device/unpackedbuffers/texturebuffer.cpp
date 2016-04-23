#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/device/unpackedbuffers/texturebuffer.h>
#include <base/device/unpackedbuffers/buffer.h>



#include <iostream>
#include <cassert>

namespace ngs {

#if GLES_MAJOR_VERSION >= 100

TextureBuffer::TextureBuffer(GLenum unpacked_format)
    : _target(GL_TEXTURE_BUFFER),
      _unpacked_format(unpacked_format) {
  create_name();
}

TextureBuffer::~TextureBuffer() {
  remove_name();
}

void TextureBuffer::set_unpacked_format(GLenum unpacked_format) {
  _unpacked_format = unpacked_format;
}

void TextureBuffer::bind(unsigned int texture_unit_number) {
  // Bind.
  Device::disable_texture_targets(texture_unit_number);
  Device::enable_texture_target(texture_unit_number, _target);
  gpu(glBindTexture(_target, _name));

  /*
   gpu(glPixelStorei(GL_PACK_ALIGNMENT,4));
   gpu(glPixelStorei(GL_PACK_ROW_LENGTH,0));
   gpu(glPixelStorei(GL_UNPACK_ALIGNMENT,4));
   gpu(glPixelStorei(GL_UNPACK_ROW_LENGTH,0));
   */
}

void TextureBuffer::unbind(unsigned int texture_unit_number) {
  Device::disable_texture_targets(texture_unit_number);
  gpu(glBindTexture(_target, 0));
}

void TextureBuffer::load(unsigned int texture_unit_number, Buffer& buffer) {
  bind(texture_unit_number);
  load_helper(buffer);
}

bool TextureBuffer::name_is_valid(GLuint name) {
  GLboolean success;
  gpu(success = glIsTexture(name));
  if (success) {
    return true;
  }
  return false;
}

void TextureBuffer::create_name() {
  gpu(glGenTextures(1, &_name));
  if (_name == 0) {
    std::cerr
        << "Error: memory error is likely. unable to create a new_ texture name.\n";
    assert(false);
  }
}

void TextureBuffer::remove_name() {
  if (_name != 0) {
    gpu(glDeleteTextures(1, &_name));
  }
  _name = 0;
}

/*
 Only color formats are acceptable in buffer textures. These formats can be 1, 2, and 4 channels (with 3 channels in OpenGL 4.0 or with ARB_texture_buffer_object_rgb32), so only RED, RG, and RGBA (or RGB with GL 4.0) are acceptable.
 Only formats with an explicit size are allowed. GL_RGBA is not sufficient for a buffer texture; the user must use GL_RGBA8 or GL_RGBA16. The sizes can only be 8, 16, or 32; smaller per-component sizes are not allowed. Also, the more unusual formats like GL_RGB10_A2 are not supported in buffer textures.
 The format can be any of the various types, like unsigned normalized, floating-point, signed integral, etc. GL_RGBA8UI would be 4-bytes per color, with
 The byte-order for each component is native for the CPU platform in use. So if the user's code is running on a little-endian, then the floats, 32-bit integers, or 16-bit integers are stored in little-endian order. However the byte-order among components is always RGBA in memory, regarldess of the endianess. 
 */
void TextureBuffer::load_helper(Buffer& buffer) {
  gpu(glTexBuffer(_target, _unpacked_format, buffer.get_name()););
}

#endif

}
