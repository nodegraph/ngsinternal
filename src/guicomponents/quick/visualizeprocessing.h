#pragma once
#include <base/objectmodel/component.h>
#include <guicomponents/quick/quick_export.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <guicomponents/quick/basevisualizeprocessing.h>
#include <string>

namespace ngs {

class NodeSelection;
class NodeGraphQuickItem;

class QUICK_EXPORT VisualizeProcessing: public BaseVisualizeProcessing {
 public:
  COMPONENT_ID(BaseVisualizeProcessing, VisualizeProcessing);

  VisualizeProcessing(Entity* entity);
  virtual ~VisualizeProcessing();

  virtual void set_processing(Entity* entity);

private:
  Dep<NodeSelection> _node_selection;
  Dep<NodeGraphQuickItem> _ng_quick;

};

}
