#include <base/objectmodel/upperhierarchychange.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/dep.h>

namespace ngs {

UpperHierarchyChange::UpperHierarchyChange(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _parent_change(this) {
  get_dep_loader()->register_fixed_dep(_parent_change, "..");
}

UpperHierarchyChange::~UpperHierarchyChange() {
}

//void UpperHierarchyChange::initialize_fixed_deps() {
//  Component::initialize_fixed_deps();
//  update_state();
//}
//
//void UpperHierarchyChange::update_state() {
//  if (has_entity("..")) {
//    _parent_change = get_dep<UpperHierarchyChange>(get_entity(".."));
//  } else {
//    _parent_change.reset();
//  }
//}

}
