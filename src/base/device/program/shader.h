#pragma once
#include <base/device/device_export.h>

#include <string>

namespace ngs {

class DEVICE_EXPORT Shader {
 public:
  Shader(GLenum type, GLsizei num_strings, const GLchar** strings);
  virtual ~Shader();

  GLuint get_name() const;
  GLenum get_type() const;

 private:
  void create_name();
  void remove_name();

  bool is_compiled();
  std::string get_info_log();

  GLuint _name;
  GLenum _type;

};

}
