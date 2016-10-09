#pragma once
#include <base/objectmodel/component.h>
#include <guicomponents/quick/quick_export.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <string>

namespace ngs {

class NodeSelection;
class NodeGraphQuickItem;

class QUICK_EXPORT NodeGraphManipulator: public BaseNodeGraphManipulator {
 public:
  COMPONENT_ID(BaseNodeGraphManipulator, NodeGraphManipulator);

  NodeGraphManipulator(Entity* entity);
  virtual ~NodeGraphManipulator();

  virtual void set_processing(Entity* entity);

private:
  Dep<NodeSelection> _node_selection;
  Dep<NodeGraphQuickItem> _ng_quick;

};

}
