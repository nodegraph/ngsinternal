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

/*
 * The Circle is draw with the origin at (0,0).
 */

class DEVICE_EXPORT Circle {
 public:

  static const size_t num_samples;

  Circle();
  Circle(float radius, float depth, glm::vec2 shift);
  ~Circle();

  void set(float radius, float depth, glm::vec2 shift);
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

  static std::vector<glm::vec2> get_samples(size_t num_samples, float radius, const glm::vec2& translate);

 private:
  float _radius, _depth;
  glm::vec2 _origin;

  // Vertex Array to keep track of our associated buffers.
  VertexArray _vao;

  // Buffers to draw one quad.
  VertexBuffer _vbo;
  IndexBuffer _ibo;
};

}

