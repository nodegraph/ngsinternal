#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>

namespace ngs {

struct ElementSource {
  GLint num_components;
  GLenum type;
  GLboolean normalized;
  GLsizei stride;
  const GLvoid *pointer;

  // The vertex attrib divisor for instanced rendering.
  // eg: glVertexAttribDivisor(attr_index, divisor);
  GLuint vertex_divisor;
};

}
