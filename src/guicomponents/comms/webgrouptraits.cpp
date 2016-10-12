#include <guicomponents/comms/webgrouptraits.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/webworker.h>

namespace ngs {

WebGroupTraits::WebGroupTraits(Entity* entity)
    : BaseGroupTraits(entity, kDID()),
      _web_worker(this) {
  get_dep_loader()->register_fixed_dep(_web_worker, Path({}));
}

WebGroupTraits::~WebGroupTraits() {

}

void WebGroupTraits::on_enter() {
  std::cerr << "Web group traits on enter\n";
  _web_worker->open_browser();
  //_web_worker->start_polling();
}

void WebGroupTraits::on_exit() {
  std::cerr << "Web group traits on exit\n";
  _web_worker->close_browser();
  //_web_worker->stop_polling();
}

//bool GroupTraits::node_type_is_permitted(EntityDID did) const {
//}
//
//const std::unordered_set<EntityDID>& GroupTraits::get_permitted_node_types() {
//}

}
