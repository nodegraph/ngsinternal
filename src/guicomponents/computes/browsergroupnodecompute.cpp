#include <guicomponents/comms/basegrouptraits.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/computes/browsergroupnodecompute.h>

namespace ngs {

BrowserGroupNodeCompute::BrowserGroupNodeCompute(Entity* entity):
    GroupNodeCompute(entity, kDID()),
    _group_traits(this) {
  get_dep_loader()->register_fixed_dep(_group_traits, Path({"."}));
}

BrowserGroupNodeCompute::~BrowserGroupNodeCompute() {
}

bool BrowserGroupNodeCompute::update_state() {
  _group_traits->on_enter();
  bool done = GroupNodeCompute::update_state();
  if (done) {
    _group_traits->on_exit();
  }
  return done;
}

}
