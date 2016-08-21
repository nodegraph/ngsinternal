#include <entities/factory.h>
#include <entities/componentinstancer.h>
#include <entities/entityinstancer.h>
#include <base/memoryallocator/taggednew.h>


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

BaseEntityInstancer* Factory::get_entity_instancer() const {
return _entity_instancer;
}

BaseComponentInstancer* Factory::get_component_instancer() const {
  return _component_instancer;
}


}
