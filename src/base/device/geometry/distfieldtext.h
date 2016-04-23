#pragma once
#include <base/device/device_export.h>
#include <base/device/geometry/instancevertexattribute.h>
#include <base/device/geometry/objectvertexattribute.h>
#include <base/device/transforms/glmhelper.h>
#include <base/device/unpackedbuffers/vertexarray.h>
#include <base/device/unpackedbuffers/vertexbuffer.h>
#include <base/device/unpackedbuffers/indexbuffer.h>
#include <vector>


namespace ngs {

template<class VertexAttr>
class DEVICE_EXPORT DistFieldText {
 public:
  DistFieldText();
  ~DistFieldText();

  // Binding.
  void bind();
  void unbind();

  // Draw.
  void draw();

  // DistFieldText must be bound before calling these methods.

  // Object settings.
//  void set_position_index(GLuint attr_index);
//  void set_tex_coord_index(GLuint attr_index);

  void load_all_vertices(const std::vector<VertexAttr>& vertices);

 public:
  // Vertex Array to keep track of our associated buffers.
  VertexArray _vao;

  // Buffers to draw one quad.
  VertexBuffer _vbo;
  IndexBuffer _ibo;

  // System side indices.
  std::vector<unsigned int> _text_indices;
};

}

namespace ngs {
#ifndef ong_device_EXPORTS
    extern
#endif
    //template class DEVICE_EXPORT DistFieldText<Pos2TexVertex>;
    template class DEVICE_EXPORT DistFieldText<PosTexVertex>;
}
