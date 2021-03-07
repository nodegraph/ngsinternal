#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>
#include <base/device/geometry/quad.h>

#include <iostream>

namespace ngs {

Quad::Quad()
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

Quad::Quad(float width, float height, float depth, glm::vec2 shift)
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


Quad::~Quad() {
}


void Quad::set(float width, float height, float depth, glm::vec2 shift) {
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


void Quad::update_gpu() {
  std::vector<PosTexVertex> attrs;
  attrs.resize(4);

  glm::vec2 ll = _shift;
  glm::vec2 lr = ll;
  lr[0] += _width;
  glm::vec2 tr = lr;
  tr[1] += _height;
  glm::vec2 tl = tr;
  tl[0] = ll[0];

  // Setup vertices.
  attrs[0].position[0] = ll[0];
  attrs[0].position[1] = ll[1];
  attrs[0].position[2] = _depth;
  attrs[1].position[0] = lr[0];
  attrs[1].position[1] = lr[1];
  attrs[1].position[2] = _depth;
  attrs[2].position[0] = tr[0];
  attrs[2].position[1] = tr[1];
  attrs[2].position[2] = _depth;
  attrs[3].position[0] = tl[0];
  attrs[3].position[1] = tl[1];
  attrs[3].position[2] = _depth;

  // Setup coordinates.
  attrs[0].tex_coord[0] = 0;
  attrs[0].tex_coord[1] = 0;
  attrs[1].tex_coord[0] = 1;
  attrs[1].tex_coord[1] = 0;
  attrs[2].tex_coord[0] = 1;
  attrs[2].tex_coord[1] = 1;
  attrs[3].tex_coord[0] = 0;
  attrs[3].tex_coord[1] = 1;

  // Setup Indices.
  std::vector<unsigned short> indices;
  indices.resize(4);
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 3;
  indices[3] = 2;

  // Bind VAO.
  _vao.bind();

  // Bind VBO.
  _vbo.bind();
  _vbo.load(sizeof(PosTexVertex) * 4, &attrs[0]);

  // Bind IBO;
  _ibo.bind();
  _ibo.load(sizeof(unsigned short) * 4, &indices[0]);
}


void Quad::bind() {
  _vao.bind();
}


void Quad::unbind() {
  _vao.unbind();
}


void Quad::draw() {
  _ibo.draw();
}


void Quad::draw_instances(size_t num) {
  _ibo.draw_instances(num);
}


void Quad::feed_positions_to_attr(GLuint attr_loc) {
  _vbo.bind();
  _vbo.set_elements_to_feed_to_float_vertex_attribute(PosTexVertex::position_source, attr_loc);
}


void Quad::feed_tex_coords_to_attr(GLuint attr_loc) {
  _vbo.bind();
  _vbo.set_elements_to_feed_to_float_vertex_attribute(PosTexVertex::tex_coord_source, attr_loc);
}

}
