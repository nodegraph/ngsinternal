#include <base/device/unpackedbuffers/vertexarray.h>
#include <base/device/unpackedbuffers/vertexbuffer.h>
#include <base/device/unpackedbuffers/indexbuffer.h>

#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <iostream>
#include <cassert>

namespace ngs {
#if GLES_MAJOR_VERSION >= 3


VertexArray::VertexArray() {
  create_name();
}

VertexArray::~VertexArray() {
  remove_name();
}

void VertexArray::create_name() {
  gpu(glGenVertexArrays(1, &_name));
  if (_name == 0) {
    std::cerr
        << "Error: insufficient memory is likely. unable to create a buffer object name.\n";
    assert(false);
  }
}

void VertexArray::remove_name() {
  if (_name != 0) {
    gpu(glDeleteVertexArrays(1, &_name));
  }
  _name = 0;
}

//GLuint VertexArray::get_name() const {
//  return _name;
//}

void VertexArray::bind() {
  gpu(glBindVertexArray(_name));
}

void VertexArray::unbind() {
  gpu(glBindVertexArray(0));
}


#elif GLES_MAJOR_VERSION >= 1

VertexArray::VertexArray():
  _vertex_buffer(NULL),
  _index_buffer(NULL){}

VertexArray::~VertexArray(){
  _vertex_buffer=NULL;
  _index_buffer=NULL;
}

void VertexArray::bind() {
  _vertex_buffer->bind();
  _vertex_buffer->bind_positions();
  if (_index_buffer) {
    _index_buffer->bind();
  }
}

void VertexArray::unbind() {
  _vertex_buffer->unbind();
  if (_index_buffer) {
    _index_buffer->unbind();
  }
}

void VertexArray::set_vertex_buffer(VertexBuffer* vertex_buffer){
  _vertex_buffer = vertex_buffer;
}
void VertexArray::set_index_buffer(IndexBuffer* index_buffer){
  _index_buffer = index_buffer;
}

#endif
}
