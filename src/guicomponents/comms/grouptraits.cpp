#include <guicomponents/comms/grouptraits.h>

namespace ngs {

GroupTraits::GroupTraits(Entity* entity)
    : BaseGroupTraits(entity, kDID()) {

}

GroupTraits::~GroupTraits() {
}

void GroupTraits::on_enter() {
}

void GroupTraits::on_exit() {
}

//bool GroupTraits::node_type_is_permitted(EntityDID did) const {
//}
//
//const std::unordered_set<EntityDID>& GroupTraits::get_permitted_node_types() {
//}

}
