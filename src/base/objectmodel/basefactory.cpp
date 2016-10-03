#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/entity.h>

namespace ngs {

BaseEntityInstancer::~BaseEntityInstancer() {}

BaseComponentInstancer::~BaseComponentInstancer() {}

BaseFactory::BaseFactory(Entity* entity, ComponentDID did)
    : Component(entity, kIID(), did) {
}

Entity* BaseFactory::instance_entity(Entity* parent, const std::string& name, EntityDID derived_id) const {
  return _entity_instancer->instance(parent, name, derived_id);
}

Component* BaseFactory::instance_component(Entity* entity, ComponentDID derived_id) const {
  return _component_instancer->instance(entity, derived_id);
}

const char* BaseFactory::get_entity_name_for_did(EntityDID derived_id) const {
  return _entity_instancer->get_name_for_did(derived_id);
}

ComponentIID BaseFactory::get_component_iid(ComponentDID did) const {
  return _component_instancer->get_iid_for_did(did);
}

Entity* BaseFactory::create_entity(Entity* parent, const std::string& name, EntityDID did) const {
  Entity* e = instance_entity(parent, name, did);
  e->create_internals();
  return e;
}

Component* BaseFactory::create_component(Entity* entity, ComponentDID did) const {
  Component* c = instance_component(entity, did);
  return c;
}

void BaseFactory::push_group(Entity* group) {
  internal();
  _group_stack.push_back(group);
}

void BaseFactory::pop_group() {
  internal();
  // The last group should not be popped.
  if (_group_stack.size()<=1) {
    return;
  }
  // Otherwise pop it.
  _group_stack.pop_back();
}

Entity* BaseFactory::get_current_group() const {
  external();
  return _group_stack.back();
}

bool BaseFactory::no_current_group() const {
  if (_group_stack.empty()) {
    return true;
  }
  return false;
}

size_t BaseFactory::get_group_stack_depth() const {
  return _group_stack.size();
}

void BaseFactory::clear_group_stack() {
  _group_stack.clear();
}

}
