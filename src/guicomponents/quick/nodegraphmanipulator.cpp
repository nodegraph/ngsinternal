#include <guicomponents/quick/nodegraphmanipulator.h>

#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/computes/taskqueuer.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/mqttcomputes.h>
#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/messagereceiver.h>
#include <guicomponents/quick/nodegraphview.h>

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
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputs.h>

// CompShapes.
#include <components/compshapes/topology.h>
#include <components/compshapes/nodeshape.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/compshapecollective.h>

#include <entities/guientities.h>

#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>


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
      _queuer(this),
      _scheduler(this),
      _msg_receiver(this),
      _file_model(this),
      _ng_view(this) {

  // Timer setup.
  _idle_timer.setSingleShot(true);
  _idle_timer.setInterval(0);
  connect(&_idle_timer,SIGNAL(timeout()),this,SLOT(on_idle_timer_timeout()));

  // Auto Run Timer setup.
  _auto_run_timer.setSingleShot(false);
  connect(&_auto_run_timer,SIGNAL(timeout()),this,SLOT(on_auto_run_timer_timeout()));

}

bool NodeGraphManipulatorImp::start_waiting(const std::function<void()>& on_idle) {
  if (_idle_timer.isActive()) {
    // We already waiting for something.
    return false;
  }
  _on_idle = on_idle;
  _idle_timer.start();
  return true;
}

void NodeGraphManipulatorImp::on_idle_timer_timeout() {
  //assert(!is_busy_cleaning());
  //_idle_timer.stop();
  _on_idle();
}

void NodeGraphManipulatorImp::on_auto_run_timer_timeout() {
  if (is_busy_cleaning()) {
    return;
  }
  _ng_quick->surface_to_root();
  _ng_quick->reclean_group();
}

void NodeGraphManipulatorImp::initialize_wires() {
  Component::initialize_wires();

  _factory = get_dep<BaseFactory>(_app_root);
  _selection = get_dep<NodeSelection>(_app_root);
  _ng_quick = get_dep<NodeGraphQuickItem>(_app_root);
  _queuer = get_dep<TaskQueuer>(_app_root);
  _scheduler = get_dep<TaskScheduler>(_app_root);
  _msg_receiver = get_dep<MessageReceiver>(_app_root);
  _file_model = get_dep<FileModel>(_app_root);
  _ng_view = get_dep<NodeGraphView>(_app_root);

  // Just to be safe when initialize_wires get called multiple times,
  // we disconnect possible pre-existing connections.
  disconnect(_file_model.get(), SIGNAL(auto_run_changed(bool)), this, SLOT(on_auto_run_changed(bool)));
  disconnect(_file_model.get(), SIGNAL(auto_run_interval_changed(int)), this, SLOT(on_auto_run_interval_changed(int)));

  // Now reconnect.
  connect(_file_model.get(), SIGNAL(auto_run_changed(bool)), this, SLOT(on_auto_run_changed(bool)));
  connect(_file_model.get(), SIGNAL(auto_run_interval_changed(int)), this, SLOT(on_auto_run_interval_changed(int)));

  // Initialize auto run values.
  on_auto_run_changed(_file_model->get_auto_run());
  on_auto_run_interval_changed(_file_model->get_auto_run_interval());
}

void NodeGraphManipulatorImp::receive_message(const QString& msg) {
  _msg_receiver->on_text_received(msg);
}

void NodeGraphManipulatorImp::set_ultimate_targets(Entity* entity, bool force_stack_reset) {
  std::unordered_set<Entity*> entities;
  entities.insert(entity);
  set_ultimate_targets(entities, force_stack_reset);
}

