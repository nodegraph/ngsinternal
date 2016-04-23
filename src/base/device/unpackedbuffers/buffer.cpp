#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/device/unpackedbuffers/buffer.h>
#include <base/device/packedbuffers/packedtexture.h>

#include <iostream>
#include <cassert>

namespace ngs {

// Target determines what kind of buffer object this will be.
Buffer::Buffer(GLenum target)
    : _buffer_usage(GL_DYNAMIC_DRAW), //GL_STREAM_DRAW
      _target(target),
      _size_bytes(0) {
  // Create and load.
  create_name();
}

Buffer::~Buffer() {
  remove_name();
}

size_t Buffer::get_size_bytes() {
  return _size_bytes;
}

void Buffer::create_name() {
  gpu(glGenBuffers(1, &_name));
  if (_name == 0) {
    std::cerr
        << "Error: insufficient memory is likely. unable to create a buffer object name.\n";
    assert(false);
  }
}

void Buffer::remove_name() {
  if (_name != 0) {
    gpu(glDeleteBuffers(1, &_name));
  }
  _name = 0;
}

GLuint Buffer::get_name() const {
  return _name;
}

void Buffer::load(size_t size_bytes, const void* data) {
  if (
      (_target == GL_ARRAY_BUFFER)
      || (_target == GL_ELEMENT_ARRAY_BUFFER)
#if GLES_MAJOR_VERSION >= 3
      || (_target == GL_UNIFORM_BUFFER)
      || (_target == GL_TRANSFORM_FEEDBACK)
#endif
#if GLES_MAJOR_VERSION >= 100
      || (_target == GL_TEXTURE_BUFFER)
#endif
      ) {
    gpu(glBufferData(_target, size_bytes, data, _buffer_usage););
    _size_bytes = size_bytes;
  } else {
    std::cerr
        << "Error: attempt to load an unhandled buffer type was encountered.\n";
  }
}

void Buffer::load_subsection(size_t offset, size_t size_bytes,
                             const void* data) const {
  gpu(glBufferSubData(_target, offset, size_bytes, data));
}

#if GLES_MAJOR_VERSION >= 100
void Buffer::fill_packed_texture(PackedTexture& chunk) {
  size_t num_bytes = chunk.get_chunk_geometry().get_num_bytes();
  if (
      (_target == GL_ARRAY_BUFFER)
      || (_target == GL_ELEMENT_ARRAY_BUFFER)
      || (_target == GL_UNIFORM_BUFFER)
      || (_target == GL_TRANSFORM_FEEDBACK)
#if GLES_MAJOR_VERSION >= 100
      || (_target == GL_TEXTURE_BUFFER)
#endif
      ) {
    gpu(glGetBufferSubData(_target, 0, num_bytes, chunk.get_pixel()););
  } else {
    std::cerr << "Error: attempt to pack an unhandled buffer type.\n";
  }
}
#endif

// IPackedChunk overrides.
#if GLES_MAJOR_VERSION >= 3
void* Buffer::map_buffer_range(int offset, int length,
                               unsigned int bit_access) {
  void* data = glMapBufferRange(_target, offset, length, bit_access);
  gpu(1);
  return data;
}
void Buffer::unmap_buffer_range() {
  gpu(glUnmapBuffer(_target));
}
#endif

void Buffer::bind() {
  // Bind.
  gpu(glBindBuffer(_target,_name));
}

void Buffer::unbind() {
  gpu(glBindBuffer(_target,0));
}



}

