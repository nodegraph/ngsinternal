#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>
#include <base/device/packedbuffers/packeduniformbuffer.h>

#include <base/device/unpackedbuffers/uniformbuffer.h>

#include <base/device/unpackedbuffers/buffer.h>
#include <iostream>

namespace ngs {

#if GLES_MAJOR_VERSION >= 3


UniformBuffer::UniformBuffer()
    : Buffer(GL_UNIFORM_BUFFER) {
  clean_dirty_range();
}

UniformBuffer::~UniformBuffer() {
}

// Uniform Block Binding.

void UniformBuffer::bind_to_uniform_block_binding(GLuint uniform_block_binding_index, GLintptr offset, GLsizeiptr size) {
  gpu(glBindBufferRange(_target, uniform_block_binding_index, _name, offset, size));
}
void UniformBuffer::bind_to_uniform_block_binding(GLuint uniform_block_binding_index) {
  gpu(glBindBufferRange(_target, uniform_block_binding_index, _name, 0, _size_bytes));
}

// Loading.

void UniformBuffer::load_all(PackedUniformBuffer* packed_uniforms) {
  bind();
  load(packed_uniforms->get_byte_size(), packed_uniforms->get_raw());
}

void UniformBuffer::load_changed(PackedUniformBuffer* packed_uniforms) {
  if (packed_uniforms->get_byte_size() != get_size_bytes()) {
    // If the packed_uniforms size doesn't match us then load everything.
    load_all(packed_uniforms);
  } else {
    // If the uniform buffer size matches, then only load what's changed.
    if (is_dirty()) {
      bind();
      size_t min_dirty_byte = get_min_dirty_byte();
      size_t max_dirty_byte = get_max_dirty_byte();
      load_subsection(min_dirty_byte, max_dirty_byte-min_dirty_byte, &packed_uniforms->get_raw()[min_dirty_byte]);
    }
  }
  clean_dirty_range();
}

// Dirtiness Tracking.


bool UniformBuffer::is_dirty() {
  if (_max_dirty_byte>_min_dirty_byte) {
    return true;
  }
  return false;
}

void UniformBuffer::clean_dirty_range() {
  _min_dirty_byte = get_size_bytes();
  _max_dirty_byte = 0;
}

void UniformBuffer::augment_dirty_range(size_t min, size_t max) {
  if (min < _min_dirty_byte) {
    _min_dirty_byte = min;
  }
  if (max > _max_dirty_byte) {
    _max_dirty_byte = max;
  }
}


#endif

}
