#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>

namespace ngs {

class Entity;

class OBJECTMODEL_EXPORT BaseEntityInstancer {
 public:
  virtual ~BaseEntityInstancer();
  virtual Entity* instance(Entity* entity, const std::string& name, size_t derived_id) const = 0;
  virtual size_t get_iid(size_t derived_id) const = 0;
  virtual const char* get_did_name(size_t derived_id) const = 0;
};

class OBJECTMODEL_EXPORT BaseComponentInstancer {
 public:
  virtual ~BaseComponentInstancer();
  virtual Component* instance(Entity* entity, size_t derived_id) const = 0;
  virtual size_t get_loadable_iid(size_t derived_id) const = 0;
};

class OBJECTMODEL_EXPORT BaseFactory: public Component {
 public:

  COMPONENT_ID(BaseFactory, InvalidComponent);

  BaseFactory(Entity* entity, size_t did);
  virtual ~BaseFactory() {}

  virtual BaseEntityInstancer* get_entity_instancer() const = 0;
  virtual BaseComponentInstancer* get_component_instancer() const = 0;
};



}
