#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <entities/entityids.h>
namespace ngs {

class Entity;
class Compute;

class OBJECTMODEL_EXPORT BaseEntityInstancer {
 public:
  virtual ~BaseEntityInstancer();
  virtual Entity* instance(Entity* parent, const std::string& name, EntityDID did) const = 0;
  virtual EntityIID get_iid(EntityDID did) const = 0;
  virtual const char* get_name_for_did(EntityDID did) const = 0;
};

class OBJECTMODEL_EXPORT BaseComponentInstancer {
 public:
  virtual ~BaseComponentInstancer();
  virtual Component* instance(Entity* entity, ComponentDID did) const = 0;
  virtual ComponentIID get_iid_for_did(ComponentDID did) const = 0;
  virtual const char* get_compute_name(ComponentDID compute_did) const = 0;
};

class OBJECTMODEL_EXPORT BaseFactory: public Component {
 public:

  COMPONENT_ID(BaseFactory, InvalidComponent);

  BaseFactory(Entity* entity, ComponentDID did);
  virtual ~BaseFactory() {}

  virtual Entity* instance_entity(Entity* parent, const std::string& name, EntityDID did) const;
  virtual Component* instance_component(Entity* entity, ComponentDID did) const;

  virtual const char* get_entity_name_for_did(EntityDID did) const;
  virtual ComponentIID get_component_iid(ComponentDID did) const;

  virtual Entity* create_entity(Entity* parent, const std::string& name, EntityDID did) const;
  virtual Component* create_component(Entity* entity, ComponentDID did) const;

  virtual const char* get_compute_name(ComponentDID compute_did) const;

  virtual Entity* instance_compute_node(Entity* parent, ComponentDID compute_did, const std::string& name="") const = 0;
  virtual Entity* create_compute_node(Entity* parent, ComponentDID compute_did, const std::string& name="") const;

  // Group navigation management.
  virtual void push_group(Entity* group);
  virtual void pop_group();
  virtual Entity* get_current_group() const;
  virtual bool no_current_group() const;
  virtual size_t get_group_stack_depth() const;
  virtual void clear_group_stack();

 protected:
  BaseEntityInstancer* _entity_instancer;
  BaseComponentInstancer* _component_instancer;

  // Our group stack.
  std::vector<Entity*> _group_stack;

};



}
