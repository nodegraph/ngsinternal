#include <base/device/unpackedbuffers/vertexbuffer.h>
#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/device/geometry/elementsource.h>

#include <iostream>
#include <functional>


namespace ngs {

VertexBuffer::VertexBuffer()
    : Buffer(GL_ARRAY_BUFFER) {
}

VertexBuffer::~VertexBuffer() {
}

void VertexBuffer::set_elements_to_feed_to_float_vertex_attribute(const ElementSource& element_source, GLuint attr_loc) {
  VoidCallback f = std::bind(VertexBuffer::__set_elements_to_feed_to_float_vertex_attribute, element_source, attr_loc);
  _positioners[attr_loc] = f;
  f();
}

#if GLES_MAJOR_VERSION >= 3

void VertexBuffer::set_elements_to_feed_to_integer_vertex_attribute(const ElementSource& element_source, GLuint attr_loc) {
  VoidCallback f = std::bind(VertexBuffer::__set_elements_to_feed_to_integer_vertex_attribute, element_source, attr_loc);
  // Add it to our positioners.
  _positioners[attr_loc] = f;

  // perform the positioning.
  f();
}

#endif



void VertexBuffer::set_vertex_divisor(GLuint attr_index, GLuint divisor) {
#if (GLES_MAJOR_VERSION == 2)
  #if GLES_USE_ANGLE == 1
    gpu(glVertexAttribDivisorANGLE(attr_index, divisor));
  #else
    gpu(glVertexAttribDivisorEXT(attr_index, divisor));
  #endif
#else
    gpu(glVertexAttribDivisor(attr_index, divisor));
#endif
}



void VertexBuffer::bind_positions() {
#if GLES_MAJOR_VERSION <= 2
  bind();
  AttrIndexToCallback::iterator iter;
  for (iter = _positioners.begin(); iter != _positioners.end(); ++iter) {
    iter->second();
  }
#endif
}

void VertexBuffer::wipe_positioners() {
  _positioners.clear();
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

void VertexBuffer::__set_elements_to_feed_to_float_vertex_attribute(const ElementSource& element_source, GLuint attr_loc) {
  gpu(glEnableVertexAttribArray(attr_loc));
  gpu(glVertexAttribPointer(attr_loc, element_source.num_components, element_source.type, element_source.normalized, element_source.stride, element_source.pointer));
  set_vertex_divisor(attr_loc, element_source.vertex_divisor);
}

#if GLES_MAJOR_VERSION >= 3
void VertexBuffer::__set_elements_to_feed_to_integer_vertex_attribute(const ElementSource& element_source, GLuint attr_loc) {
  gpu(glEnableVertexAttribArray(attr_loc));
  gpu(glVertexAttribIPointer(attr_loc, element_source.num_components, element_source.type, element_source.stride, element_source.pointer));
  set_vertex_divisor(attr_loc, element_source.vertex_divisor);
}
#endif

void VertexBuffer::draw(GLenum primitive_mode, GLint first, GLsizei count) {
  gpu(glDrawArrays(primitive_mode, first, count));
}
}
