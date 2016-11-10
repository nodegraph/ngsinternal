#include <guicomponents/quick/nodegraphmanipulator.h>
#include <components/compshapes/nodeselection.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/basegrouptraits.h>
#include <guicomponents/computes/mqttcomputes.h>

// ObjectModel.
#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

// Computes.
#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/groupnodecompute.h>
#include <components/computes/outputs.h>

// CompShapes.
#include <components/compshapes/topology.h>
#include <components/compshapes/nodeshape.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/compshapecollective.h>

namespace ngs {

// This is the implementation class for the NodeGraphManipulator.
// It is held by a raw pointer in NodeGraphManipulator to avoid dependency cycles.
// This allows us to call this from the non-gui side (eg computes) at arbitrary
// locations to update the gui side to reflect non-gui side changes.
class NodeGraphManipulatorImp: public Component {
 public:
  // Note that components with and IID of kIInvalidComponent, should not be
  // created with the app root as its parent as there may be other invalid components
  // which also get created later resulting in collisions.
  // Invalid components are always created under a null entity.
  // Note that an invalid component usually needs a reference to the app root entity
  // in order create Dep<>s to other components.
  COMPONENT_ID(InvalidComponent, InvalidComponent);
  NodeGraphManipulatorImp(Entity* app_root);

  virtual void initialize_wires();
  
  // Asynchronous Component Cleaning.
  virtual void set_ultimate_target(Entity* entity, bool force_stack_reset = false);
  virtual void clear_ultimate_target();
  virtual void continue_cleaning_to_ultimate_target();
  virtual bool is_busy_cleaning();

  // Update current compute markers on nodes.
  virtual void set_processing_node(Entity* entity);
  virtual void clear_processing_node();
  virtual void set_error_node(const QString& error_message);
  virtual void clear_error_node();
  virtual void update_clean_marker(Entity* entity, bool clean);

  virtual void dive_into_lockable_group(const std::string& child_group_name);
  virtual void clean_lockable_group(const std::string& child_group_name);

  // Builds and positions a compute node under the lowest node in the node graph.
  // If possible it will also link the latest node with the lowest.
  Entity* build_and_link_compute_node(ComponentDID compute_did, const QJsonObject& chain_state);

  // Update inputs and outputs configuration for the gui side.
  virtual void set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);
  virtual void set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering);

  // Modify links..
  virtual void destroy_link(Entity* input_entity);
  virtual Entity* create_link();
  virtual Entity* connect_plugs(Entity* input_entity, Entity* output_entity);

  virtual void synchronize_graph_dirtiness(Entity* group_entity);
  virtual void synchronize_group_dirtiness(Entity* group_entity);
  virtual void dirty_group_from_internals(Entity* group_entity);
  virtual void dirty_internals_from_group(Entity* group_entity);

  virtual void set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload);

 private:
  Entity* build_compute_node(ComponentDID compute_did, const QJsonObject& chain_state);
  void link(Entity* downstream);

  Entity* _app_root;
  Dep<BaseFactory> _factory;
  Dep<NodeSelection> _node_selection;
  Dep<NodeGraphQuickItem> _ng_quick;
  Dep<TaskScheduler> _scheduler;

  // The ultimate compute (of a node) that we are trying to clean.
  // Note that there maybe many asynchronous computes which cause each cleaning pass over the dependencies
  // to finish early (returning false). Holding this reference to the ultimate component we want to clean
  // allows us to restart the cleaning process once other asynchronous cleaning processes finish.
  Dep<Compute> _ultimate_target;
};



// -----------------------------------------------------------------------------------
// NodeGraphManipulatorImp.
// -----------------------------------------------------------------------------------

// Note that the NodeGraphManipulatorImp Component gets created outside the app_root hierarchy.
// The NodeGraphManipulatorImp Component has no parent and in a sense is its own root.
// Because it has no parent, you must explicitly hold a pointer to this when
// allocated so that you can manipulate it and delete it.
NodeGraphManipulatorImp::NodeGraphManipulatorImp(Entity* app_root)
    : Component(NULL, kIID(), kDID()),
      _app_root(app_root),
      _factory(this),
      _node_selection(this),
      _ng_quick(this),
      _scheduler(this),
      _ultimate_target(this) {
}

