#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/device/unpackedbuffers/renderbuffer.h>

#include <iostream>
#include <cassert>



namespace ngs {

RenderBuffer::RenderBuffer(GLenum internal_format, GLsizei width,
                           GLsizei height, GLsizei samples)
    : _internal_format(internal_format),
      _width(width),
      _height(height),
      _samples(samples) {
  _target = GL_RENDERBUFFER;
  create_name();
  bind();
  create_storage();
}

RenderBuffer::~RenderBuffer() {
  remove_name();
}

GLuint RenderBuffer::get_name() {
  return _name;
}

GLenum RenderBuffer::get_target() {
  return _target;
}

GLsizei RenderBuffer::get_width() {
  return _width;
}

GLsizei RenderBuffer::get_height() {
  return _height;
}

void RenderBuffer::bind() {
  gpu(glBindRenderbuffer(_target,_name););
}

void RenderBuffer::unbind() {
  gpu(glBindRenderbuffer(_target,0););
}

void RenderBuffer::resize(GLsizei width, GLsizei height, GLsizei samples) {
  _width = width;
  _height = height;
  _samples = samples;
  create_storage();
}

void RenderBuffer::create_name() {
  gpu(glGenRenderbuffers(1,&_name););
  if (_name == 0) {
    std::cerr
        << "Error: memory error is likely. unable to create a new_ renderbuffer object name.\n";
    assert(false);
  }
}

void RenderBuffer::remove_name() {
  if (_name != 0) {
    gpu(glDeleteRenderbuffers(1, &_name));
  }
  _name = 0;
}


void RenderBuffer::create_storage() {
#if GLES_MAJOR_VERSION >= 3
  gpu(glRenderbufferStorageMultisample(_target, _samples, _internal_format, _width, _height););
#else
  gpu(glRenderbufferStorage(_target, _internal_format, _width, _height););
#endif
}


}
