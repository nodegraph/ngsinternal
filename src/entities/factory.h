#pragma once
#include <entities/entities_export.h>
#include <base/objectmodel/basefactory.h>

namespace ngs {

class Entity;
class BaseEntityInstancer;
class BaseComponentInstancer;

class ENTITIES_EXPORT Factory: public BaseFactory {
 public:

  COMPONENT_ID(BaseFactory, Factory);

  Factory(Entity* entity);
  virtual ~Factory();

  virtual Entity* create_compute_node(Entity* parent, size_t compute_did, const std::string& name="") const;
};

}