void NodeGraphManipulatorImp::initialize_wires() {
  Component::initialize_wires();

  _factory = get_dep<BaseFactory>(_app_root);
  _node_selection = get_dep<NodeSelection>(_app_root);
  _ng_quick = get_dep<NodeGraphQuickItem>(_app_root);
  _scheduler = get_dep<TaskScheduler>(_app_root);
}

void NodeGraphManipulatorImp::set_ultimate_target(Entity* entity, bool force_stack_reset) {
  external();

  if (!get_dep<Compute>(entity)->is_state_dirty()) {
    return;
  }

  // Synchronize the dirtiness of all groups with the internal contents.
  // This is needed because a group maintains it's own dirtiness separately from the internal nodes,
  // and the user can manually change the dirtiness of the internal nodes or the group.
  // This consolidates those dirtiness states, so the we perform the compute properly.
  // Note that nodes in the currently are not dirtied because the user is likely adding/editing nodes
  // and we don't want to have to keep recomputing all the upstream nodes in this case.
  // All other groups get their dirtiness synchronized.
  synchronize_graph_dirtiness(_app_root);

  // Clear the error marker on nodes.
  _node_selection->clear_error_node();
  // This may be called while we are already trying to clean another ultimate target.
  // Hence we force a stack reset to clear out any pre-existing tasks.
  if (force_stack_reset) {
    _scheduler->force_stack_reset();
  }
  // Set and try cleaning the ultimate target.
  _ultimate_target = get_dep<Compute>(entity);

  _app_root->clean_wires();

  continue_cleaning_to_ultimate_target();
}

void NodeGraphManipulatorImp::clear_ultimate_target() {
  external();
  std::cerr << "clearing ultimate target\n";
  _ultimate_target.reset();
}

void NodeGraphManipulatorImp::continue_cleaning_to_ultimate_target() {
  external();

  // Return if there is no ultimate target set.
  if (!_ultimate_target) {
    return;
  }

  // Return if the ultimate target is already clean, and wipe out the ultimate target.
  if (!_ultimate_target->is_state_dirty()) {
    _ultimate_target.reset();
    return;
  }

  // Clean all the inputs on all the groups down to the ultimate target.
  // The target path holds the path down to the immediately surrounding group of the target.
  Path target_path = _ultimate_target->our_entity()->get_path();
  target_path.pop_back();
  Path path({});

  do {
    path.push_back(target_path.front());
    target_path.pop_front();
    std::cerr << "working on group inputs: " << path.get_as_string() << "\n";

    Entity* e = _app_root->get_entity(path);
    Dep<GroupNodeCompute> c = get_dep<GroupNodeCompute>(e);
    if (!c->clean_inputs()) {
      return;
    }
  } while (target_path.size());

  // If we get here the inputs on our surrounding group and our input nodes
  // now have appropriate values to perform the compute.
  if (_ultimate_target->clean_state()) {
    // Reset the ultimate target if we can clean it right away.
    _ultimate_target.reset();
  }
}

bool NodeGraphManipulatorImp::is_busy_cleaning() {
  external();
  if (_ultimate_target) {
    return true;
  }
  return false;
}

void NodeGraphManipulatorImp::set_processing_node(Entity* entity) {
  _node_selection->set_processing_node_entity(entity);
  _ng_quick->update();
}

void NodeGraphManipulatorImp::clear_processing_node() {
  _node_selection->clear_processing_node();
  _ng_quick->update();
}

void NodeGraphManipulatorImp::set_error_node(const QString& error_message) {
  _ng_quick->set_error_node(error_message);
}

void NodeGraphManipulatorImp::clear_error_node() {
  _node_selection->clear_error_node();
  _ng_quick->update();
}

void NodeGraphManipulatorImp::update_clean_marker(Entity* entity, bool clean) {
  Dep<NodeShape> ns = get_dep<NodeShape>(entity);
  if (ns) {
	  ns->show_clean_marker(clean);
	  _ng_quick->update();
  }

  // Make sure not to call qApp->processEvents() here.
  // Because this causes problems when destroying entities.
  // When components get destroyed they will destroy their Dep<> objects,
  // which dirties objects. However the process events will generally
  // have events to update the display which will actually start cleaning
  // components. But now it is cleaning in a bad state.
}

