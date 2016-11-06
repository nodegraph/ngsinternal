#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/browsergrouptraits.h>
#include <guicomponents/comms/browserworker.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

BrowserGroupTraits::BrowserGroupTraits(Entity* entity)
    : BaseGroupTraits(entity, kDID()),
      _worker(this) {
  get_dep_loader()->register_fixed_dep(_worker, Path({}));
}

BrowserGroupTraits::~BrowserGroupTraits() {

}

void BrowserGroupTraits::on_enter() {
  std::cerr << "Web group traits on enter\n";
  _worker->open_browser();
}

void BrowserGroupTraits::on_exit() {
  std::cerr << "Web group traits on exit\n";
  _worker->close_browser();
}

//bool GroupTraits::node_type_is_permitted(EntityDID did) const {
//}
//
//const std::unordered_set<EntityDID>& GroupTraits::get_permitted_node_types() {
//}

}
