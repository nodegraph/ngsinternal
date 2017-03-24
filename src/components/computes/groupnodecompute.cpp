#include <base/objectmodel/entity.h>
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/entityids.h>
#include <entities/guientities.h>

#include <components/computes/groupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>

#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/linkshape.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

GroupNodeCompute::WireUpdater::WireUpdater(GroupNodeCompute* target):
    Component(NULL, ComponentIID::kIInvalidComponent, ComponentDID::kInvalidComponent),
    _target(target){}
void GroupNodeCompute::WireUpdater::update_wires() {
  // Make sure the inputs and outputs on this group match up
  // with the input and output nodes inside this group.
  Entity* inputs_space = _target->get_entity(Path({".",kInputsFolderName}));
  Entity* outputs_space = _target->get_entity(Path({".",kOutputsFolderName}));

  std::unordered_set<std::string> exposed_inputs;
  std::unordered_set<std::string> exposed_outputs;

  // Make sure all the input/outputs nodes in this group are represented by inputs/outputs.
  const Entity::NameToChildMap& children = _target->our_entity()->get_children();
  for (auto &iter : children) {
    const std::string& child_name = iter.first;
    Entity* node = iter.second;
    EntityDID did = node->get_did();
    if (did == EntityDID::kInputNodeEntity || did == EntityDID::kPasswordInputNodeEntity) {
      // Update the set of exposed_inputs.
      exposed_inputs.insert(child_name);
      // If we already have an input plug corresponding to the input node, then make sure the hints are synced up.
      if (!inputs_space->has_child_name(child_name)) {
        // Otherwise we create an input plug.
        InputEntity* input = static_cast<InputEntity*>(_target->_factory->instance_entity(inputs_space, EntityDID::kInputEntity, child_name));
        EntityConfig config;
        if (child_name == kMainInputNodeName) {
          config.expose_plug = true;
        } else {
          config.expose_plug = false;
        }
        input->create_internals(config);
        input->initialize_wires();
        // Grab the computes on the input and the input node inside the group.
        Dep<InputCompute> outer = get_dep<InputCompute>(input);
        Dep<InputNodeCompute> inner = get_dep<InputNodeCompute>(node);
        // Note an input node's input is not connected to anything and cleaning it won't start any asynchronous processes.
        inner->get_inputs()->clean_wires();
        inner->get_inputs()->clean_state();
        // Skip setting the unconnected value, because this can be dangerous with passwords.
        // We encountered a case where the password was getting set on the root group,
        // and there is no way for the user to see or edit these param on the root group.
        outer->set_unconnected_value(inner->get_inputs()->get_unset_value("default_value"));
      }
      // Make sure the hints match.
      Entity* input_node_description = node->get_entity(Path({".",kInputsFolderName,"description"}));
      Dep<InputCompute> param = get_dep<InputCompute>(input_node_description);
      QJsonValue description = param->get_unconnected_value();

      Dep<InputNodeCompute> input_node = get_dep<InputNodeCompute>(node);
      QJsonObject param_hints = input_node->get_hints().value("default_value").toObject();
      param_hints.insert(QString::number(to_underlying(GUITypes::HintKey::DescriptionHint)), description);
      _target->add_param_hints(child_name, param_hints);
    } else if (did == EntityDID::kOutputNodeEntity) {
      // Update the set of exposed_outputs.
      exposed_outputs.insert(child_name);
      // If we already have an output plug corresponding to the output node, then continue.
      if (outputs_space->has_child_name(child_name)) {
        continue;
      }
      // Otherwise we create an output plug.
      OutputEntity* out = static_cast<OutputEntity*>(_target->_factory->instance_entity(outputs_space, EntityDID::kOutputEntity, child_name));
      EntityConfig config;
      config.expose_plug = true;
      out->create_internals(config);
      out->initialize_wires();
    }
  }

   // Now remove any inputs/outputs that no longer have associated nodes in this group.
  {
    std::vector<Entity*> _inputs_to_destroy;
    for (auto &iter : inputs_space->get_children()) {
      const std::string& child_name = iter.first;
      if (_target->get_fixed_inputs().count(child_name)) {
        continue;
      }
      if (!exposed_inputs.count(child_name)) {
        _inputs_to_destroy.push_back(iter.second);
        _target->remove_param_hints(child_name);
      }
    }
    std::vector<Entity*> _outputs_to_destroy;
    for (auto &iter : outputs_space->get_children()) {
      const std::string& child_name = iter.first;
      if (!exposed_outputs.count(child_name)) {
        _outputs_to_destroy.push_back(iter.second);
      }
    }
    for (Entity* e : _inputs_to_destroy) {
      // This can leave dangling link shapes, which are cleaned up by subsequest passes of update_hierarchy().
      delete_ff(e);
    }
    for (Entity* e : _outputs_to_destroy) {
      // This can leave dangling link shapes, which are cleaned up by subsequest passes of update_hierarchy().
      delete_ff(e);
    }
  }
}

