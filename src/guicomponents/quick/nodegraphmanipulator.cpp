#include <guicomponents/quick/nodegraphmanipulator.h>

#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/mqttcomputes.h>
#include <guicomponents/computes/entergroupcompute.h>
#include <guicomponents/computes/firebasecomputes.h>

#include <components/compshapes/nodeselection.h>
#include <components/interactions/groupinteraction.h>

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

#include <entities/guientities.h>

namespace ngs {

// -----------------------------------------------------------------------------------
// NodeGraphManipulatorImp.
// -----------------------------------------------------------------------------------

// Note that the NodeGraphManipulatorImp Component gets created outside the app_root hierarchy.
// The NodeGraphManipulatorImp Component has no parent and in a sense is its own root.
// Because it has no parent, you must explicitly hold a pointer to this when
// allocated so that you can manipulate it and delete it.
NodeGraphManipulatorImp::NodeGraphManipulatorImp(Entity* app_root)
    : QObject(),
      Component(NULL, kIID(), kDID()),
      _app_root(app_root),
      _factory(this),
      _selection(this),
      _ng_quick(this),
      _scheduler(this) {

  // Timer setup.
  _condition_timer.setSingleShot(false);
  _condition_timer.setInterval(0); // We allow 1 second to wait to connect.
  connect(&_condition_timer,SIGNAL(timeout()),this,SLOT(on_condition_timer()));
}

bool NodeGraphManipulatorImp::start_waiting(std::function<void()> on_clean_inputs) {
  if (_condition_timer.isActive()) {
    // We already waiting for something.
    return false;
  }
  _on_clean_inputs = on_clean_inputs;
  _condition_timer.start();
  return true;
}

void NodeGraphManipulatorImp::stop_waiting() {
  _condition_timer.stop();
}

void NodeGraphManipulatorImp::on_condition_timer() {
  if (!is_busy_cleaning()) {
    _condition_timer.stop();
    std::cerr << "performing work now that inputs are clean\n";
    _on_clean_inputs();
  }
}

void NodeGraphManipulatorImp::initialize_wires() {
  Component::initialize_wires();

  _factory = get_dep<BaseFactory>(_app_root);
  _selection = get_dep<NodeSelection>(_app_root);
  _ng_quick = get_dep<NodeGraphQuickItem>(_app_root);
  _scheduler = get_dep<TaskScheduler>(_app_root);
}

void NodeGraphManipulatorImp::set_ultimate_targets(Entity* entity, bool force_stack_reset) {
  std::unordered_set<Entity*> entities;
  entities.insert(entity);
  set_ultimate_targets(entities, force_stack_reset);
}

void NodeGraphManipulatorImp::set_ultimate_targets(const std::unordered_set<Entity*>& entities, bool force_stack_reset) {
  DepUSet<Compute> computes;
  for (Entity* entity: entities) {
    Dep<Compute> c = get_dep<Compute>(entity);
    if (c->is_state_dirty()) {
      computes.insert(c);
    }
  }
  if (computes.empty()) {
    return;
  }

  // Synchronize the dirtiness of all groups with the internal contents.
  // This is needed because a group maintains it's own dirtiness separately from the internal nodes,
  // and the user can manually change the dirtiness of the internal nodes or the group.
  // This consolidates those dirtiness states, so the we perform the compute properly.
  // Note that nodes in the currently are not dirtied because the user is likely adding/editing nodes
  // and we don't want to have to keep recomputing all the upstream nodes in this case.
  // All other groups get their dirtiness synchronized.
  bubble_group_dirtiness();

  // Clear the error marker on nodes.
  _selection->clear_error_node();
  // This may be called while we are already trying to clean another ultimate target.
  // Hence we force a stack reset to clear out any pre-existing tasks.
  if (force_stack_reset) {
    _scheduler->force_stack_reset();
  }
  // Set and try cleaning the ultimate target.
  _ultimate_targets = computes;

  _app_root->clean_wires();

  continue_cleaning_to_ultimate_targets();
}

void NodeGraphManipulatorImp::set_inputs_as_ultimate_targets(Entity* node_entity) {
  std::unordered_set<Entity*> entities;

  // If the entity has a compute, we add its inputs into the ultimate targets.
  Dep<Compute> compute = get_dep<Compute>(node_entity);
  const Dep<Inputs>& inputs = compute->get_inputs();
  const std::unordered_map<std::string, Dep<InputCompute> >& input_computes = inputs->get_all();
  for (auto &iter: input_computes) {
    entities.insert(iter.second->our_entity());
  }

  set_ultimate_targets(entities);
}

void NodeGraphManipulatorImp::clear_ultimate_targets() {
  _ultimate_targets.clear();
}

void NodeGraphManipulatorImp::prune_clean_or_dead() {
  // Remove any ultimate targets that are already clean.
  DepUSet<Compute>::iterator iter = _ultimate_targets.begin();
  while (iter != _ultimate_targets.end()) {
    if (!(*iter)) {
      // If the compute has been destoryed, then we erase it.
      iter = _ultimate_targets.erase(iter);
      continue;
    } else {
      // If the compute is now clean, then we erase it.
      if (!(*iter)->is_state_dirty()) {
        iter = _ultimate_targets.erase(iter);
        continue;
      }
    }
    // Otherwise we continue to the next compute.
    ++iter;
  }
}

void NodeGraphManipulatorImp::continue_cleaning_to_ultimate_targets() {
  // Make sure the ulimate targets have been pruned of dead or cleaned computes.
  prune_clean_or_dead();

  // Return if there is no ultimate target set.
  if (_ultimate_targets.empty()) {
    return;
  }

  // Lets continue each target of our ultimate targets.
  while(!_ultimate_targets.empty()) {
    const Dep<Compute> &compute = *_ultimate_targets.begin();

    // Clean all the inputs on all the groups down to the ultimate target.
    // We need to do this because there is no direct dependency link between the group's inputs
    // and the input nodes inside the group.

    // The target path holds the path down to the immediately surrounding group of the target.
    Path target_path = compute->get_path();
    target_path.pop_back();

    // Starting from the root group we dive one by one to the target group.
    Path path({});
    do {
      path.push_back(target_path.front());
      target_path.pop_front();
      std::cerr << "working on group inputs: " << path.get_as_string() << "\n";

      Entity* e = _app_root->get_entity(path);
      Dep<GroupNodeCompute> c = get_dep<GroupNodeCompute>(e);
      if (!c) {
        // If the ultimate target is an input on a node, then we may iterate thruough a node.
        // In this case the GroupNodeCompute dep will be null.
        continue;
      }
      if (!c->clean_inputs()) {
        return;
      }
    } while (target_path.size());

    // If we get here the inputs on our surrounding group and our input nodes
    // now have appropriate values to perform the compute.
    if (compute->clean_state()) {
      // We need to clean any clean computes right away, otherwise the app will think it's still processing..
      prune_clean_or_dead();
    } else {
      return;
    }
  }
}

bool NodeGraphManipulatorImp::is_busy_cleaning() {
  if (_ultimate_targets.size()) {
    return true;
  }
  return false;
}

void NodeGraphManipulatorImp::set_processing_node(Entity* entity) {
  _selection->set_processing_node_entity(entity);
  _ng_quick->update();
}

void NodeGraphManipulatorImp::clear_processing_node() {
  _selection->clear_processing_node();
  _ng_quick->update();
}

void NodeGraphManipulatorImp::set_error_node(const QString& error_message) {
  Dep<NodeShape> ns = _selection->get_last_processing_node();
  if (!ns) {
    std::cerr << "Warning: could not find the error node\n";
  }
  if (ns) {
    // Show the error marker on the node.
    _selection->set_error_node(ns);
    // Update the gui.
    _ng_quick->update();
  }

  // Emit messages to the qml side.
  emit _ng_quick->set_error_message(error_message);
  emit _ng_quick->show_error_page();
}

void NodeGraphManipulatorImp::clear_error_node() {
  _selection->clear_error_node();
  _ng_quick->update();
}

void NodeGraphManipulatorImp::update_clean_marker(Entity* entity, bool clean) {
  std::cerr << "updating clean marker for: " << entity->get_path().get_as_string() << " : " << clean << "\n";
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

void NodeGraphManipulatorImp::clean_enter_group(Entity* group) {
  // Graph the entity for the group context node.
  Entity* node = group->get_child("group_context");
  if (!node) {
    return;
  }
  // Dirty the group context node.
  Dep<EnterGroupCompute> enter = get_dep<EnterGroupCompute>(node);
  enter->dirty_state();
  // Now we it as the ultimate target to clean to.
  set_ultimate_targets(node);
}

void NodeGraphManipulatorImp::clean_exit_group(Entity* group) {
  // If this group has an exit group node, then make it dirty and clean it.
  Entity* child = group->get_child("exit_group_context");
  if (!child) {
    return;
  }

  // We expect the exit compute to not have to wait for an asynchronous response.
  Dep<ExitGroupCompute> exit = get_dep<ExitGroupCompute>(child);
  exit->dirty_state();
  exit->clean_state();
}

void NodeGraphManipulatorImp::dive_into_group(const std::string& child_group_name) {
  // Graph the entity for the group context node.
  Entity* group =_factory->get_current_group()->get_child(child_group_name);
  if (!group) {
    return;
  }

  _ng_quick->dive_into_group(child_group_name);
  clean_enter_group(group);
}

void NodeGraphManipulatorImp::surface_from_group() {
  Entity* group_entity = _factory->get_current_group();
  clean_exit_group(group_entity);
  _ng_quick->surface();
}

Entity* NodeGraphManipulatorImp::build_and_link_compute_node(ComponentDID compute_did, const QJsonObject& chain_state) {
  Entity* node = build_compute_node(compute_did, chain_state);
  link(node);
  return node;
}

void NodeGraphManipulatorImp::set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) {
  Dep<InputTopology> topo = get_dep<InputTopology>(entity);
  // If the entity doesn't have a gui, then the topo with be null.
  if (topo) {
    topo->set_topology(ordering);
  }
}

void NodeGraphManipulatorImp::set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) {
  Dep<OutputTopology> topo = get_dep<OutputTopology>(entity);
  // If the entity doesn't have a gui, then the topo with be null.
  if (topo) {
    topo->set_topology(ordering);
  }
}

