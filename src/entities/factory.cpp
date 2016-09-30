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

Entity* Factory::create_compute_node(Entity* parent, size_t compute_did, const std::string& name) const {
  std::string node_name;
  if (name.empty()) {
    node_name = get_compute_name(compute_did);
  } else {
    node_name = name;
  }
  Entity* e = create_entity(parent, node_name, kComputeNodeEntity);
  Compute* c = static_cast<Compute*>(create_component(e, compute_did));
  c->create_inputs_outputs();
  return e;
}

}
