#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/basefactory.h>

namespace ngs {

BaseEntityInstancer::~BaseEntityInstancer() {}

BaseComponentInstancer::~BaseComponentInstancer() {}

BaseFactory::BaseFactory(Entity* entity, size_t did)
    : Component(entity, kIID(), did) {
}

Entity* BaseFactory::instance_entity(Entity* parent, const std::string& name, size_t derived_id) const {
  return get_entity_instancer()->instance(parent, name, derived_id);
}

Component* BaseFactory::instance_component(Entity* entity, size_t derived_id) const {
  return get_component_instancer()->instance(entity, derived_id);
}

}
