#pragma once
#include <base/objectmodel/component.h>
#include <guicomponents/quick/quick_export.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <string>

#include <QtCore/QVariantMap>

namespace ngs {

class NodeGraphManipulatorImp;

// This class wraps an internal Component held by a raw pointer.
// This allows it to break dependency cycles.
class QUICK_EXPORT NodeGraphManipulator: public BaseNodeGraphManipulator {
 public:
  COMPONENT_ID(BaseNodeGraphManipulator, NodeGraphManipulator);

  NodeGraphManipulator(Entity* entity);
  virtual ~NodeGraphManipulator();

  virtual void initialize_wires();

  virtual void set_compute_node(Entity* entity);
  virtual void clear_compute_node();

  virtual Entity* build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);

private:
  NodeGraphManipulatorImp* _imp;
};

}
