#pragma once
#include <base/objectmodel/basefactory.h>
#include <components/entities/entities_export.h>

namespace ngs {

class Component;

class ENTITIES_EXPORT ComponentInstancer: public BaseComponentInstancer {
 public:
  ComponentInstancer(){}
  virtual ~ComponentInstancer(){}
  virtual Component* instance(Entity* entity, size_t derived_id) const;
  virtual size_t get_loadable_iid(size_t derived_id) const;
};

}
