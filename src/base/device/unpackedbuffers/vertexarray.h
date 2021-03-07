#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>

namespace ngs {

class VertexBuffer;
class IndexBuffer;

/*
 In order to set the vertex attribute pointers in this Vertex Array Object,
 first bind this Vertex Array Object then bind a Vertex Buffer Objects,
 then call VertexBuffer::set_vertex_attribute with appropriate parameters for each 
 vertex attribute.

 In order to set the index attribute pointers in this Vertex Array Object,
 first bind this Vertex Array Object then bind an Index Buffer Object.

 In order to draw with the info provided by the bound VertexArray, you can call
 draw on the Vertex Buffer Object or the Index Buffer Object depending on
 whether or not your geometry has an indices or not.

 */

#if GLES_MAJOR_VERSION >= 3
class DEVICE_EXPORT VertexArray {
 public:
  VertexArray();
  ~VertexArray();

  //GLuint get_name() const;

  void bind();
  void unbind();

  // The following methods require the VertexArray to be bound.
  //void draw();

  void set_vertex_buffer(VertexBuffer* vertex_buffer){}
  void set_index_buffer(IndexBuffer* index_buffer){}

 private:
  void create_name();
  void remove_name();

  // Name.
  GLuint _name;
};
#else
class DEVICE_EXPORT VertexArray {
 public:
  VertexArray();
  ~VertexArray();

  void bind();
  void unbind();

  // We have an artificial convention where we currently only allow one vertex buffer
  // and one index buffer per vertex array. In other words all the vertex attributes
  // should be packed into one vertex buffer.
  void set_vertex_buffer(VertexBuffer* vertex_buffer);
  void set_index_buffer(IndexBuffer* index_buffer);
 private:
  VertexBuffer* _vertex_buffer; // borrowed reference
  IndexBuffer* _index_buffer; // borrowed reference
};
#endif

}
