#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>
#include <base/device/geometry/triangle.h>

#include <iostream>

namespace ngs {

Triangle::Triangle()
    : _width(1),
      _height(1),
      _depth(0),
      _shift(0, 0),
      _ibo(GL_TRIANGLE_STRIP, GL_UNSIGNED_SHORT),
      _vao(),
      _vbo() {
  _vao.set_vertex_buffer(&_vbo);
  _vao.set_index_buffer(&_ibo);
  update_gpu();
}

Triangle::Triangle(float width, float height, float depth, glm::vec2 shift)
    : _width(width),
      _height(height),
      _depth(depth),
      _shift(shift),
      _ibo(GL_TRIANGLE_STRIP, GL_UNSIGNED_SHORT),
      _vao(),
      _vbo(){
  _vao.set_vertex_buffer(&_vbo);
  _vao.set_index_buffer(&_ibo);
  update_gpu();
}


Triangle::~Triangle() {
}


void Triangle::set(float width, float height, float depth, glm::vec2 shift) {
  if ((_width == width) &&
      (_height == height) &&
      (_depth == depth) &&
      (_shift == shift)) {
    return;
  }
  _width=width;
  _height=height;
  _depth=depth;
  _shift=shift;
  update_gpu();
}


void Triangle::update_gpu() {
  std::vector<PosTexVertex> attrs;
  attrs.resize(4);

  glm::vec2 tip = _shift;
  glm::vec2 right_base = tip + glm::vec2(_width/2.0f, _height);
  glm::vec2 left_base = tip + glm::vec2(-_width/2.0f, _height);

  // Setup vertices.
  attrs[0].position[0] = tip[0];
  attrs[0].position[1] = tip[1];
  attrs[0].position[2] = _depth;
  attrs[1].position[0] = right_base[0];
  attrs[1].position[1] = right_base[1];
  attrs[1].position[2] = _depth;
  attrs[2].position[0] = left_base[0];
  attrs[2].position[1] = left_base[1];
  attrs[2].position[2] = _depth;

  // Setup coordinates.
  attrs[0].tex_coord[0] = 0.5;
  attrs[0].tex_coord[1] = 0;
  attrs[1].tex_coord[0] = 1;
  attrs[1].tex_coord[1] = 1;
  attrs[2].tex_coord[0] = 0;
  attrs[2].tex_coord[1] = 1;

  // Setup Indices.
  std::vector<unsigned short> indices;
  indices.resize(4);
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;

  // Bind VAO.
  _vao.bind();

  // Bind VBO.
  _vbo.bind();
  _vbo.load(sizeof(PosTexVertex) * 3, &attrs[0]);

  // Bind IBO;
  _ibo.bind();
  _ibo.load(sizeof(unsigned short) * 3, &indices[0]);
}


void Triangle::bind() {
  _vao.bind();
}


void Triangle::unbind() {
  _vao.unbind();
}


void Triangle::draw() {
  _ibo.draw();
}


void Triangle::draw_instances(size_t num) {
  _ibo.draw_instances(num);
}


void Triangle::feed_positions_to_attr(GLuint attr_loc) {
  _vbo.bind();
  _vbo.set_elements_to_feed_to_float_vertex_attribute(PosTexVertex::position_source, attr_loc);
}


void Triangle::feed_tex_coords_to_attr(GLuint attr_loc) {
  _vbo.bind();
  _vbo.set_elements_to_feed_to_float_vertex_attribute(PosTexVertex::tex_coord_source, attr_loc);
}

}