void NodeGraphManipulatorImp::set_ultimate_targets(const std::unordered_set<Entity*>& entities, bool force_stack_reset) {
  std::cerr << "appending to ultimate targets: " << entities.size() << "\n";
  _ng_view->report_ng_usage();

  DepUSet<Compute> computes;
  for (Entity* entity: entities) {
    Dep<Compute> c = get_dep<Compute>(entity);
    if (c->is_state_dirty()) {
      computes.insert(c);
      c->reset_dirty_state(); // This resets things like loop counters in dirty loop-like computes.
    }
  }
  if (computes.empty()) {
    return;
  }

  // Clear the error marker on nodes.
  _selection->clear_error_node();

//  // This may be called while we are already trying to clean another ultimate target.
//  // Hence we force a stack reset to clear out any pre-existing tasks.
//  if (force_stack_reset) {
//    _scheduler->force_stack_reset();
//  }


  // Remove any computes which are already in the ultimate targets.
  for (auto &c: _ultimate_targets) {
    if (computes.count(c) > 0) {
      computes.erase(c);
    }
  }

  // Set and try cleaning the ultimate target.
  for (auto &c: computes) {
    _ultimate_targets.push_back(c);
  }

  _app_root->clean_wires();

  continue_cleaning_to_ultimate_targets_on_idle();
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
  std::vector<Dep<Compute> >::iterator iter = _ultimate_targets.begin();
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

void NodeGraphManipulatorImp::continue_cleaning_to_ultimate_targets_on_idle() {
  std::function<void()> f = std::bind(&NodeGraphManipulatorImp::continue_cleaning_to_ultimate_targets, this);
  start_waiting(f);
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
    Path path;
    while(target_path.size()) {
      path.push_back(target_path.front());
      target_path.pop_front();

      Entity* e = _app_root->get_entity(path);
      Dep<GroupNodeCompute> c = get_dep<GroupNodeCompute>(e);
      if (!c) {
        // If the ultimate target is an input on a node, then we may iterate thruough a node.
        // In this case the GroupNodeCompute dep will be null.
        continue;
      }
      std::cerr << "cleaning inputs to path: " << path.get_as_string() << "\n";
      if (!c->clean_inputs()) {
        return;
      }
    }

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
  if (_ultimate_targets.size() || _scheduler->is_busy()) {
    return true;
  }
  return false;
}

bool NodeGraphManipulatorImp::current_task_is_cancelable() {
  return _scheduler->current_task_is_cancelable();
}

void NodeGraphManipulatorImp::set_processing_node(Entity* entity) {
  _selection->set_processing_node_entity(entity);
  _ng_quick->update();
}

bool NodeGraphManipulatorImp::is_waiting_for_response() {
  return _scheduler->is_waiting_for_response();
}

void NodeGraphManipulatorImp::force_stack_reset() {
  clear_ultimate_targets();
  _scheduler->force_stack_reset();
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

void NodeGraphManipulatorImp::enter_group_prep(Entity* group) {
  if (group->get_did() == EntityDID::kBrowserGroupNodeEntity) {
    TaskContext tc(_scheduler);
    _queuer->queue_open_browser(tc);
  }
}

void NodeGraphManipulatorImp::exit_group_prep(Entity* group) {
  if (group->get_did() == EntityDID::kBrowserGroupNodeEntity) {
    // Determine whether we are nested recusively inside muliple browser groups.
    // We do this by counting the number surrounding browser groups.
    Entity* parent = group;
    size_t count = 0;
    while(parent) {
      if (parent->get_did() == EntityDID::kBrowserGroupNodeEntity) {
        count++;
      }
      parent = parent->get_parent();
    }
    // If we're not nested, then we can close the browser.
    if (count <= 1) {
      // Determine whether to release the browser to the user instead of destorying it.
      Entity* release_entity = group->has_entity(Path({".","release_on_exit"}));
      bool release = false;
      if (release_entity) {
        Dep<InputNodeCompute> in = get_dep<InputNodeCompute>(release_entity);
        release = in->get_output("out").toBool();
        std::cerr << "found release input node with value: " << release << "\n";
      } else {
        std::cerr << "did not find release input node\n";
      }

      TaskContext tc(_scheduler);
      if (!release) {
        _queuer->queue_send_msg(tc, Message(WebDriverRequestType::kCloseBrowser), false);
      } else {
        _queuer->queue_send_msg(tc, Message(WebDriverRequestType::kReleaseBrowser), false);
      }
    }
  }
}

void NodeGraphManipulatorImp::dive_into_group(const std::string& child_group_name) {
  // Graph the entity for the group context node.
  Entity* group =_factory->get_current_group()->get_child(child_group_name);
  if (!group) {
    return;
  }

  _ng_quick->dive_into_group(child_group_name);
  enter_group_prep(group);
}

void NodeGraphManipulatorImp::surface_from_group() {
  Entity* group_entity = _factory->get_current_group();
  exit_group_prep(group_entity);
  _ng_quick->surface();
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
  entity->initialize_wires();
  Dep<NodeShape> cs = get_dep<NodeShape>(entity);

  // Some nodes won't be visible.
  if (cs) {
    if (centered) {
      _ng_quick->get_current_interaction()->centralize(cs);
    } else {
      cs->set_pos(_ng_quick->get_last_press_info().object_space_pos.xy());
    }
    // The parenting group node needs to clean its wires and update the Inputs to
    // to reflect the possible creation of input and output nodes.
//    _factory->get_current_group()->clean_wires();
//    Dep<Inputs> group_inputs = get_dep<Inputs>(entity->get_parent());
//    group_inputs->clean_state();

    // We need let encompassing groups know that we are dirty.
    bubble_group_dirtiness(entity);
  }

  // A little overkill, but we clean the wires on everything in this group.
  // Otherwise after creating nodes like mock or input or output nodes,
  // they appear with their input and outplugs not showing.
  _factory->get_current_group()->clean_wires();
  // We need 2 passes of clean wires for our special group nodes like the firebase group node.
  _factory->get_current_group()->clean_wires();
  // Update the gui.
  _ng_quick->update();
}

Entity* NodeGraphManipulatorImp::create_node(bool centered, EntityDID entity_did, const std::string& name, Entity* group_entity) {
  if (!group_entity) {
    group_entity = _factory->get_current_group();
  }

  Entity* e = _factory->instance_entity(group_entity, entity_did, name);
  e->create_internals();
  finish_creating_node(e, centered);
  return e;
}

Entity* NodeGraphManipulatorImp::create_compute_node(bool centered, EntityDID entity_did, ComponentDID compute_did, const std::string& name, Entity* group_entity) {
  if (!group_entity) {
    group_entity = _factory->get_current_group();
  }

  std::string node_name = name;
  if (node_name.empty()) {
    node_name = get_component_user_did_name(compute_did);
  }
  Entity* e = _factory->instance_entity(group_entity, entity_did, node_name);
  EntityConfig config;
  config.compute_did = compute_did;
  e->create_internals(config);
  finish_creating_node(e, centered);
  return e;
}

Entity* NodeGraphManipulatorImp::create_public_macro_node(bool centered, const std::string& macro_name, const std::string& name, Entity* group_entity) {
  if (!group_entity) {
    group_entity = _factory->get_current_group();
  }

  Entity* e = _factory->instance_entity(group_entity, EntityDID::kPublicMacroNodeEntity, name);
  PublicMacroNodeEntity* u = static_cast<PublicMacroNodeEntity*>(e);
  u->load_internals(macro_name);
  e->create_internals();
  finish_creating_node(e, centered);

  Dep<GroupNodeCompute> g = get_dep<GroupNodeCompute>(u);
  g->revert_params_to_defaults();
  return e;
}

Entity* NodeGraphManipulatorImp::create_private_macro_node(bool centered, const std::string& macro_name, const std::string& name, Entity* group_entity) {
  if (!group_entity) {
    group_entity = _factory->get_current_group();
  }

  Entity* e = _factory->instance_entity(group_entity, EntityDID::kPrivateMacroNodeEntity, name);
  PrivateMacroNodeEntity* u = static_cast<PrivateMacroNodeEntity*>(e);
  u->load_internals(macro_name);
  e->create_internals();
  finish_creating_node(e, centered);

  Dep<GroupNodeCompute> g = get_dep<GroupNodeCompute>(u);
  g->revert_params_to_defaults();
  return e;
}

Entity* NodeGraphManipulatorImp::create_app_macro_node(bool centered, const std::string& macro_name, const std::string& name, Entity* group_entity) {
  if (!group_entity) {
    group_entity = _factory->get_current_group();
  }

  Entity* e = _factory->instance_entity(group_entity, EntityDID::kAppMacroNodeEntity, name);
  AppMacroNodeEntity* a = static_cast<AppMacroNodeEntity*>(e);
  a->load_internals(macro_name);
  e->create_internals();
  finish_creating_node(e, centered);

  Dep<GroupNodeCompute> g = get_dep<GroupNodeCompute>(a);
  g->revert_params_to_defaults();
  return e;
}

Entity* NodeGraphManipulatorImp::create_password_input_node(bool centered, const QJsonValue& unconnected_value, const std::string& name, Entity* group_entity) {
  if (!group_entity) {
    group_entity = _factory->get_current_group();
  }

  Entity* e = _factory->instance_entity(group_entity, EntityDID::kPasswordInputNodeEntity, name);
  EntityConfig config;
  config.unconnected_value = unconnected_value;

  e->create_internals(config);
  finish_creating_node(e, centered);
  return e;
}

Entity* NodeGraphManipulatorImp::create_input_node(bool centered, const QJsonValue& unconnected_value, const std::string& name, Entity* group_entity) {
  if (!group_entity) {
    group_entity = _factory->get_current_group();
  }

  Entity* e = _factory->instance_entity(group_entity, EntityDID::kInputNodeEntity, name);
  EntityConfig config;
  config.unconnected_value = unconnected_value;

  e->create_internals(config);
  finish_creating_node(e, centered);
  return e;
}

Entity* NodeGraphManipulatorImp::create_password_data_node(bool centered, const QJsonValue& unconnected_value, const std::string& name, Entity* group_entity) {
  if (!group_entity) {
    group_entity = _factory->get_current_group();
  }

  Entity* e = _factory->instance_entity(group_entity, EntityDID::kPasswordDataNodeEntity, name);
  EntityConfig config;
  config.unconnected_value = unconnected_value;

  e->create_internals(config);
  finish_creating_node(e, centered);
  return e;
}

Entity* NodeGraphManipulatorImp::create_data_node(bool centered, const QJsonValue& unconnected_value, const std::string& name, Entity* group_entity) {
  if (!group_entity) {
    group_entity = _factory->get_current_group();
  }

  Entity* e = _factory->instance_entity(group_entity, EntityDID::kDataNodeEntity, name);
  EntityConfig config;
  config.unconnected_value = unconnected_value;

  e->create_internals(config);
  finish_creating_node(e, centered);
  return e;
}

Entity* NodeGraphManipulatorImp::create_browser_node(bool centered, ComponentDID compute_did, const QJsonObject& chain_state, const std::string& name, Entity* group_entity) {
  if (!group_entity) {
    group_entity = _factory->get_current_group();
  }

  // Create the node.
  Entity* _node = create_compute_node(centered, EntityDID::kComputeNodeEntity, compute_did, name, group_entity);

  // Initialize and update the wires.
  _node->initialize_wires();
  group_entity->clean_wires();

  // Set the values on all the inputs from the chain_state.
  for (QJsonObject::const_iterator iter = chain_state.constBegin(); iter != chain_state.constEnd(); ++iter) {
    // Find the input entity.
    Path path( { ".", kInputsFolderName });
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
    // Set the unconnected value of tne input.
    compute->set_unconnected_value(iter.value());
  }
  return _node;
}

Entity* NodeGraphManipulatorImp::create_link(Entity* input_entity, Entity* output_entity) {
  Dep<OutputCompute> output_compute = get_dep<OutputCompute>(output_entity);
  Dep<InputCompute> input_compute = get_dep<InputCompute>(input_entity);

  // Link the computes.
  input_compute->link_output_compute(output_compute);

  // Create a link.
  Entity* current_group = _factory->get_current_group();
  Entity* links_folder = current_group->get_entity(Path( { ".", kLinksFolderName }));
  Entity* link = _factory->instance_entity(links_folder, EntityDID::kLinkEntity, "link");
  link->create_internals();

  // Link the link, input and output shapes.
  Dep<OutputShape> output_shape = get_dep<OutputShape>(output_compute->our_entity());
  Dep<InputShape> input_shape = get_dep<InputShape>(input_compute->our_entity());
  Dep<LinkShape> link_shape = get_dep<LinkShape>(link);
  link_shape->start_moving();
  link_shape->link_input_shape(input_shape);
  link_shape->link_output_shape(output_shape);
  link_shape->finished_moving();

  // Clean the wires in this group, as new nodes and links were created.
  _factory->get_current_group()->clean_wires();

  return link_shape->our_entity();
}

void NodeGraphManipulatorImp::link_to_selected_node(Entity* downstream) {
  // Get the factory.
  Dep<BaseFactory> factory = get_dep<BaseFactory>(_app_root);
  Entity* current_group = factory->get_current_group();

  // Hopefully the user has only one node selected.
  // If not we're grabbing one node at random.
  const DepUSet<NodeShape>& selected = _selection->get_selected();
  std::cerr << "num selected is: " << selected.size() << "\n";

  Dep<NodeShape> upstream_node(this);
  for (auto &n: selected) {

    // Skip the downstream node which is what we're trying to connect.
    if (n->our_entity() == downstream) {
      continue;
    }

    // If the comp shape is not a node's linkable shape then continue.
    if (!n->is_linkable()) {
      continue;
    }

    // If the entity has no exposed outputs then continue.
    Dep<OutputTopology> outputs = get_dep<OutputTopology>(n->our_entity());
    if (outputs->get_num_exposed() == 0) {
      continue;
    }

    // Otherwise we've found a candidate.
    upstream_node = n;
  }

  // Get our downstream shape.
  Dep<NodeShape> downstream_node = get_dep<NodeShape>(downstream);

  /* Make sure the created node is selected.*/\
  if (upstream_node) {
    _selection->clear_selection();
    _selection->select(downstream_node);
  }

  if (!upstream_node) {
    return;
  }

  Entity* upstream = upstream_node->our_entity();

  // -------------------------------------------------------------------------
  // At this point we have both the upstream and downstream nodes to connect.
  // -------------------------------------------------------------------------

  // Position our node below the upstream.
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
    Entity* links_folder = current_group->get_entity(Path( { ".", kLinksFolderName }));
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
  Entity* links_folder = group->get_entity(Path({".",kLinksFolderName}));
  Entity* link = _factory->instance_entity(links_folder, EntityDID::kLinkEntity, "link");
  link->create_internals();

  Dep<LinkShape> dep = get_dep<LinkShape>(link);
  dep->start_moving(); // This prevents the link from getting destroyed.
  return link;
}

Entity* NodeGraphManipulatorImp::connect_plugs(Entity* input_entity, Entity* output_entity) {
  Dep<InputCompute> input_compute = get_dep<InputCompute>(input_entity);
  //Dep<InputShape> input_shape = get_dep<InputShape>(input_entity);
  Dep<OutputCompute> output_compute = get_dep<OutputCompute>(output_entity);
  //Dep<OutputShape> output_shape = get_dep<OutputShape>(output_entity);

  //assert(input_shape);
  //assert(output_shape);
  assert(input_compute);
  assert(output_compute);

//  // Remove any existing link shapes on this input.
//  // There is only one link per input.
//  Entity* old_link_entity = input_shape->find_link_entity();
//  if (old_link_entity) {
//    // Remove any existing compute connection on this input compute.
//    input_compute->unlink_output_compute();
//    // Destroy the link.
//    delete_ff(old_link_entity);
//  }

  // Try to connect the input and output computes.
  if (!input_compute->link_output_compute(output_compute)) {
    // Otherwise destroy the link and selection.
    assert(false);
  }

//  // Link the output shape.
//  Entity* group = input_entity->get_entity(Path({"..","..",".."}));
//  Entity* link = create_link(group);
//  Dep<LinkShape> link_shape = get_dep<LinkShape>(link);
//  link_shape->set_visible(false);
//  link_shape->link_input_shape(input_shape);
//  link_shape->link_output_shape(output_shape);
//  link_shape->finished_moving();

  // We need to bake our dynamic dep paths because initialize_wires may get called later which will try to setup up the dep from an unset path.
  input_compute->bake_paths();
//  link_shape->bake_paths();
  return NULL;
}

void NodeGraphManipulatorImp::copy_description_to_input_node(Entity* input_entity, Entity* input_node_entity) {
  std::string input_name = input_entity->get_name();
  Dep<InputCompute> input_compute = get_dep<InputCompute>(input_entity);
  Dep<Compute> node_compute = get_dep<Compute>(input_entity->get_entity(Path({"..",".."})));

  // Get the description from the description hint on the node for our input name.
  const QJsonObject& node_hints = node_compute->get_hints();
  QJsonObject input_hints = node_hints.value(input_name.c_str()).toObject();
  QJsonValue description = input_hints.value(QString::number(to_underlying(GUITypes::HintKey::DescriptionHint))).toString();

  // Get the unconnected value from the input. This determine the type as well.
  const QJsonValue& unconnected_value = input_compute->get_unconnected_value();

  // Set the description on the input node.
  Dep<InputCompute> desc_param = get_dep<InputCompute>(input_node_entity->get_child(kInputsFolderName)->get_child("description"));
  desc_param->set_unconnected_value(description);

  // Set the unconnected value on the input node.
  Dep<InputCompute> default_param = get_dep<InputCompute>(input_node_entity->get_child(kInputsFolderName)->get_child("default_value"));
  default_param->set_unconnected_value(unconnected_value);
}

// Bubbles dirtiness to parent groups. Needed because the dirtiness on the group is separate from the dirtiness of the internal nodes.
// When a node is dirties inside a group, this method is used to bubble the dirtiness outside the group.
// When a node is dirtied outside a group, we rely on the dirty propagation to dirty up to the group's inputs (input plugs).
// Now that the group node is dirty, it will be forced to update_state(). The update_state() will copy the input values
// onto the input nodes as overrides. This will make those input nodes dirty, if the values are differrrent.
// This in turn should cause the group's internal nodes to re-compute.
void NodeGraphManipulatorImp::bubble_group_dirtiness(Entity* dirty_node_entity) {
  Entity* parent = dirty_node_entity->get_parent();
  while(parent) {
    Dep<Compute> c = get_dep<Compute>(parent);
    if (c) {
      c->dirty_state();
    }
    parent = parent->get_parent();
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
}

void NodeGraphManipulatorImp::send_post_value_signal(int post_type, const QString& title, const QJsonObject& obj) {
  emit post_value(post_type, title, obj);
}

void NodeGraphManipulatorImp::update_auto_run_timer() {
  int interval = _file_model->get_auto_run_interval() * 1000;
  if (_auto_run_timer.interval() != interval) {
    std::cerr << "auto run interval being set to: " << interval << "\n";
    _auto_run_timer.setInterval(interval);
  }

  bool auto_run = _file_model->get_auto_run();
  if (auto_run) {
    if (!_auto_run_timer.isActive()) {
      std::cerr << "auto run timer starting\n";
      _auto_run_timer.start();
    }
  } else {
    if (_auto_run_timer.isActive()) {
      std::cerr << "auto run timer stopping\n";
      _auto_run_timer.stop();
    }
  }
}

void NodeGraphManipulatorImp::on_auto_run_changed(bool r) {
  update_auto_run_timer();
}

void NodeGraphManipulatorImp::on_auto_run_interval_changed(int i) {
  update_auto_run_timer();
}

void NodeGraphManipulatorImp::report_dl_usage() {
  _ng_view->report_dl_usage();
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

void NodeGraphManipulator::receive_message(const QString& msg) {
  _imp->receive_message(msg);
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

void NodeGraphManipulator::continue_cleaning_to_ultimate_targets_on_idle() {
  _imp->continue_cleaning_to_ultimate_targets_on_idle();
}

void NodeGraphManipulator::continue_cleaning_to_ultimate_targets() {
  _imp->continue_cleaning_to_ultimate_targets();
}

bool NodeGraphManipulator::is_busy_cleaning() {
  return _imp->is_busy_cleaning();
}

bool NodeGraphManipulator::current_task_is_cancelable() {
  return _imp->current_task_is_cancelable();
}

bool NodeGraphManipulator::is_waiting_for_response() {
  return _imp->is_waiting_for_response();
}

void NodeGraphManipulator::force_stack_reset() {
  return _imp->force_stack_reset();
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

void NodeGraphManipulator::enter_group_prep(Entity* group) {
  _imp->enter_group_prep(group);
}

void NodeGraphManipulator::exit_group_prep(Entity* group) {
  _imp->exit_group_prep(group);
}

void NodeGraphManipulator::dive_into_group(const std::string& child_group_name) {
  _imp->dive_into_group(child_group_name);
}

void NodeGraphManipulator::surface_from_group() {
  _imp->surface_from_group();
}

void NodeGraphManipulator::link_to_selected_node(Entity* downstream_node) {
  return _imp->link_to_selected_node(downstream_node);
}

Entity* NodeGraphManipulator::create_link(Entity* input_entity, Entity* output_entity) {
  return _imp->create_link(input_entity, output_entity);
}

void NodeGraphManipulator::set_input_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) {
  _imp->set_input_topology(entity, ordering);
}

void NodeGraphManipulator::set_output_topology(Entity* entity, const std::unordered_map<std::string, size_t>& ordering) {
  _imp->set_output_topology(entity, ordering);
}

Entity* NodeGraphManipulator::create_node(bool centered, EntityDID entity_did, const std::string& name, Entity* group_entity) {
  return _imp->create_node(centered, entity_did, name, group_entity);
}

Entity* NodeGraphManipulator::create_compute_node(bool centered, EntityDID entity_did, ComponentDID compute_did, const std::string& name, Entity* group_entity) {
  return _imp->create_compute_node(centered, entity_did, compute_did, name, group_entity);
}

Entity* NodeGraphManipulator::create_public_macro_node(bool centered, const std::string& macro_name, const std::string& name, Entity* group_entity) {
  // Name the node with the same name as the macro name.
  return _imp->create_public_macro_node(centered, macro_name, name, group_entity);
}

Entity* NodeGraphManipulator::create_private_macro_node(bool centered, const std::string& macro_name, const std::string& name, Entity* group_entity) {
  // Name the node with the same name as the macro name.
  return _imp->create_private_macro_node(centered, macro_name, name, group_entity);
}

Entity* NodeGraphManipulator::create_app_macro_node(bool centered, const std::string& macro_name, const std::string& name, Entity* group_entity) {
  // Name the node with the same name as the macro name.
  return _imp->create_app_macro_node(centered, macro_name, name, group_entity);
}

Entity* NodeGraphManipulator::create_password_input_node(bool centered, const QJsonValue& value, const std::string& name, Entity* group_entity) {
  return _imp->create_password_input_node(centered, value, name, group_entity);
}

Entity* NodeGraphManipulator::create_input_node(bool centered, const QJsonValue& value, const std::string& name, Entity* group_entity) {
  return _imp->create_input_node(centered, value, name, group_entity);
}

Entity* NodeGraphManipulator::create_password_data_node(bool centered, const QJsonValue& value, const std::string& name, Entity* group_entity) {
  return _imp->create_password_data_node(centered, value, name, group_entity);
}

Entity* NodeGraphManipulator::create_data_node(bool centered, const QJsonValue& value, const std::string& name, Entity* group_entity) {
  return _imp->create_data_node(centered, value, name, group_entity);
}

Entity* NodeGraphManipulator::create_browser_node(bool centered, ComponentDID compute_did, const QJsonObject& chain_state, const std::string& name, Entity* group_entity) {
  return _imp->create_browser_node(centered, compute_did, chain_state, name, group_entity);
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

void NodeGraphManipulator::copy_description_to_input_node(Entity* input_entity, Entity* input_node_entity) {
  return _imp->copy_description_to_input_node(input_entity, input_node_entity);
}

void NodeGraphManipulator::bubble_group_dirtiness(Entity* dirty_node_entity) {
  _imp->bubble_group_dirtiness(dirty_node_entity);
}

void NodeGraphManipulator::set_mqtt_override(const Path& node_path, const QString& topic, const QString& payload) {
  _imp->set_mqtt_override(node_path, topic, payload);
}

void NodeGraphManipulator::send_post_value_signal(int post_type, const QString& title, const QJsonObject& obj) {
  _imp->send_post_value_signal(post_type, title, obj);
}

void NodeGraphManipulator::report_dl_usage() {
  _imp->report_dl_usage();
}

}
