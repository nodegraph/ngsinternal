#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>
#include <base/device/geometry/circle.h>

#include <boost/math/constants/constants.hpp>
#include <iostream>

namespace ngs {

const size_t Circle::num_samples = 12;

Circle::Circle()
    : _radius(1.0f),
      _depth(0.0f),
      _origin(0.0f, 0.0f),
      _ibo(GL_TRIANGLE_FAN, GL_UNSIGNED_SHORT),
      _vao(),
      _vbo() {
  _vao.set_vertex_buffer(&_vbo);
  _vao.set_index_buffer(&_ibo);
  update_gpu();
}

Circle::Circle(float radius, float depth, glm::vec2 shift)
    : _radius(radius),
      _depth(depth),
      _origin(shift),
      _ibo(GL_TRIANGLE_FAN, GL_UNSIGNED_SHORT),
      _vao(),
      _vbo(){
  _vao.set_vertex_buffer(&_vbo);
  _vao.set_index_buffer(&_ibo);
  update_gpu();
}


Circle::~Circle() {
}


void Circle::set(float radius, float depth, glm::vec2 shift) {
  if ((_radius == radius) &&
      (_depth == depth) &&
      (_origin == shift)) {
    return;
  }
  _radius=radius;
  _depth=depth;
  _origin=shift;
  update_gpu();
}

std::vector<glm::vec2> Circle::get_samples(size_t num_samples, float radius, const glm::vec2& translate) {
  std::vector<glm::vec2> samples;
  samples.resize(num_samples);
  const float pi = boost::math::constants::pi<float>();
  const float inc = 2.0f * pi / static_cast<float>(num_samples);
  float angle = 0.0f;
  for (size_t i = 0; i < num_samples; ++i) {
    // Setup vertices.
    samples[i] = radius * glm::vec2(cos(angle) - sin(angle), sin(angle) + cos(angle)) + translate;
    // Ready the next loop.
    angle += inc;
  }
  return samples;
}


void Circle::update_gpu() {
  // There num_samples+2 vertices.
  // One is the for the center, and one is a duplicate to close out the circle.
  std::vector<PosTexVertex> attrs;
  attrs.resize(num_samples+2);

  const float pi = boost::math::constants::pi<float>();
  const float inc = 2.0f * pi / static_cast<float>(num_samples);
  float angle = 0.0f;
  // First record the origin of the triangle fan.
  attrs[0].position[0] = _origin.x;
  attrs[0].position[1] = _origin.y;
  attrs[0].position[2] = _depth;
  attrs[0].tex_coord[0] = 0;
  attrs[0].tex_coord[1] = 0;
  // Now record all the outer vertices of the circle.
  for (size_t i = 0; i < num_samples+1; ++i) {
    PosTexVertex &ptv = attrs[i+1];
    // Setup vertices.
    ptv.position[0] = _radius * (cos(angle) - sin(angle));
    ptv.position[1] = _radius * (sin(angle) + cos(angle));
    ptv.position[2] = _depth;
    // Setup coordinates.
    ptv.tex_coord[0] = 0; // bogus value for now
    ptv.tex_coord[1] = 0; // bogus value for now
    // Ready the next loop.
    angle += inc;
  }

  // Setup Indices.
  std::vector<unsigned short> indices;
  indices.resize(num_samples + 2);
  for (size_t i = 0; i < num_samples + 2; ++i) {
    indices[i] = i;
  }

  // Bind VAO.
  _vao.bind();

  // Bind VBO.
  _vbo.bind();
  _vbo.load(sizeof(PosTexVertex) * (num_samples+2), &attrs[0]);

  // Bind IBO;
  _ibo.bind();
  _ibo.load(sizeof(unsigned short) * (num_samples+2), &indices[0]);
}


void Circle::bind() {
  _vao.bind();
}


void Circle::unbind() {
  _vao.unbind();
}


void Circle::draw() {
  _ibo.draw();
}


void Circle::draw_instances(size_t num) {
  _ibo.draw_instances(num);
}


void Circle::feed_positions_to_attr(GLuint attr_loc) {
  _vbo.bind();
  _vbo.set_elements_to_feed_to_float_vertex_attribute(PosTexVertex::position_source, attr_loc);
}


void Circle::feed_tex_coords_to_attr(GLuint attr_loc) {
  _vbo.bind();
  _vbo.set_elements_to_feed_to_float_vertex_attribute(PosTexVertex::tex_coord_source, attr_loc);
}

}
