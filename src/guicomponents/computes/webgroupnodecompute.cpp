#include <guicomponents/comms/basegrouptraits.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/computes/webgroupnodecompute.h>

namespace ngs {

WebGroupNodeCompute::WebGroupNodeCompute(Entity* entity):
    GroupNodeCompute(entity, kDID()),
    _group_traits(this) {
  get_dep_loader()->register_fixed_dep(_group_traits, Path({"."}));
}

WebGroupNodeCompute::~WebGroupNodeCompute() {
}

bool WebGroupNodeCompute::update_state() {
  _group_traits->on_enter();
  bool done = GroupNodeCompute::update_state();
  if (done) {
    _group_traits->on_exit();
  }
  return done;
}

//void WebGroupNodeCompute::dirty_web_computes() {
//  dirty_all_node_computes();
//}

}
