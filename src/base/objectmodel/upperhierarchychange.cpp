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

}
