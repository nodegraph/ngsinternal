#include <base/memoryallocator/taggednew.h>
#include <base/device/deviceheadersgl.h>
#include <base/device/program/fragdatainfos.h>
#include <base/device/program/program.h>

#include <base/utils/stringutil.h>

#include <iostream>
#include <regex>

namespace ngs {

FragDataInfos::FragDataInfos(Program& program,
                                 const std::string& fragment_source)
    : _program(program) {
  find_frag_data(fragment_source);
}

FragDataInfos::~FragDataInfos() {
  for (auto &kv: _name_to_info) {
    delete_ff(kv.second);
  }
  _name_to_info.clear();
}

void FragDataInfos::find_frag_data(const std::string& fragment_source) {
  std::vector<std::string> outputs;
  std::vector<std::string> data_types;
  find_fragment_outputs(fragment_source, outputs, data_types);

  for (size_t i = 0; i < outputs.size(); i++) {
    FragDataInfo* type = new_ff FragDataInfo();

    type->name = outputs[i];
#if GLES_MAJOR_VERSION >= 3
    type->location = _program.get_frag_data_location(outputs[i]);
#else
    type->location = 0; // color attachment 0 is the only color attachment position in GLES 2.0
#endif
#if GLES_MAJOR_VERSION >= 100
    type->index = _program.get_frag_data_index(outputs[i]);
#endif

    // Floats.
    if (data_types[i] == "float") {
      type->type = GL_FLOAT;
    } else if (data_types[i] == "vec2") {
      type->type = GL_FLOAT_VEC2;
    } else if (data_types[i] == "vec3") {
      type->type = GL_FLOAT_VEC3;
    } else if (data_types[i] == "vec4") {
      type->type = GL_FLOAT_VEC4;
    }
    // Ints.
    else if (data_types[i] == "int") {
      type->type = GL_INT;
    } else if (data_types[i] == "ivec2") {
      type->type = GL_INT_VEC2;
    } else if (data_types[i] == "ivec3") {
      type->type = GL_INT_VEC3;
    } else if (data_types[i] == "ivec4") {
      type->type = GL_INT_VEC4;
    }
    // Unsigned ints.
    else if (data_types[i] == "uint") {
      type->type = GL_UNSIGNED_INT;
    }
#if GLES_MAJOR_VERSION >= 3
    else if (data_types[i] == "uvec2") {
      type->type = GL_UNSIGNED_INT_VEC2;
    } else if (data_types[i] == "uvec3") {
      type->type = GL_UNSIGNED_INT_VEC3;
    } else if (data_types[i] == "uvec4") {
      type->type = GL_UNSIGNED_INT_VEC4;
    }
#endif
    // Square Matrices.
    else if (data_types[i] == "mat2") {
      type->type = GL_FLOAT_MAT2;
    } else if (data_types[i] == "mat3") {
      type->type = GL_FLOAT_MAT3;
    } else if (data_types[i] == "mat4") {
      type->type = GL_FLOAT_MAT4;
    }
    // Non-square matrices.
#if GLES_MAJOR_VERSION >= 3
    else if (data_types[i] == "mat2x3") {
      type->type = GL_FLOAT_MAT2x3;
    } else if (data_types[i] == "mat2x4") {
      type->type = GL_FLOAT_MAT2x4;
    } else if (data_types[i] == "mat3x2") {
      type->type = GL_FLOAT_MAT3x2;
    } else if (data_types[i] == "mat3x4") {
      type->type = GL_FLOAT_MAT3x4;
    } else if (data_types[i] == "mat4x2") {
      type->type = GL_FLOAT_MAT4x2;
    } else if (data_types[i] == "mat4x3") {
      type->type = GL_FLOAT_MAT4x3;
    }
#endif
    else {
      std::cerr
          << "Error: an unknown fragment shader output type was specified in the glsl: "
          << data_types[i] << "\n";
    }

    _name_to_info[type->name] = type;
  }
}

size_t FragDataInfos::get_num_frag_datas() const {
  return _name_to_info.size();
}

bool FragDataInfos::frag_data_exists(const std::string& name) const {
  return _name_to_info.count(name);
}

std::vector<std::string> FragDataInfos::get_frag_data_names() const {
  std::vector<std::string> names;
  for(auto key : _name_to_info) {
      names.push_back(key.first);
  }
  return names;
}

const FragDataInfo* FragDataInfos::get_frag_data_info(const std::string& name) const {
  if (!frag_data_exists(name)) {
    return NULL;
  }
  return _name_to_info.at(name);
}

std::vector<const FragDataInfo*> FragDataInfos::get_frag_data_info() const {
  std::vector<const FragDataInfo*> infos;
  for(auto key : _name_to_info) {
      infos.push_back(key.second);
  }
  return infos;
}

// Looks for outputs in the fragments shader. An example follows:
// layout(location = 3, index = 1) out vec4 factor;
void FragDataInfos::find_fragment_outputs(const std::string& src,
                                            std::vector<std::string>& names,
                                            std::vector<std::string>& types) {
  // layout(location=0) out vec4 output_color;
//  std::regex out_exp(
//      "layout[[:space:]]*\\([^)]*\\)[[:space:]]*out[[:space:]]+(?<type>[[:word:]]+)[[:space:]]+(?<name>[[:word:]]+)[[:space:]]*;");
  std::regex out_exp(
      "layout[[:space:]]*\\([^)]*\\)[[:space:]]*out[[:space:]]+(\\w+)[[:space:]]+(\\w+)[[:space:]]*;");
  std::string::const_iterator start, end;
  start = src.begin();
  end = src.end();
  std::match_results<std::string::const_iterator> what;
  //std::match_flag_type flags = std::match_default;
  std::regex_constants::match_flag_type flags = std::regex_constants::match_default;
  names.clear();
  while (std::regex_search(start, end, what, out_exp, flags)) {
    std::string type(what[1].first, what[1].second);
    std::string name(what[2].first, what[2].second);
    types.push_back(type);
    names.push_back(name);

    // Update search position. 
    start = what[0].second;
  }

}

}

