#include <base/objectmodel/lowerhierarchychange.h>
#include <base/objectmodel/entity.h>
#include <base/utils/path.h>

namespace ngs {

LowerHierarchyChange::LowerHierarchyChange(Entity* entity):
  Component(entity, kIID(), kDID()) {
}

LowerHierarchyChange::~LowerHierarchyChange() {
}

void LowerHierarchyChange::update_wires() {
//  _children.clear();
//  for (auto &iter: our_entity()->get_children()) {
//    Dep<LowerHierarchyChange> our_dep = get_dep<LowerHierarchyChange>(iter.second);
//    _children.insert(our_dep);
//  }
}

}