void GroupNodeCompute::WireUpdater::revert_params_to_defaults() {
  // Copy the default value of input nodes to the groups inputs.
  Entity* inputs_space = _target->get_entity(Path({".",kInputsFolderName}));
  const Entity::NameToChildMap& params = inputs_space->get_children();
  const Entity::NameToChildMap& nodes = _target->our_entity()->get_children();

  for (auto &input_entity: params) {
    // Outer is the param on the group node.
    Dep<InputCompute> outer = get_dep<InputCompute>(input_entity.second);
    std::string name = input_entity.first;
    // Inner is our input node inside the group.
    Entity* node_entity = nodes.at(name);
    assert(node_entity);
    Dep<InputNodeCompute> inner = get_dep<InputNodeCompute>(node_entity);
    // Inner param is the default value param on the inner node.
    Dep<InputCompute> inner_param = inner->get_inputs()->get("default_value");
    // We clean it. This won't start any asynchronous processes as it's the top level input node in the group.
    inner_param->clean_state();
    QJsonValue inner_value = inner_param->get_main_output();
    outer->set_unconnected_value(inner_value);
  }
}

GroupNodeCompute::GroupNodeCompute(Entity* entity, ComponentDID did):
    Compute(entity, did),
    _factory(this),
    _wire_updater(new_ff WireUpdater(this)) {
  get_dep_loader()->register_fixed_dep(_factory, Path());
}

GroupNodeCompute::~GroupNodeCompute() {
  delete_ff(_wire_updater);
}

void GroupNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_namespace(kLinksFolderName);
}

const std::unordered_set<std::string>& GroupNodeCompute::get_fixed_inputs() const {
  static const std::unordered_set<std::string> dummy;
  return dummy;
}

void GroupNodeCompute::add_param_hints(const std::string& name, const QJsonValue& param_hints) {
  if (_node_hints.contains(name.c_str()) && (_node_hints[name.c_str()] == param_hints) ) {
    return;
  }
  // Only dirty this component if we actually add something.
  external();
  _node_hints.insert(name.c_str(), param_hints);
}

void GroupNodeCompute::remove_param_hints(const std::string& name) {
  if (!_node_hints.contains(name.c_str())) {
    return;
  }
  // Only dirty this component if we actually remove something.
  external();
  Compute::remove_hint(_node_hints, name);
}

void GroupNodeCompute::revert_params_to_defaults() {
  _wire_updater->revert_params_to_defaults();
}

void GroupNodeCompute::dirty_all_nodes_in_group() {
  external();
  const Entity::NameToChildMap& children = our_entity()->get_children();
  for (auto &iter : children) {
    Dep<Compute> compute = get_dep<Compute>(iter.second);
    if (compute) {
      compute->dirty_state();
    }
  }
}

void GroupNodeCompute::dirty_all_nodes_in_group_recursively() {
  external();
  const Entity::NameToChildMap& children = our_entity()->get_children();
  for (auto &iter : children) {
    // If we have a group, we recurse.
    if (iter.second->has_comp_with_did(ComponentIID::kIGroupInteraction, ComponentDID::kGroupInteraction)) {
      Dep<GroupNodeCompute> compute = get_dep<GroupNodeCompute>(iter.second);
      assert(compute);
      compute->dirty_state();
      compute->dirty_all_nodes_in_group_recursively();
    } else {
      Dep<Compute> compute = get_dep<Compute>(iter.second);
      if (compute) {
        compute->dirty_state();
      }
    }
  }
}

void GroupNodeCompute::update_wires() {
  internal();
  _wire_updater->update_wires();

  // The update_wires method on components, is called basically in random order for a given entity.
  // However for group nodes, the inputs and outputs are created dynamically directly inside update_wires(),
  // so the inputs(flux) component depends on this component to have updated its wires first.
  // So we force the inputs component's update_wires call here.
  // Note that this is an edge case that breaks our architecture.
  _inputs->update_wires();
}

