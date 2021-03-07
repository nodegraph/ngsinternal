#include <base/device/deviceheadersgl.h>
#include <base/device/program/uniforminfo.h>
#include <iostream>

namespace ngs {

UniformInfo::UniformInfo()
    : _name(""),
      _num_elements(0),
      _type(GL_FLOAT),
      _index(0) {
}

UniformInfo::~UniformInfo() {
}

// -----------------------------------------------------------------------------------------
// Setters.
// -----------------------------------------------------------------------------------------

void UniformInfo::set_name(const std::string& name) {
  _name = name;
}

void UniformInfo::set_num_elements(GLint num) {
  _num_elements = num;
}

void UniformInfo::set_type(GLenum type) {
  _type = type;
}

void UniformInfo::set_index(GLuint index) {
  _index = index;
}

// -----------------------------------------------------------------------------------------
// Getters.
// -----------------------------------------------------------------------------------------

const std::string& UniformInfo::get_name() const {
  return _name;
}

GLint UniformInfo::get_num_elements() const {
  return _num_elements;
}

GLenum UniformInfo::get_type() const {
  return _type;
}

GLuint UniformInfo::get_index() const {
  return _index;
}

GLint UniformInfo::get_total_size_bytes() const {
  return get_type_size_bytes() * _num_elements;
}

GLint UniformInfo::get_type_size_bytes() const {
  switch (_type) {
    case GL_FLOAT: {
      return sizeof(float);
    }
      break;
    case GL_FLOAT_VEC2: {
      return 2 * sizeof(float);
    }
      break;
    case GL_FLOAT_VEC3: {
      return 3 * sizeof(float);
    }
      break;
    case GL_FLOAT_VEC4: {
      return 4 * sizeof(float);
    }
      break;
    case GL_INT: {
      return sizeof(int);
    }
      break;
    case GL_INT_VEC2: {
      return 2 * sizeof(int);
    }
      break;
    case GL_INT_VEC3: {
      return 3 * sizeof(int);
    }
      break;
    case GL_INT_VEC4: {
      return 4 * sizeof(int);
    }
      break;
    case GL_UNSIGNED_INT: {
      return sizeof(unsigned int);
    }
      break;
#if GLES_MAJOR_VERSION >= 3
    case GL_UNSIGNED_INT_VEC2: {
      return 2 * sizeof(unsigned int);
    }
      break;
    case GL_UNSIGNED_INT_VEC3: {
      return 3 * sizeof(unsigned int);
    }
      break;
    case GL_UNSIGNED_INT_VEC4: {
      return 4 * sizeof(unsigned int);
    }
      break;
#endif
    case GL_BOOL: {
      return sizeof(bool);
    }
      break;
    case GL_BOOL_VEC2: {
      return 2 * sizeof(bool);
    }
      break;
    case GL_BOOL_VEC3: {
      return 3 * sizeof(bool);
    }
      break;
    case GL_BOOL_VEC4: {
      return 4 * sizeof(bool);
    }
      break;
    case GL_FLOAT_MAT2: {
      return 4 * sizeof(float);
    }
      break;
    case GL_FLOAT_MAT3: {
      return 9 * sizeof(float);
    }
      break;
    case GL_FLOAT_MAT4: {
      return 16 * sizeof(float);
    }
      break;
#if GLES_MAJOR_VERSION >= 3
    case GL_FLOAT_MAT2x3: {
      return 6 * sizeof(float);
    }
      break;
    case GL_FLOAT_MAT2x4: {
      return 8 * sizeof(float);
    }
      break;
    case GL_FLOAT_MAT3x2: {
      return 6 * sizeof(float);
    }
      break;
    case GL_FLOAT_MAT3x4: {
      return 12 * sizeof(float);
    }
      break;
    case GL_FLOAT_MAT4x2: {
      return 8 * sizeof(float);
    }
      break;
    case GL_FLOAT_MAT4x3: {
      return 12 * sizeof(float);
    }
      break;
#endif
#if GLES_MAJOR_VERSION >= 100
    case GL_SAMPLER_1D:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_2D_RECT:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
#endif

    case GL_SAMPLER_2D:
    case GL_SAMPLER_CUBE:

#if GLES_MAJOR_VERSION >= 3
    case GL_SAMPLER_3D:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
#endif
 {
      return sizeof(int);
    }
      break;
    default: {
      std::cerr << "Error: UniformType encountered an unhandled GL type.\n";
      return -1;
    }
      break;
  }
}


}