void NodeGraphManipulatorImp::finish_creating_node(Entity* entity, bool centered) {
  entity->create_internals();
  entity->initialize_wires();
  Dep<NodeShape> cs = get_dep<NodeShape>(entity);
  if (centered) {
    _ng_quick->get_current_interaction()->centralize(cs);
  } else {
    cs->set_pos(_ng_quick->get_last_press_info().object_space_pos.xy());
  }
  // The parenting group node needs to update its inputs and outputs.
  //get_app_root()->update_wires();
  _selection->clear_selection();
  _selection->select(cs);

  // A little overkill, but we clean the wires on everything in this group.
  // Otherwise after creating nodes like mock or input or output nodes,
  // they appear with their input and outplugs not showing.
  _factory->get_current_group()->clean_wires();
  _ng_quick->update();
}

void NodeGraphManipulatorImp::create_node(bool centered, EntityDID entity_did) {
  Entity* e = _factory->instance_entity(_factory->get_current_group(), entity_did);
  finish_creating_node(e, centered);
}

void NodeGraphManipulatorImp::create_compute_node(bool centered, ComponentDID compute_did) {
  Entity* e = _factory->instance_compute_node(_factory->get_current_group(), compute_did);
  finish_creating_node(e, centered);
}

void NodeGraphManipulatorImp::create_user_macro_node(bool centered, const std::string& macro_name) {
  Entity* e = _factory->instance_entity(_factory->get_current_group(), EntityDID::kUserMacroNodeEntity);
  static_cast<UserMacroNodeEntity*>(e)->load_internals(macro_name);
  finish_creating_node(e, centered);
}

