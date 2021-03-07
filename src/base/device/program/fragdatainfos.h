#pragma once
#include <base/device/device_export.h>
#include <base/device/program/fragdatainfo.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace ngs {

class Program;

// Frag Data may be one of the following vector types.
// FLOAT, FLOAT_VEC2, FLOAT_VEC3, FLOAT_VEC4, 
// INT, INT_VEC2, INT_VEC3, INT_VEC4, 
// UNSIGNED_INT, UNSIGNED_INT_VEC2, UNSIGNED_INT_VEC3, or UNSIGNED_INT_VEC4.

// Matrix types.
// FLOAT_MAT2, FLOAT_MAT3, FLOAT_MAT4, FLOAT_MAT2x3, FLOAT_MAT2x4, FLOAT_MAT3x2, FLOAT_MAT3x4, FLOAT_MAT4x2, FLOAT_MAT4x3, 

class DEVICE_EXPORT FragDataInfos {
 public:
  FragDataInfos(Program& program, const std::string& fragment_source);
  virtual ~FragDataInfos();

  size_t get_num_frag_datas() const;
  bool frag_data_exists(const std::string& name) const;
  std::vector<std::string> get_frag_data_names() const;
  const FragDataInfo* get_frag_data_info(const std::string& name) const;
  std::vector<const FragDataInfo*> get_frag_data_info() const;

  static void find_fragment_outputs(const std::string& src, std::vector<std::string>& names, std::vector<std::string>& types);

 private:
  void find_frag_data(const std::string& fragment_source);

  Program& _program;
  std::unordered_map<std::string,FragDataInfo*> _name_to_info;

};

}

