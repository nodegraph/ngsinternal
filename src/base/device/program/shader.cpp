#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>

#include <base/device/program/shader.h>
#include <base/utils/archdebug.h>

#include <iostream>
#include <cassert>


namespace ngs {

Shader::Shader(GLenum type, GLsizei num_strings, const GLchar** strings)
    : _type(type) {
  create_name();
  gpu(glShaderSource(_name,num_strings,strings,0));  // we assume null terminated strings.
  gpu(glCompileShader(_name));
  if (!is_compiled()) {
    std::cerr << "Error compiling shader:\n";
    std::cerr << get_info_log() << "\n";
    std::string info = get_info_log();
    assert(false);
#ifdef ANDROID_ARCH
    __android_log_print(ANDROID_LOG_INFO, "XXXXXX", "error compiling shader!!!!");
    __android_log_print(ANDROID_LOG_INFO, "XXXXXX", "error is: %s",get_info_log().c_str());
    exit(1);
#endif
  } else {
    //std::cerr<<"Success: shader compiled.\n";
    //std::cerr<<get_info_log()<<"\n";
  }
}

Shader::~Shader() {
  remove_name();
}

GLuint Shader::get_name() const {
  return _name;
}

GLenum Shader::get_type() const {
  return _type;
}

void Shader::create_name() {
  gpu(_name=glCreateShader(_type));
  if (_name == 0) {
    std::cerr
        << "Error insufficient memory is likely.  unable to create a shader object name.\n";
    assert(false);
  }
}

void Shader::remove_name() {
  if (_name) {
    gpu(glDeleteShader(_name));
    _name = 0;
  }
}

bool Shader::is_compiled() {
  GLint state;
  gpu(glGetShaderiv(_name, GL_COMPILE_STATUS, &state));
  if (state == 1) {
    return true;
  }
  return false;
}

std::string Shader::get_info_log() {
  std::string log;
  int length = 0;
  gpu(glGetShaderiv(_name,GL_INFO_LOG_LENGTH,&length));

  if (length > 0) {
    int written = 0;
    log.resize(length);
    gpu(glGetShaderInfoLog(_name,length,&written,&log[0]));
  }
  return log;
}

}
