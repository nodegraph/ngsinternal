#include <base/objectmodel/lowerhierarchychange.h>
#include <base/objectmodel/entity.h>
#include <base/utils/path.h>

namespace ngs {

LowerHierarchyChange::LowerHierarchyChange(Entity* entity):
  Component(entity, kIID(), kDID()) {
}

LowerHierarchyChange::~LowerHierarchyChange() {
}

void LowerHierarchyChange::initialize_fixed_deps() {
  Component::initialize_fixed_deps();
  update_state();
}

void LowerHierarchyChange::update_state() {
  _child_changes.clear();
  for (auto &iter: our_entity()->get_children()) {
    // Take ownership of the deps.
    Dep<LowerHierarchyChange> our_dep(this);
    our_dep = get_dep<LowerHierarchyChange>(iter.second);
    _child_changes.insert(our_dep);
  }
}

}
