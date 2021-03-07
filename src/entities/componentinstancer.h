#pragma once
#include <base/objectmodel/basefactory.h>
#include <entities/entities_export.h>

namespace ngs {

class Component;

class ENTITIES_EXPORT ComponentInstancer: public BaseComponentInstancer {
 public:
  static Component* instance_imp(Entity* entity, ComponentDID did);

  ComponentInstancer(){}
  virtual ~ComponentInstancer(){}
  virtual Component* instance(Entity* entity, ComponentDID did) const;
  virtual ComponentIID get_iid_for_did(ComponentDID did) const;
};

}
