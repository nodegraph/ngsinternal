#include <guicomponents/computes/grouplock.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>

namespace ngs {

GroupLock::GroupLock(Entity* entity, ComponentIID iid, ComponentDID did)
    : Component(entity, iid, did),
      _inputs(this) {
  get_dep_loader()->register_fixed_dep(_inputs, Path({"."}));
}

GroupLock::~GroupLock() {
}

}