void NodeGraphManipulatorImp::dive_into_lockable_group(const std::string& child_group_name) {
  _ng_quick->dive_into_lockable_group(child_group_name);
}

void NodeGraphManipulatorImp::clean_lockable_group(const std::string& child_group_name) {
  _ng_quick->clean_lockable_group(child_group_name);
}

Entity* NodeGraphManipulatorImp::build_and_link_compute_node(ComponentDID compute_did, const QJsonObject& chain_state) {
  Entity* node = build_compute_node(compute_did, chain_state);
  link(node);
  return node;
}

void NodeGraphManipulatorImp::set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) {
  Dep<InputTopology> topo = get_dep<InputTopology>(entity);
  topo->set_topology(ordering);
}

void NodeGraphManipulatorImp::set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) {
  Dep<OutputTopology> topo = get_dep<OutputTopology>(entity);
  topo->set_topology(ordering);
}

Entity* NodeGraphManipulatorImp::build_compute_node(ComponentDID compute_did, const QJsonObject& chain_state) {
  // Create the node.
  Entity* group = _factory->get_current_group();
  Entity* _node = _factory->create_compute_node(group, compute_did);

  // Initialize and update the wires.
  _node->initialize_wires();
  group->clean_wires();

  // Set the values on all the inputs from the chain_state.
  for (QJsonObject::const_iterator iter = chain_state.constBegin(); iter != chain_state.constEnd(); ++iter) {
    // Find the input entity.
    Path path( { ".", "inputs" });
    path.push_back(iter.key().toStdString());
    Entity* input_entity = _node->has_entity(path);
    // Skip this key if the entity doesn't exist.
    if (!input_entity) {
      continue;
    }
    // Get the compute.
    Dep<InputCompute> compute = get_dep<InputCompute>(input_entity);
    // Skip inputs which are plugs and not params.
    if (compute->is_exposed()) {
      continue;
    }

    std::cerr << "setting name: " << iter.key().toStdString() << " value: " << iter.value().toString().toStdString() << "\n";
    compute->set_unconnected_value(iter.value());
  }

  return _node;
}

void NodeGraphManipulatorImp::link(Entity* downstream) {
  // Get the factory.
  Dep<BaseFactory> factory = get_dep<BaseFactory>(_app_root);
  Entity* current_group = factory->get_current_group();

  // Get the current comp shape collective.
  Dep<CompShapeCollective> collective = get_dep<CompShapeCollective>(current_group);

  // Find the lowest node that's where we'll link to.
  Dep<CompShape> upstream_node = collective->get_lowest( { downstream });
  if (!upstream_node) {
    return;
  }
  Entity* upstream = upstream_node->our_entity();

  // -------------------------------------------------------------------------
  // At this point we have both the upstream and downstream nodes to connect.
  // -------------------------------------------------------------------------

  // Position our node below the upstream.
  Dep<NodeShape> downstream_node = get_dep<NodeShape>(downstream);
  glm::vec2 pos = upstream_node->get_pos();
  pos.y -= 700;
  downstream_node->set_pos(pos);

  // Get the exposed outputs from the upstream node.
  Dep<Outputs> outputs = get_dep<Outputs>(upstream);
  const std::unordered_map<std::string, Dep<OutputCompute> >& exposed_outputs = outputs->get_exposed();

  // Get the exposed inputs from the downstream node.
  Dep<Inputs> inputs = get_dep<Inputs>(downstream);
  const std::unordered_map<std::string, Dep<InputCompute> >& exposed_inputs = inputs->get_exposed();

  // If we have exposed outputs and inputs then lets connect the first of each of them.
  if (!exposed_outputs.empty() && !exposed_inputs.empty()) {
    Dep<OutputCompute> output_compute = exposed_outputs.begin()->second;
    Dep<InputCompute> input_compute = exposed_inputs.begin()->second;

    // Link the computes.
    input_compute->link_output_compute(output_compute);

    // Create a link.
    Entity* links_folder = current_group->get_entity(Path( { ".", "links" }));
    Entity* link = factory->instance_entity(links_folder, "link", EntityDID::kLinkEntity);
    link->create_internals();

    // Link the link, input and output shapes.
    Dep<OutputShape> output_shape = get_dep<OutputShape>(output_compute->our_entity());
    Dep<InputShape> input_shape = get_dep<InputShape>(input_compute->our_entity());
    Dep<LinkShape> link_shape = get_dep<LinkShape>(link);
    link_shape->start_moving();
    link_shape->link_input_shape(input_shape);
    link_shape->link_output_shape(output_shape);
    link_shape->finished_moving();
  }

  // Clean the wires in this group, as new nodes and links were created.
  _factory->get_current_group()->clean_wires();
}