void NodeGraphManipulatorImp::create_app_macro_node(bool centered, const std::string& macro_name) {
  Entity* e = _factory->instance_entity(_factory->get_current_group(), EntityDID::kAppMacroNodeEntity);
  static_cast<AppMacroNodeEntity*>(e)->load_internals(macro_name);
  finish_creating_node(e, centered);
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
    Entity* link = factory->instance_entity(links_folder, EntityDID::kLinkEntity, "link");
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

Entity* NodeGraphManipulatorImp::create_link(Entity* group) {
  Entity* links_folder = group->get_entity(Path({".","links"}));
  Entity* link = _factory->instance_entity(links_folder, EntityDID::kLinkEntity, "link");
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

  assert(input_shape);
  assert(output_shape);
  assert(input_compute);
  assert(output_compute);

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
  Entity* group = input_entity->get_entity(Path({"..","..",".."}));
  Entity* link = create_link(group);
  Dep<LinkShape> link_shape = get_dep<LinkShape>(link);
  link_shape->link_input_shape(input_shape);
  link_shape->link_output_shape(output_shape);
  link_shape->finished_moving();

  // We need to bake our dynamic dep paths because initialize_wires may get called later which will try to setup up the dep from an unset path.
  link_shape->bake_paths();
  return link;
}

void NodeGraphManipulatorImp::bubble_group_dirtiness() {
  synchronize_graph_dirtiness(_app_root);
}

void NodeGraphManipulatorImp::synchronize_graph_dirtiness(Entity* group_entity) {
  const Entity::NameToChildMap& children = group_entity->get_children();
  for (auto &iter : children) {
    const std::string& child_name = iter.first;
    Entity* child = iter.second;

    EntityDID did = child->get_did();
    if (child->has_comp_with_iid(ComponentIID::kIGroupInteraction)) {
      // Skip our current group, because the user is likely incremently adding nodes
      // and we don't want to recompute everything upstream over and over.
      if (child != _factory->get_current_group()) {
        synchronize_group_dirtiness(child);
      }
      synchronize_graph_dirtiness(child);
    }
  }
}

void NodeGraphManipulatorImp::dirty_compute(const Path& path) {
  Entity* entity = _app_root->get_entity(path);
  Dep<Compute> compute = get_dep<Compute>(entity);
  if (compute) {
    compute->dirty_state();
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
  // Find the entity.
  Entity* entity = _app_root->has_entity(node_path);
  if (!entity) {
    return;
  }

  // Make sure we have the right compute.
  if (!entity->has_comp_with_did(ComponentIID::kICompute, ComponentDID::kMQTTSubscribeCompute)) {
    return;
  }

  // Set the override.
  Dep<MQTTSubscribeCompute> compute = get_dep<MQTTSubscribeCompute>(entity);
  compute->set_override(topic, payload);

  // Bubble dirtiness through the group hierarchies.
  bubble_group_dirtiness();
}

void NodeGraphManipulatorImp::set_firebase_override(const Path& node_path, const QString& data_path, const QJsonValue& value) {
  // Find the entity.
  Entity* entity = _app_root->has_entity(node_path);
  if (!entity) {
    return;
  }

  // Make sure we have the right compute.
  if (!entity->has_comp_with_did(ComponentIID::kICompute, ComponentDID::kFirebaseReadDataCompute)) {
    return;
  }

  // Set the override.
  Dep<FirebaseReadDataCompute> compute = get_dep<FirebaseReadDataCompute>(entity);
  compute->set_override(data_path, value);

  // Bubble dirtiness through the group hierarchies.
  bubble_group_dirtiness();
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

void NodeGraphManipulator::set_ultimate_targets(Entity* entity, bool force_stack_reset) {
  _imp->set_ultimate_targets(entity, force_stack_reset);
}

void NodeGraphManipulator::set_ultimate_targets(const std::unordered_set<Entity*>& entities, bool force_stack_reset) {
  _imp->set_ultimate_targets(entities, force_stack_reset);
}

void NodeGraphManipulator::set_inputs_as_ultimate_targets(Entity* node_entity) {
  _imp->set_inputs_as_ultimate_targets(node_entity);
}

void NodeGraphManipulator::clear_ultimate_targets() {
  _imp->clear_ultimate_targets();
}

void NodeGraphManipulator::continue_cleaning_to_ultimate_targets() {
  _imp->continue_cleaning_to_ultimate_targets();
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

void NodeGraphManipulator::clean_enter_group(Entity* group) {
  _imp->clean_enter_group(group);
}

void NodeGraphManipulator::clean_exit_group(Entity* group) {
  _imp->clean_exit_group(group);
}

void NodeGraphManipulator::dive_into_group(const std::string& child_group_name) {
  _imp->dive_into_group(child_group_name);
}

void NodeGraphManipulator::surface_from_group() {
  _imp->surface_from_group();
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

void NodeGraphManipulator::create_node(bool centered, EntityDID entity_did) {
  _imp->create_node(centered, entity_did);
}

void NodeGraphManipulator::create_compute_node(bool centered, ComponentDID compute_did) {
  _imp->create_compute_node(centered, compute_did);
}

void NodeGraphManipulator::create_user_macro_node(bool centered, const std::string& macro_name) {
  _imp->create_user_macro_node(centered, macro_name);
}

void NodeGraphManipulator::create_app_macro_node(bool centered, const std::string& macro_name) {
  _imp->create_app_macro_node(centered, macro_name);
}

void NodeGraphManipulator::destroy_link(Entity* input_entity) {
  _imp->destroy_link(input_entity);
}

Entity* NodeGraphManipulator::create_link(Entity* group) {
  return _imp->create_link(group);
}

Entity* NodeGraphManipulator::connect_plugs(Entity* input_entity, Entity* output_entity) {
  return _imp->connect_plugs(input_entity, output_entity);
}

void NodeGraphManipulator::bubble_group_dirtiness() {
  _imp->bubble_group_dirtiness();
}

void NodeGraphManipulator::synchronize_graph_dirtiness(Entity* group_entity) {
  _imp->synchronize_graph_dirtiness(group_entity);
}

void NodeGraphManipulator::dirty_compute(const Path& path) {
  _imp->dirty_compute(path);
}

void NodeGraphManipulator::set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload) {
  _imp->set_mqtt_override(node_path, topic, payload);
}

void NodeGraphManipulator::set_firebase_override(const Path& node_path, const QString& data_path, const QJsonValue& value) {
  _imp->set_firebase_override(node_path, data_path, value);
}

}
