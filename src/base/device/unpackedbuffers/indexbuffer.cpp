#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>
#include <base/device/unpackedbuffers/indexbuffer.h>
#include <base/device/unpackedbuffers/buffer.h>



#include <iostream>
#include <cassert>

namespace ngs {

IndexBuffer::IndexBuffer(GLenum primitive_mode, GLenum index_type)
    : Buffer(GL_ELEMENT_ARRAY_BUFFER),
      _primitive_mode(primitive_mode),
      _index_type(index_type) {
}

IndexBuffer::~IndexBuffer() {
}

size_t IndexBuffer::get_num_indices() {
  switch (_index_type) {
    case GL_UNSIGNED_BYTE: {
      return get_size_bytes() / sizeof(unsigned char);
    }
      break;
    case GL_UNSIGNED_SHORT: {
      return get_size_bytes() / sizeof(unsigned short);
    }
      break;
    case GL_UNSIGNED_INT: {
      return get_size_bytes() / sizeof(unsigned int);
    }
      break;
  }
  assert(false);
  return 0;
}

void IndexBuffer::draw() {
  GLsizei num_indices = get_num_indices();
  bind();
  gpu(glDrawElements(_primitive_mode, num_indices, _index_type, (void*) 0));
}

void IndexBuffer::draw(GLsizei num_indices) {
  bind();
  gpu(glDrawElements(_primitive_mode, num_indices, _index_type, (void*) 0));
}

void IndexBuffer::draw_instances(GLsizei num_instances) {
  GLsizei num_indices = get_num_indices();
  bind();

#if (GLES_MAJOR_VERSION >= 3) && (GLES_MINOR_VERSION >=2)
	gpu(glDrawElementsInstancedBaseVertex(_primitive_mode, num_indices, _index_type, (void*)0, num_instances, 0));
#elif (GLES_MAJOR_VERSION >= 3) && (GLES_MINOR_VERSION >=0)
  gpu(glDrawElementsInstanced(_primitive_mode, num_indices, _index_type, (void*)0, num_instances));
#elif (GLES_MAJOR_VERSION >= 2)
  #if GLES_USE_ANGLE == 1
    gpu(glDrawElementsInstancedANGLE(_primitive_mode, num_indices, _index_type, (void*) 0, num_instances));
  #else
    gpu(glDrawElementsInstancedEXT(_primitive_mode, num_indices, _index_type, (void*)0, num_instances));
  #endif
#endif
}

}
