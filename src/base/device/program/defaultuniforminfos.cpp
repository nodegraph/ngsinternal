#include <base/memoryallocator/taggednew.h>
#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>
#include <base/device/program/defaultuniforminfo.h>
#include <base/device/program/defaultuniforminfos.h>

#include <base/device/program/program.h>
#include <cassert>
#include <iostream>

namespace ngs {

DefaultUniformInfos::DefaultUniformInfos(Program& program)
    : _program(program) {

  // We own the returned infos.
  std::vector<DefaultUniformInfo*> infos = _program.create_default_uniform_infos();

  // Create a dictionary of the uniform type for fast lookup by name.
  for (size_t i = 0; i < infos.size(); i++) {
    _name_to_info[infos[i]->get_name()] = infos[i];
  }
}

DefaultUniformInfos::~DefaultUniformInfos() {
  for (auto &kv: _name_to_info) {
    delete_ff(kv.second);
  }
  _name_to_info.clear();
}

bool DefaultUniformInfos::uniform_exists(const std::string& name) const {
  return _name_to_info.count(name);
}

std::vector<std::string> DefaultUniformInfos::get_uniform_names() const {
  std::vector<std::string> names;
  for(auto key : _name_to_info) {
      names.push_back(key.first);
  }
  return names;
}

const DefaultUniformInfo* DefaultUniformInfos::get_uniform_info(const std::string& name) const{
  if (!uniform_exists(name)) {
    return NULL;
  }
  return _name_to_info.at(name);
}

std::vector<const DefaultUniformInfo*> DefaultUniformInfos::get_uniform_infos() const {
  std::vector<const DefaultUniformInfo*> infos;
  infos.clear();
  for(auto key : _name_to_info) {
      infos.push_back(key.second);
  }
  return infos;
}

bool DefaultUniformInfos::set_uniform(const DefaultUniformInfo* info, const void* value) {
  switch (info->get_type()) {
    case GL_FLOAT: {
      const float* data = reinterpret_cast<const float*>(value);
      gpu(glUniform1fv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_FLOAT_VEC2: {
      const float* data = reinterpret_cast<const float*>(value);
      gpu(glUniform2fv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_FLOAT_VEC3: {
      const float* data = reinterpret_cast<const float*>(value);
      gpu(glUniform3fv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_FLOAT_VEC4: {
      const float* data = reinterpret_cast<const float*>(value);
      gpu(glUniform4fv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_INT: {
      const int* data = reinterpret_cast<const int*>(value);
      gpu(glUniform1iv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_INT_VEC2: {
      const int* data = reinterpret_cast<const int*>(value);
      gpu(glUniform2iv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_INT_VEC3: {
      const int* data = reinterpret_cast<const int*>(value);
      gpu(glUniform3iv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_INT_VEC4: {
      const int* data = reinterpret_cast<const int*>(value);
      gpu(glUniform4iv(info->get_location(),info->get_num_elements(),data));
    }
      break;
#if GLES_MAJOR_VERSION >= 3
    case GL_UNSIGNED_INT: {
      const unsigned int* data = reinterpret_cast<const unsigned int*>(value);
      gpu(glUniform1uiv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_UNSIGNED_INT_VEC2: {
      const unsigned int* data = reinterpret_cast<const unsigned int*>(value);
      gpu(glUniform2uiv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_UNSIGNED_INT_VEC3: {
      const unsigned int* data = reinterpret_cast<const unsigned int*>(value);
      gpu(glUniform3uiv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_UNSIGNED_INT_VEC4: {
      const unsigned int* data = reinterpret_cast<const unsigned int*>(value);
      gpu(glUniform4uiv(info->get_location(),info->get_num_elements(),data));
    }
      break;
#endif
    case GL_BOOL: {
      // By convention we expect value to be int here.
      // Although opengl allows setting bools from any type, int, unsigned int, float,
      // We only allow setting it from int.
      const int* data = reinterpret_cast<const int*>(value);
      gpu(glUniform1iv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_BOOL_VEC2: {
      const int* data = reinterpret_cast<const int*>(value);
      gpu(glUniform2iv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_BOOL_VEC3: {
      const int* data = reinterpret_cast<const int*>(value);
      gpu(glUniform3iv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_BOOL_VEC4: {
      const int* data = reinterpret_cast<const int*>(value);
      gpu(glUniform4iv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    case GL_FLOAT_MAT2: {
      const float* data = reinterpret_cast<const float*>(value);
      bool transpose = false;
      gpu(glUniformMatrix2fv(info->get_location(), info->get_num_elements(), transpose, data));
    }
      break;
    case GL_FLOAT_MAT3: {
      const float* data = reinterpret_cast<const float*>(value);
      bool transpose = false;
      gpu(glUniformMatrix3fv(info->get_location(), info->get_num_elements(), transpose, data));
    }
      break;
    case GL_FLOAT_MAT4: {
      const float* data = reinterpret_cast<const float*>(value);
      bool transpose = false;
      gpu(glUniformMatrix4fv(info->get_location(), info->get_num_elements(), transpose, data));
    }
      break;
#if GLES_MAJOR_VERSION >= 3
    case GL_FLOAT_MAT2x3: {
      const float* data = reinterpret_cast<const float*>(value);
      bool transpose = false;
      gpu(glUniformMatrix2x3fv(info->get_location(), info->get_num_elements(), transpose, data));
    }
      break;
    case GL_FLOAT_MAT2x4: {
      const float* data = reinterpret_cast<const float*>(value);
      bool transpose = false;
      gpu(glUniformMatrix2x4fv(info->get_location(), info->get_num_elements(), transpose, data));
    }
      break;
    case GL_FLOAT_MAT3x2: {
      const float* data = reinterpret_cast<const float*>(value);
      bool transpose = false;
      gpu(glUniformMatrix3x2fv(info->get_location(), info->get_num_elements(), transpose, data));
    }
      break;
    case GL_FLOAT_MAT3x4: {
      const float* data = reinterpret_cast<const float*>(value);
      bool transpose = false;
      gpu(glUniformMatrix3x4fv(info->get_location(), info->get_num_elements(), transpose, data));
    }
      break;
    case GL_FLOAT_MAT4x2: {
      const float* data = reinterpret_cast<const float*>(value);
      bool transpose = false;
      gpu(glUniformMatrix4x2fv(info->get_location(), info->get_num_elements(), transpose, data));
    }
      break;
    case GL_FLOAT_MAT4x3: {
      const float* data = reinterpret_cast<const float*>(value);
      bool transpose = false;
      gpu(glUniformMatrix4x3fv(info->get_location(), info->get_num_elements(), transpose, data));
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
      const int* data = reinterpret_cast<const int*>(value);
      gpu(glUniform1iv(info->get_location(),info->get_num_elements(),data));
    }
      break;
    default: {
      std::cerr
          << "Error: DefaultUniformTracker encountered an unhandled GL type: " << info->get_type() << "\n";
      return false;
    }
      break;
  }
  return true;
}

bool DefaultUniformInfos::get_uniform(const DefaultUniformInfo* info, void* value) {
  switch (info->get_type()) {
    case GL_FLOAT:
    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4: {
      float* data = reinterpret_cast<float*>(value);
      gpu(glGetUniformfv(_program.get_name(),info->get_location(),data));
    }
      break;
    case GL_INT:
    case GL_INT_VEC2:
    case GL_INT_VEC3:
    case GL_INT_VEC4: {
      int* data = reinterpret_cast<int*>(value);
      gpu(glGetUniformiv(_program.get_name(),info->get_location(),data));
    }
      break;
#if GLES_MAJOR_VERSION >= 3
    case GL_UNSIGNED_INT:
    case GL_UNSIGNED_INT_VEC2:
    case GL_UNSIGNED_INT_VEC3:
    case GL_UNSIGNED_INT_VEC4: {
      unsigned int* data = reinterpret_cast<unsigned int*>(value);
      gpu(glGetUniformuiv(_program.get_name(),info->get_location(),data));
    }
      break;
#endif
    case GL_BOOL:
      // By convention we expect value to be int here.
      // Although opengl allows setting bools from any type, int, unsigned int, float,
      // We only allow setting it from int.
    case GL_BOOL_VEC2:
    case GL_BOOL_VEC3:
    case GL_BOOL_VEC4: {
      int* data = reinterpret_cast<int*>(value);
      gpu(glGetUniformiv(_program.get_name(),info->get_location(),data));
    }
      break;
    case GL_FLOAT_MAT2:
    case GL_FLOAT_MAT3:
    case GL_FLOAT_MAT4:
#if GLES_MAJOR_VERSION >= 3
    case GL_FLOAT_MAT2x3:
    case GL_FLOAT_MAT2x4:
    case GL_FLOAT_MAT3x2:
    case GL_FLOAT_MAT3x4:
    case GL_FLOAT_MAT4x2:
    case GL_FLOAT_MAT4x3:
#endif
    {
      float* data = reinterpret_cast<float*>(value);
      gpu(glGetUniformfv(_program.get_name(),info->get_location(),data));
    }
      break;
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
      int* data = reinterpret_cast<int*>(value);
      gpu(glGetUniformiv(_program.get_name(),info->get_location(),data));
    }
      break;
    default: {
      std::cerr
          << "Error: DefaultUniformTracker encountered an unhandled GL type: " << info->get_type() << "\n";
      return false;
    }
      break;
  }

  return true;
}

}

