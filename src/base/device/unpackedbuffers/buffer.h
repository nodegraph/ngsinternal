#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>

#include <cstddef>

// Usage parameter:  eg. GL_STREAM_DRAW
// 
// STATIC: The user will set the data once.
// DYNAMIC: The user will set the data occasionally.
// STREAM: The user will be changing the data after every use. Or almost every use. 
//
// DRAW: The user will be writing data to the buffer, but the user will not read it.
// READ: The user will not be writing data, but the user will be reading it back.
// COPY: The user will be neither writing nor reading the data. 

//           DRAW              COPY              READ
//STATIC     GL_STATIC_DRAW    GL_STATIC_COPY    GL_STATIC_READ
//STREAM     GL_STREAM_DRAW    GL_STREAM_COPY    GL_STREAM_READ
//DYNAMIC    GL_DYNAMIC_DRAW   GL_DYNAMIC_COPY   GL_DYNAMIC_READ



namespace ngs {

class PackedTexture;

// T is the unpacked element type.
class DEVICE_EXPORT Buffer {
 public:
  Buffer(GLenum target);
  virtual ~Buffer();

  virtual size_t get_size_bytes();

  // Loading.
  virtual void load_null(size_t size_bytes) {load(size_bytes,NULL);}
  virtual void load(size_t size_bytes, const void* data);
  virtual void load_subsection(size_t offset, size_t size_bytes,
                               const void* data) const;

  // Readback.
#if GLES_MAJOR_VERSION >= 100
  void fill_packed_texture(PackedTexture& chunk);
#endif

  // Buffer mapping.
#if GLES_MAJOR_VERSION >= 3
  virtual void* map_buffer_range(int offset, int length,
                                 unsigned int bit_access);
  virtual void unmap_buffer_range();
#endif

  // Binding.
  virtual GLuint get_name() const;
  virtual void bind();
  virtual void unbind();

 protected:
  void create_name();
  void remove_name();

  // Name.
  GLuint _name;

  // Binding target.
  GLenum _target;

  // Usage hints.
  GLenum _buffer_usage;

  // Size of the buffer.
  size_t _size_bytes;

};

}
