#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>
#include <base/device/geometry/instancevertexattribute.h>
#include <base/device/geometry/objectvertexattribute.h>
#include <base/device/transforms/glmhelper.h>
#include <base/device/unpackedbuffers/vertexarray.h>
#include <base/device/unpackedbuffers/vertexbuffer.h>
#include <base/device/unpackedbuffers/indexbuffer.h>
#include <vector>


namespace ngs {

class DEVICE_EXPORT Quad {
 public:
  Quad();
  Quad(float width, float height, float depth, glm::vec2 shift);
  ~Quad();

  void set(float width, float height, float depth, glm::vec2 shift);
  void update_gpu();

  // Binding.
  void bind();
  void unbind();

  // Draw.
  void draw();
  void draw_instances(size_t num);

  // Feed elements to an attribute location.
  void feed_positions_to_attr(GLuint attr_loc);
  void feed_tex_coords_to_attr(GLuint attr_loc);

 private:
  float _width, _height, _depth;
  glm::vec2 _shift;

  // Vertex Array to keep track of our associated buffers.
  VertexArray _vao;

  // Buffers to draw one quad.
  VertexBuffer _vbo;
  IndexBuffer _ibo;
};

}

