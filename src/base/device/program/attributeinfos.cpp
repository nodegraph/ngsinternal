#include <base/memoryallocator/taggednew.h>
#include <base/device/program/attributeinfos.h>
#include <base/device/program/program.h>

#include <iostream>

namespace ngs {

AttributeInfos::AttributeInfos(Program& program)
    : _program(program) {
  find_attributes();
}

AttributeInfos::~AttributeInfos() {
  for (auto &kv: _name_to_info) {
    delete_ff(kv.second);
  }
  _name_to_info.clear();
}

void AttributeInfos::find_attributes() {
  // Query the number of attributes.
  int num_attributes = _program.get_num_attributes();
  //std::cerr<<"The number of active attributes is: "<<num_attributes<<"\n";

  // Get the vertex attributes.
  for (int i = 0; i < num_attributes; i++) {
    AttributeInfo* info = _program.create_attribute_info(i);
    //std::cerr << "adding name: -->" << type.name << "<--\n";
    _name_to_info[info->name]= info;
  }
}

bool AttributeInfos::attribute_exists(const std::string& name) const {
  return _name_to_info.count(name);
}

std::vector<std::string> AttributeInfos::get_attribute_names() const {
  std::vector<std::string> names;
  for(auto key : _name_to_info) {
      names.push_back(key.first);
  }
  return names;
}

const AttributeInfo* AttributeInfos::get_attribute_info(const std::string& name) const {
  if (!attribute_exists(name)) {
    return NULL;
  }
  return _name_to_info.at(name);
}

std::vector<const AttributeInfo*> AttributeInfos::get_attribute_infos() const {
  std::vector<const AttributeInfo*> infos;
  infos.reserve(_name_to_info.size());
  for (auto &kv: _name_to_info) {
    infos.push_back(kv.second);
  }
  return infos;
}

bool AttributeInfos::set_attribute_location(const std::string& name,
                                              GLuint location) {
  const AttributeInfo *type = _name_to_info[name];

  // Modify the location and record it.
  AttributeInfo* changed = new_ff AttributeInfo(*type);
  changed->location = location;
  // Destroy the previous attribute info.
  delete_ff(_name_to_info[name]);
  // Record the novel attribute info.
  _name_to_info[name] = changed;

  // Modify the program.
  _program.set_attribute_location(changed);
  return true;
}

GLuint AttributeInfos::get_attribute_location(const std::string& name) const {
// Use this to print the keys.
//  std::vector<std::string> keys;
//  _name_to_type.get_keys(keys);
//  for (size_t i=0; i<keys.size(); ++i) {
//    std::cerr << i << ":-->" << keys[i] << "<--\n";
//  }

  if (!attribute_exists(name)) {
    return -1;
  }

  const AttributeInfo *type = _name_to_info.at(name);
  return type->location;
}

void AttributeInfos::update_program() {
  _program.link();
}

}

