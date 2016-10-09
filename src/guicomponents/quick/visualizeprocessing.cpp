#include <guicomponents/quick/visualizeprocessing.h>
#include <components/compshapes/nodeselection.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <base/objectmodel/deploader.h>

namespace ngs {

VisualizeProcessing::VisualizeProcessing(Entity* entity):
    BaseVisualizeProcessing(entity, kDID()),
    _node_selection(this),
    _ng_quick(this) {
  get_dep_loader()->register_fixed_dep(_node_selection, Path({}));
  get_dep_loader()->register_fixed_dep(_ng_quick, Path({}));
}

VisualizeProcessing::~VisualizeProcessing(){
}

void VisualizeProcessing::set_processing(Entity* entity) {
  _node_selection->set_processing_node_entity(entity);
  _ng_quick->update();
}


}