void NodeGraphManipulatorImp::destroy_link(Entity* input_entity) {
  // Unlink the computes.
  Dep<InputCompute> input_compute = get_dep<InputCompute>(input_entity);
  input_compute->unlink_output_compute();

  // Unlink the gui shapes.
  Dep<InputShape> input_shape = get_dep<InputShape>(input_entity);
  Entity* link_entity = input_shape->find_link_entity();
  assert(link_entity);
  delete_ff(link_entity);
}

Entity* NodeGraphManipulatorImp::create_link() {
  Entity* group = _factory->get_current_group();
  Entity* links_folder = group->get_entity(Path({".","links"}));
  Entity* link = _factory->instance_entity(links_folder, "link", EntityDID::kLinkEntity);
  link->create_internals();

  Dep<LinkShape> dep = get_dep<LinkShape>(link);
  dep->start_moving(); // This prevents the link from getting destroyed.
  return link;
}

Entity* NodeGraphManipulatorImp::connect_plugs(Entity* input_entity, Entity* output_entity) {
  Dep<InputCompute> input_compute = get_dep<InputCompute>(input_entity);
  Dep<InputShape> input_shape = get_dep<InputShape>(input_entity);
  Dep<OutputCompute> output_compute = get_dep<OutputCompute>(output_entity);
  Dep<OutputShape> output_shape = get_dep<OutputShape>(output_entity);

  // Remove any existing link shapes on this input.
  // There is only one link per input.
  Entity* old_link_entity = input_shape->find_link_entity();
  if (old_link_entity) {
    // Remove any existing compute connection on this input compute.
    input_compute->unlink_output_compute();
    // Destroy the link.
    delete_ff(old_link_entity);
  }

  // Try to connect the input and output computes.
  if (!input_compute->link_output_compute(output_compute)) {
    // Otherwise destroy the link and selection.
    assert(false);
  }

  // Link the output shape.
  Entity* link = create_link();
  Dep<LinkShape> link_shape = get_dep<LinkShape>(link);
  link_shape->link_input_shape(input_shape);
  link_shape->link_output_shape(output_shape);
  link_shape->finished_moving();
  return link;
}

void NodeGraphManipulatorImp::synchronize_graph_dirtiness(Entity* group_entity) {
  const Entity::NameToChildMap& children = group_entity->get_children();
  for (auto &iter : children) {
    const std::string& child_name = iter.first;
    Entity* child = iter.second;

    EntityDID did = child->get_did();
    if (child->has_comp(ComponentIID::kIGroupInteraction)) {
      // Skip our current group, because the user is likely incremently adding nodes
      // and we don't want to recompute everything upstream over and over.
      if (child != _factory->get_current_group()) {
        synchronize_group_dirtiness(child);
      }
      synchronize_graph_dirtiness(child);
    }
  }
}

void NodeGraphManipulatorImp::synchronize_group_dirtiness(Entity* group_entity) {
  // Update the group's dirtiness from our internals.
  dirty_group_from_internals(group_entity);
  dirty_internals_from_group(group_entity);
}

void NodeGraphManipulatorImp::dirty_group_from_internals(Entity* group_entity) {
  // Grab the group compute.
  Dep<Compute> group_compute = get_dep<Compute>(group_entity);

  // Loop over the group's children.
  const Entity::NameToChildMap& children = group_entity->get_children();
  for (auto &iter : children) {
    const std::string& child_name = iter.first;
    Entity* child = iter.second;
    EntityDID did = child->get_did();
    // Anythings that's not a namespace will be a child node.
    if (did != EntityDID::kBaseNamespaceEntity) {
      // If a child node is dirty, then we set the group dirty.
      Dep<Compute> compute = get_dep<Compute>(child);
      if (compute->is_state_dirty()) {
        group_compute->dirty_state();
        return;
      }
    }
  }
}

