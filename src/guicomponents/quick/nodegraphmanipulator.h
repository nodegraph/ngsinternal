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
class QUICK_EXPORT NodeGraphManipulator : public BaseNodeGraphManipulator {
 public:
  COMPONENT_ID(BaseNodeGraphManipulator, NodeGraphManipulator)

  NodeGraphManipulator(Entity* entity);
  virtual ~NodeGraphManipulator();

  // Asynchronous Component Cleaning.
  virtual void set_ultimate_target(Entity* entity, bool force_stack_reset);
  virtual void clear_ultimate_target();
  virtual void continue_cleaning_to_ultimate_target();

  // Update current compute markers on nodes.
  virtual void set_processing_node(Entity* entity);
  virtual void clear_processing_node();
  virtual void set_error_node(); // Show error marker on the current compute node.
  virtual void clear_error_node();
  virtual void update_clean_marker(Entity* entity, bool clean);

  // Build and link a compute node.
  virtual Entity* build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);

  // Update inputs and outputs configuration for the gui side.
  virtual void set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);
  virtual void set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);

 protected:
  virtual void initialize_wires();

 private:
  NodeGraphManipulatorImp* _imp;
};

}
