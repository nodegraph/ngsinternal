#include <base/device/deviceheadersgl.h>
#include <base/device/asyncquery.h>
#include <base/device/devicedebug.h>
#include <base/glewhelper/glewhelper.h>


#include <iostream>
#include <cassert>

namespace ngs {

#if GLES_MAJOR_VERSION >= 3

AsyncQuery::AsyncQuery(GLenum target)
    : _target(target),
      _name(0) {
  create_name();
}

AsyncQuery::~AsyncQuery() {
  remove_name();
}

void AsyncQuery::create_name() {
  gpu(glGenQueries(1, &_name));
  if (_name == 0) {
    std::cerr
        << "Error: insufficient memory is likely. unable to create an async query object name.\n";
    assert(false);
  }
}

void AsyncQuery::remove_name() {
  if (_name != 0) {
    gpu(glDeleteQueries(1, &_name));
  }
  _name = 0;
}

GLuint AsyncQuery::get_name() const {
  return _name;
}

// Bind should be called to start query.
// This is like other gl binds except the gl keyword doens't have the word bind in it.
void AsyncQuery::bind() {
  glBeginQuery(_target, _name);
}

// Unbind should be called to end query.
// This is like other gl binds except the gl keyword doens't have the word bind in it.
void AsyncQuery::unbind() {
  glEndQuery(_target);
}

GLuint AsyncQuery::get_current_bound_name() {
  GLint name;
  glGetQueryiv(_target, GL_CURRENT_QUERY, &name);
  return name;
}

#if GLES_MAJOR_VERSION>=100
GLint AsyncQuery::get_num_bits() {
  GLint num_bits;
  glGetQueryiv(_target, GL_QUERY_COUNTER_BITS, &num_bits);
  return num_bits;
}
#endif

bool AsyncQuery::resultIsReady() {
  GLuint ready;
  glGetQueryObjectuiv(_name, GL_QUERY_RESULT_AVAILABLE, &ready);
  if (ready == GL_FALSE) {
    return false;
  }
  return true;
}

GLint AsyncQuery::get_result() {
  // Get result of the query. This can the number of samples or primitives or elapsed time.
  GLuint result;
  glGetQueryObjectuiv(_name, GL_QUERY_RESULT, &result);
  return result;
}

void AsyncQuery::disable_color_and_depth_writes() {
  glColorMask(false, false, false, false);
  glDepthMask(false);
}

void AsyncQuery::enable_color_and_depth_writes() {
  glColorMask(true, true, true, true);
  glDepthMask(true);
}

#endif

}