void GroupNodeCompute::reset_dirty_state_on_all_nodes_in_group() {
  const Entity::NameToChildMap& children = our_entity()->get_children();
  for (auto &iter : children) {
    Dep<Compute> compute = get_dep<Compute>(iter.second);
    if (compute) {
      compute->reset_dirty_state();
    }
  }
}

void GroupNodeCompute::reset_dirty_state_on_output_nodes() {
  const Entity::NameToChildMap& children = our_entity()->get_children();
  for (auto &iter : children) {
    Dep<OutputNodeCompute> compute = get_dep<OutputNodeCompute>(iter.second);
    if (compute) {
      compute->reset_dirty_state();
    }
  }
}

void GroupNodeCompute::init_dirty_state() {
  dirty_input_nodes();
  reset_dirty_state_on_output_nodes();
}

// Note this method is called to make sure the inputs to the group we are currently inside is clean.
// It gets called recursively starting at the root. Note that is not called on for any group nodes
// inside the current group. For those groups the inputs get automatically cleaned via clean propagation
// through dependencies. So node that the call to copy_inputs_to_input_nodes must be performed by both
// by this method and the update_state() method.
bool GroupNodeCompute::clean_inputs() {
  // Clean each input on this group node.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    if (!input->clean_state()) {
      return false;
    }
  }
  copy_inputs_to_input_nodes();
  return true;
}

bool GroupNodeCompute::dirty_input_nodes() {
  // Dirty each input on this group node.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    const std::string& input_name = input->get_name();
    // Try to find the input node inside this group with the same name as the input.
    Entity* input_node = our_entity()->get_child(input_name);
    if (!input_node) {
      continue;
    }
    Dep<InputNodeCompute> input_node_compute = get_dep<InputNodeCompute>(input_node);
    input_node_compute->dirty_state();
  }
  return true;
}

void GroupNodeCompute::copy_inputs_to_input_nodes() {
  // Copy the input values onto the input nodes inside this group node.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    const std::string& input_name = input->get_name();
    // Try to find the input node inside this group with the same name as the input.
    Entity* input_node = our_entity()->get_child(input_name);
    // Not all the inputs on a group are associated with input nodes inside the group.
    // Some are just params directly on the group.
    if (!input_node) {
      continue;
    }
    // Copy the input value to the input node, but only if they're different,
    // because this compute will get called multiple times as part of asynchronous updating
    // especially when the group contains asynchronous web action nodes.
    Dep<InputNodeCompute> input_node_compute = get_dep<InputNodeCompute>(input_node);
    if (input_node_compute) {
      if (input_node_compute->get_override() != input->get_main_output()) {
        input_node_compute->set_override(input->get_main_output());
      }
    }
  }
}

void GroupNodeCompute::copy_output_nodes_to_outputs() {
  Entity* outputs = get_entity(Path({".",kOutputsFolderName}));
  for (auto &iter: outputs->get_children()) {
    Entity* output_entity = iter.second;
    const std::string& output_name = output_entity->get_name();
    Entity* output_node = our_entity()->get_child(output_name);
    Dep<OutputNodeCompute> output_node_compute = get_dep<OutputNodeCompute>(output_node);
    set_output(output_name, output_node_compute->get_main_output());
  }
}

bool GroupNodeCompute::update_state() {
  internal();
  Compute::update_state();

  // Bring input values from outside the group to the inside.
  copy_inputs_to_input_nodes();

  // Find all of our output entities.
  // For each one if there is an associated output node, we clean it and cache the result.
  Entity* outputs = get_entity(Path({".",kOutputsFolderName}));
  for (auto &iter: outputs->get_children()) {
    Entity* output_entity = iter.second;
    const std::string& output_name = output_entity->get_name();
    // Find an output node in this group with the same name as the output.
    Entity* output_node = our_entity()->get_child(output_name);
    // Make sure we have an output node.
    // All outputs on a group should have an associated output node inside the group at the moment.
    if (!output_node) {
      assert(false);
      continue;
    }
    // If the output node compute is dirty, then clean it.
    Dep<OutputNodeCompute> output_node_compute = get_dep<OutputNodeCompute>(output_node);
    if (output_node_compute->is_state_dirty()) {
      if (!output_node_compute->propagate_cleanliness()) {
        return false;
      }
    }
  }

  // If we get here then all of our internal computes have finished.
  copy_output_nodes_to_outputs();

  // Run our exit node if needed.
  _manipulator->exit_group_prep(our_entity());
  return true;
}

}

