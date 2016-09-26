#pragma once
#include <base/objectmodel/basefactory.h>
#include <entities/entities_export.h>

namespace ngs {

class Component;

class ENTITIES_EXPORT ComponentInstancer: public BaseComponentInstancer {
 public:
  ComponentInstancer(){}
  virtual ~ComponentInstancer(){}
  virtual Component* instance(Entity* entity, size_t did) const;
  virtual size_t get_iid_for_did(size_t did) const;
};

}
