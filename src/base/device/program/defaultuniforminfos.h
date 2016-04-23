#pragma once
#include <base/device/device_export.h>
#include <base/device/program/defaultuniforminfo.h>
#include <vector>
#include <unordered_map>

namespace ngs {

class Program;

class DEVICE_EXPORT DefaultUniformInfos {
 public:
  DefaultUniformInfos(Program& program);
  virtual ~DefaultUniformInfos();

  bool uniform_exists(const std::string& name) const;
  std::vector<std::string> get_uniform_names() const;
  const DefaultUniformInfo* get_uniform_info(const std::string& name) const;
  std::vector<const DefaultUniformInfo*> get_uniform_infos() const;

  // Note bools and samplers are expected to be ints.
  bool set_uniform(const DefaultUniformInfo* info, const void* value);
  bool get_uniform(const DefaultUniformInfo* info, void* value);

 private:

  Program& _program;

  std::unordered_map<std::string,DefaultUniformInfo*> _name_to_info; // We own the DefaultUniformInfo's.

};

}
