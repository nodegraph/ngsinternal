#pragma once
#include <base/objectmodel/component.h>
#include <components/computes/computes_export.h>
#include <entities/componentids.h>

#include <string>

namespace ngs {

class BaseInputs: public Component {
 public:

  COMPONENT_ID(BaseInputs, InvalidComponent);

  BaseInputs(Entity* entity, ComponentDID did):Component(entity, kIID(), did){}
  virtual ~BaseInputs(){}

  // Info about our inputs. Note the output info can be found on OutputCompute.
  virtual size_t get_exposed_index(const std::string& input_name) const=0;
  virtual size_t get_num_exposed() const=0;
  virtual size_t get_num_hidden() const=0;
  virtual size_t get_total() const=0;

  virtual const std::unordered_map<std::string, Entity*>& get_hidden() const=0;
  virtual const std::unordered_map<std::string, Entity*>& get_exposed() const=0;
};

}
