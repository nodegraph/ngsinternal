#include <guicomponents/quick/nodegraphmanipulator.h>
#include <components/compshapes/nodeselection.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <base/objectmodel/deploader.h>

namespace ngs {

NodeGraphManipulator::NodeGraphManipulator(Entity* entity):
    BaseNodeGraphManipulator(entity, kDID()),
    _node_selection(this),
    _ng_quick(this) {
  get_dep_loader()->register_fixed_dep(_node_selection, Path({}));
  get_dep_loader()->register_fixed_dep(_ng_quick, Path({}));
}

NodeGraphManipulator::~NodeGraphManipulator(){
}

void NodeGraphManipulator::set_processing(Entity* entity) {
  _node_selection->set_compute_node_entity(entity);
  _ng_quick->update();
}


}