void NodeGraphManipulatorImp::dirty_internals_from_group(Entity* group_entity) {
  // Grab the group compute.
  Dep<Compute> group_compute = get_dep<Compute>(group_entity);
  // If the group is clean then we can return right away.
  if (!group_compute->is_state_dirty()) {
    return;
  }

  // Otherwise we loop over the group's children.
  const Entity::NameToChildMap& children = group_entity->get_children();
  for (auto &iter : children) {
    const std::string& child_name = iter.first;
    Entity* child = iter.second;
    EntityDID did = child->get_did();
    // Anythings that's not a namespace will be a child node.
    if (did != EntityDID::kBaseNamespaceEntity) {
      // Make sure all the child nodes are dirty.
      Dep<Compute> compute = get_dep<Compute>(child);
      if (compute) {
        compute->dirty_state();
      }
    }
  }
}

void NodeGraphManipulatorImp::set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload) {
  Entity* entity = _app_root->has_entity(node_path);
  if (!entity) {
    return;
  }

  Dep<MQTTSubscribeCompute> compute = get_dep<MQTTSubscribeCompute>(entity);
  compute->set_override(topic, payload);
}

// -----------------------------------------------------------------------------------
// The NodeGraphManipulator.
// -----------------------------------------------------------------------------------

NodeGraphManipulator::NodeGraphManipulator(Entity* entity):
    BaseNodeGraphManipulator(entity, kDID()),
    _imp(new_ff NodeGraphManipulatorImp(get_app_root()))
{
}

NodeGraphManipulator::~NodeGraphManipulator(){
  delete_ff(_imp);
}

void NodeGraphManipulator::initialize_wires() {
  Component::initialize_wires();
  _imp->initialize_wires();
}

void NodeGraphManipulator::set_ultimate_target(Entity* entity, bool force_stack_reset) {
  _imp->set_ultimate_target(entity, force_stack_reset);
}

void NodeGraphManipulator::clear_ultimate_target() {
  _imp->clear_ultimate_target();
}

void NodeGraphManipulator::continue_cleaning_to_ultimate_target() {
  _imp->continue_cleaning_to_ultimate_target();
}

bool NodeGraphManipulator::is_busy_cleaning() {
  return _imp->is_busy_cleaning();
}

void NodeGraphManipulator::set_processing_node(Entity* entity) {
  _imp->set_processing_node(entity);
}

void NodeGraphManipulator::clear_processing_node() {
  _imp->clear_processing_node();
}

void NodeGraphManipulator::set_error_node(const QString& error_message) {
  _imp->set_error_node(error_message);
}

void NodeGraphManipulator::clear_error_node() {
  _imp->clear_error_node();
}

void NodeGraphManipulator::update_clean_marker(Entity* entity, bool clean) {
  _imp->update_clean_marker(entity, clean);
}

void NodeGraphManipulator::dive_into_lockable_group(const std::string& child_group_name) {
  _imp->dive_into_lockable_group(child_group_name);
}

void NodeGraphManipulator::clean_lockable_group(const std::string& child_group_name) {
  _imp->clean_lockable_group(child_group_name);
}

Entity* NodeGraphManipulator::build_and_link_compute_node(ComponentDID compute_did, const QJsonObject& chain_state) {
  return _imp->build_and_link_compute_node(compute_did, chain_state);
}

void NodeGraphManipulator::set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) {
  _imp->set_input_topology(entity, ordering);
}

void NodeGraphManipulator::set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) {
  _imp->set_output_topology(entity, ordering);
}

void NodeGraphManipulator::destroy_link(Entity* input_entity) {
  _imp->destroy_link(input_entity);
}

Entity* NodeGraphManipulator::create_link() {
  return _imp->create_link();
}

Entity* NodeGraphManipulator::connect_plugs(Entity* input_entity, Entity* output_entity) {
  return _imp->connect_plugs(input_entity, output_entity);
}

void NodeGraphManipulator::synchronize_graph_dirtiness(Entity* group_entity) {
  _imp->synchronize_graph_dirtiness(group_entity);
}

void NodeGraphManipulator::set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload) {
  _imp->set_mqtt_override(node_path, topic, payload);
}

}
