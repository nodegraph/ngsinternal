#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>
#include <base/device/geometry/distfieldtext.h>

#include <iostream>

namespace ngs {

template<class VertexAttr>
DistFieldText<VertexAttr>::DistFieldText()
    : _ibo(GL_TRIANGLES, GL_UNSIGNED_INT),
      _vao(),
      _vbo(){
  _vao.set_vertex_buffer(&_vbo);
  _vao.set_index_buffer(&_ibo);
}

template<class VertexAttr>
DistFieldText<VertexAttr>::~DistFieldText() {
}

template<class VertexAttr>
void DistFieldText<VertexAttr>::bind() {
  _vao.bind();

  // Since we are drawing in Qt's render thread and need to make sure we restore all gl state.
  // It's better if we don't enable blending for now.
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

template<class VertexAttr>
void DistFieldText<VertexAttr>::unbind() {
  _vao.unbind();
}

template<class VertexAttr>
void DistFieldText<VertexAttr>::draw() {
  _ibo.draw();
}

// Object Attributes.

//template<class VertexAttr>
//void DistFieldText<VertexAttr>::set_position_index(GLuint attr_index) {
//  VertexAttr::set_position_index(&_vbo, attr_index);
//}
//
//template<class VertexAttr>
//void DistFieldText<VertexAttr>::set_tex_coord_index(GLuint attr_index) {
//  VertexAttr::set_tex_coord_index(&_vbo, attr_index);
//}

template<class VertexAttr>
void DistFieldText<VertexAttr>::load_all_vertices(const std::vector<VertexAttr>& vertices) {
  _vao.bind();
  _vbo.bind();
  _vbo.load(vertices.size()*sizeof(VertexAttr), &vertices[0]);

  // We also load the indices.
  size_t quad_count = _text_indices.size()/6; // 6 indices per quad, 3 vertices per triangle
  size_t next_quad_count = vertices.size()/4; // 4 vertices per quad

  // If we don't have enough system indices we create some more and load them to the gpu.
  _ibo.bind();
  if (next_quad_count > quad_count) {
    _text_indices.resize(next_quad_count*6);
    for (size_t i=quad_count; i<next_quad_count; i++) {
      size_t base_index = i*6;
      size_t base_vertex = i*4;
      _text_indices[base_index+0] = base_vertex;
      _text_indices[base_index+1] = base_vertex+1;
      _text_indices[base_index+2] = base_vertex+2;
      _text_indices[base_index+3] = base_vertex;
      _text_indices[base_index+4] = base_vertex+2;
      _text_indices[base_index+5] = base_vertex+3;
    }
    // Load the indices to the gpu.
    _ibo.load(_text_indices.size()*sizeof(unsigned int), &_text_indices[0]);
  }
}

// Template Instantiations.

//template class DEVICE_EXPORT DistFieldText<Pos2TexVertex>;
template class DEVICE_EXPORT DistFieldText<PosTexVertex>;
}
