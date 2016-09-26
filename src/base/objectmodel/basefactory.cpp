#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/basefactory.h>

namespace ngs {

BaseEntityInstancer::~BaseEntityInstancer() {}

BaseComponentInstancer::~BaseComponentInstancer() {}

BaseFactory::BaseFactory(Entity* entity, size_t did)
    : Component(entity, kIID(), did) {
}

Entity* BaseFactory::instance_entity(Entity* parent, const std::string& name, size_t derived_id) const {
  return _entity_instancer->instance(parent, name, derived_id);
}

Component* BaseFactory::instance_component(Entity* entity, size_t derived_id) const {
  return _component_instancer->instance(entity, derived_id);
}

const char* BaseFactory::get_entity_name_for_did(size_t derived_id) const {
  return _entity_instancer->get_name_for_did(derived_id);
}

size_t BaseFactory::get_component_iid(size_t did) const {
  return _component_instancer->get_iid_for_did(did);
}

}
