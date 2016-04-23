#pragma once
#include <components/entities/entities_export.h>
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

  virtual BaseEntityInstancer* get_entity_instancer() const;
  virtual BaseComponentInstancer* get_component_instancer() const;

 private:

  // Our instancing helpers.
  BaseEntityInstancer* _entity_instancer;
  BaseComponentInstancer* _component_instancer;
};

}
