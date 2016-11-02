#include <entities/factory.h>
#include <entities/componentinstancer.h>
#include <entities/entityinstancer.h>
#include <base/memoryallocator/taggednew.h>

#include <entities/entityids.h>
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

Entity* Factory::instance_compute_node(Entity* parent, ComponentDID compute_did, const std::string& name) const {
  std::string node_name;
  if (name.empty()) {
    node_name = get_compute_name(compute_did);
  } else {
    node_name = name;
  }
  ComputeNodeEntity* e = static_cast<ComputeNodeEntity*>(instance_entity(parent, node_name, EntityDID::kComputeNodeEntity));
  e->set_compute_did(compute_did);
  return e;
}


}
