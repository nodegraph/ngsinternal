#pragma once
#include <base/device/device_export.h>
#include <base/device/devicebasictypesgl.h>

#include <string>

namespace ngs {

class DEVICE_EXPORT UniformInfo {
 public:
  UniformInfo();
  virtual ~UniformInfo();  // it's not virtual because we don't expect any sub classes to inherit this.

  /* Setters.*/
  virtual void set_name(const std::string& name);
  virtual void set_num_elements(GLint num);
  virtual void set_type(GLenum type);
  virtual void set_index(GLuint index);

  /* Getters.*/
  virtual const std::string& get_name() const;
  virtual GLint get_num_elements() const;
  virtual GLenum get_type() const;
  virtual GLuint get_index() const;

  /* Size.*/
  virtual GLint get_total_size_bytes() const;
  virtual GLint get_type_size_bytes() const;

 private:
  std::string _name;  // name in the glsl shader
  GLint _num_elements;  // number of elements in the array
  GLenum _type;  // data type
  GLuint _index;  // index number which uniquely identifies this uniform with respect to the program.
                  // note that all uniforms (both default and block uniforms) are assigned a unique index.

};

}
