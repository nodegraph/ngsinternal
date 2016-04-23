#pragma once
#include <base/device/device_export.h>
#include <base/device/unpackedbuffers/buffer.h>

#include <functional>
#include <map>

namespace ngs {

struct ElementSource;

class DEVICE_EXPORT VertexBuffer : public Buffer {
 public:
  VertexBuffer();
  virtual ~VertexBuffer();

  void set_elements_to_feed_to_float_vertex_attribute(const ElementSource& element_source, GLuint attr_loc);
#if GLES_MAJOR_VERSION >= 3
  void set_elements_to_feed_to_integer_vertex_attribute(const ElementSource& element_source, GLuint attr_loc);
#endif

  void bind_positions();
  void wipe_positioners();

  // ------------------------------------------------------------------------------------------
  // Static methods
  // Call these after a Vertex Array Object and this Vertex Buffer Object are bound.
  // ------------------------------------------------------------------------------------------
  static void __set_elements_to_feed_to_float_vertex_attribute(const ElementSource& element_source, GLuint attr_loc);
#if GLES_MAJOR_VERSION >= 3
  static void __set_elements_to_feed_to_integer_vertex_attribute(const ElementSource& element_source, GLuint attr_loc);
  static void __set_vertex_divisor(GLuint attr_index, GLuint divisor);
#endif

  static void set_vertex_divisor(GLuint attr_index, GLuint divisor);

  static void draw(GLenum primitive_mode, GLint first, GLsizei count);

 private:
  typedef std::function<void()> VoidCallback;
  typedef std::map<GLuint, VoidCallback> AttrIndexToCallback;
  AttrIndexToCallback _positioners;

};

}
