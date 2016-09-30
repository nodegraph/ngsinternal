#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>

namespace ngs {

class Entity;

class OBJECTMODEL_EXPORT BaseEntityInstancer {
 public:
  virtual ~BaseEntityInstancer();
  virtual Entity* instance(Entity* parent, const std::string& name, size_t did) const = 0;
  virtual size_t get_iid(size_t did) const = 0;
  virtual const char* get_name_for_did(size_t did) const = 0;
};

class OBJECTMODEL_EXPORT BaseComponentInstancer {
 public:
  virtual ~BaseComponentInstancer();
  virtual Component* instance(Entity* entity, size_t did) const = 0;
  virtual size_t get_iid_for_did(size_t did) const = 0;
};

class OBJECTMODEL_EXPORT BaseFactory: public Component {
 public:

  COMPONENT_ID(BaseFactory, InvalidComponent);

  BaseFactory(Entity* entity, size_t did);
  virtual ~BaseFactory() {}

  virtual Entity* instance_entity(Entity* parent, const std::string& name, size_t did) const;
  virtual Component* instance_component(Entity* entity, size_t did) const;

  virtual const char* get_entity_name_for_did(size_t did) const;
  virtual size_t get_component_iid(size_t did) const;

  virtual Entity* create_entity(Entity* parent, const std::string& name, size_t did) const;
  virtual Component* create_component(Entity* entity, size_t did) const;
  virtual Entity* create_compute_node(Entity* parent, size_t compute_did, const std::string& name="") const = 0;

 protected:
  BaseEntityInstancer* _entity_instancer;
  BaseComponentInstancer* _component_instancer;

};



}
