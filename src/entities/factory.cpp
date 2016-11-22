#include <entities/factory.h>
#include <entities/componentinstancer.h>
#include <entities/entityinstancer.h>
#include <base/memoryallocator/taggednew.h>

#include <base/objectmodel/entityids.h>
#include <components/computes/compute.h>

namespace ngs {

Factory::Factory(Entity* entity):
  BaseFactory(entity, kDID()){
  _entity_instancer = new_ff EntityInstancer();
  _component_instancer = new_ff ComponentInstancer();
}
Factory::~Factory() {
  delete_ff(_component_instancer);
  delete_ff(_entity_instancer);
}

//Entity* Factory::instance_compute_node(Entity* parent, ComponentDID compute_did, const std::string& name) const {
//  std::string node_name;
//  if (name.empty()) {
//    node_name = get_component_user_did_name(compute_did);
//  } else {
//    node_name = name;
//  }
//  ComputeNodeEntity* e = static_cast<ComputeNodeEntity*>(instance_entity(parent, EntityDID::kComputeNodeEntity, node_name));
//  e->set_compute_did(compute_did);
//  return e;
//}
//
//Entity* Factory::instance_input_node(Entity* parent, const QJsonValue& default_value, const std::string& name) const {
//  std::string node_name;
//  if (name.empty()) {
//    node_name = get_component_user_did_name(ComponentDID::kInputNodeCompute);
//  } else {
//    node_name = name;
//  }
//  InputNodeEntity* e = static_cast<InputNodeEntity*>(instance_entity(parent, EntityDID::kInputNodeEntity, node_name));
//  e->set_default_value(default_value);
//  return e;
//}

}
