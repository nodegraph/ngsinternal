#pragma once
#include <base/device/device_export.h>


namespace ngs {

class DEVICE_EXPORT RenderBuffer {
 public:
  RenderBuffer(GLenum internal_format, GLsizei width, GLsizei height,
               GLsizei samples = 0);
  virtual ~RenderBuffer();

  GLuint get_name();
  GLenum get_target();

  GLsizei get_width();
  GLsizei get_height();

  void bind();
  void unbind();

  // The RenderBuffer must be bound before calling the following methods.
  void resize(GLsizei width, GLsizei height, GLsizei samples = 0);

 private:
  void create_name();
  void remove_name();
  void create_storage();

  GLuint _name;
  GLuint _target;

  GLenum _internal_format;
  GLsizei _width;
  GLsizei _height;
  GLsizei _samples;

};

}
