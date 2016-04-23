#pragma once
#include <base/device/device_export.h>
#include <base/device/program/attributeinfo.h>
#include <vector>
#include <unordered_map>

namespace ngs {

class Program;

#pragma warning(push)
#pragma warning(disable:4251)

class DEVICE_EXPORT AttributeInfos {
 public:
  AttributeInfos(Program& program);
  virtual ~AttributeInfos();

  // Existence.
  bool attribute_exists(const std::string& name) const;

  // Info.
  std::vector<std::string> get_attribute_names() const;
  const AttributeInfo* get_attribute_info(const std::string& name) const;
  std::vector<const AttributeInfo*> get_attribute_infos() const;
  GLuint get_attribute_location(const std::string& name) const;

  // Change an attribute's location.
  // After an attribute's location is changed the program must be relink.
  bool set_attribute_location(const std::string& name, GLuint location);
  void update_program();

 private:
  void find_attributes();

  Program& _program;
  std::unordered_map<std::string,AttributeInfo*> _name_to_info; // We own the AttributeInfo's.

};
#pragma warning(pop)

}

