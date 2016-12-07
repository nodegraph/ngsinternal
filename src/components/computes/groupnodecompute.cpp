#include <base/objectmodel/entity.h>
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/entityids.h>
#include <entities/guientities.h>

#include <components/computes/groupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/accumulatedatanodecompute.h>

#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/linkshape.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

GroupNodeCompute::GroupNodeCompute(Entity* entity, ComponentDID did):
    Compute(entity, did),
    _factory(this) {
  get_dep_loader()->register_fixed_dep(_factory, Path());
}

GroupNodeCompute::~GroupNodeCompute() {
}

void GroupNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_namespace("links");
}

void GroupNodeCompute::add_param_hints(const std::string& name, const QJsonValue& param_hints) {
  _node_hints.insert(name.c_str(), param_hints);
}

void GroupNodeCompute::remove_param_hints(const std::string& name) {
  Compute::remove_hint(_node_hints, name);
}

void GroupNodeCompute::update_wires() {
  internal();

  // Make sure the inputs and outputs on this group match up
  // with the input and output nodes inside this group.
  Entity* inputs_space = get_entity(Path({".","inputs"}));
  Entity* outputs_space = get_entity(Path({".","outputs"}));

  std::unordered_set<std::string> exposed_inputs;
  std::unordered_set<std::string> exposed_outputs;

  // Make sure all the input/outputs nodes in this group are represented by inputs/outputs.
  const Entity::NameToChildMap& children = our_entity()->get_children();
  for (auto &iter : children) {
    const std::string& child_name = iter.first;
    Entity* node = iter.second;
    EntityDID did = node->get_did();
    if (did == EntityDID::kInputNodeEntity) {
      // Update the set of exposed_inputs.
      exposed_inputs.insert(child_name);
      // If we already have an input plug corresponding to the input node, then make sure the hints are synced up.
      if (!inputs_space->has_child_name(child_name)) {
        // Otherwise we create an input plug.
        InputEntity* input = static_cast<InputEntity*>(_factory->instance_entity(inputs_space, EntityDID::kInputEntity, child_name));
        EntityConfig config;
        config.expose_plug = true;
        input->create_internals(config);
        input->initialize_wires();
        // Grab the computes on the input and the input node inside the group.
        Dep<InputCompute> outer = get_dep<InputCompute>(input);
        Dep<InputNodeCompute> inner = get_dep<InputNodeCompute>(node);
        // Note an input node's input is not connected to anything and cleaning it won't start any asynchronous processed.
        inner->get_inputs()->clean_wires();
        inner->get_inputs()->clean_state();
        // Set the unconnected value;
        outer->set_unconnected_value(inner->get_inputs()->get_input_value("default_value"));
      }
      // Make sure the hints match.
      Entity* input_node_description = node->get_entity(Path({".","inputs","description"}));
      Dep<InputCompute> param = get_dep<InputCompute>(input_node_description);
      QJsonValue description = param->get_unconnected_value();

      Dep<InputNodeCompute> input_node = get_dep<InputNodeCompute>(node);
      QJsonObject param_hints = input_node->get_hints().value("default_value").toObject();
      param_hints.insert(QString::number(to_underlying(HintKey::kDescriptionHint)), description);
      add_param_hints(child_name, param_hints);
    } else if (did == EntityDID::kOutputNodeEntity) {
      // Update the set of exposed_outputs.
      exposed_outputs.insert(child_name);
      // If we already have an output plug corresponding to the output node, then continue.
      if (outputs_space->has_child_name(child_name)) {
        continue;
      }
      // Otherwise we create an output plug.
      OutputEntity* out = static_cast<OutputEntity*>(_factory->instance_entity(outputs_space, EntityDID::kOutputEntity, child_name));
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
      if (!exposed_inputs.count(child_name)) {
        _inputs_to_destroy.push_back(iter.second);
        remove_param_hints(child_name);
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

  // Now do the base Compute::update_wires().
  Compute::update_wires();

  // The update_wires method on components, is called basically in random order for a given entity.
  // However for group nodes, the inputs and outputs are created dynamically directly inside update_wires(),
  // so the inputs(flux) component depends on this component to have updated its wires first.
  // So we force the inputs component's update_wires call here.
  // Note that this is an edge case that breaks our architecture.
  _inputs->update_wires();
}

void GroupNodeCompute::propagate_dirtiness(Component* dirty_source) {
  Component::propagate_dirtiness(dirty_source);
}

void GroupNodeCompute::set_self_dirty(bool dirty) {
  Compute::set_self_dirty(dirty);
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
      if (input_node_compute->get_override() != input->get_output("out")) {
        input_node_compute->set_override(input->get_output("out"));
      }
    }
  }
}

void GroupNodeCompute::copy_output_nodes_to_outputs() {
  Entity* outputs = get_entity(Path({".","outputs"}));
  for (auto &iter: outputs->get_children()) {
    Entity* output_entity = iter.second;
    const std::string& output_name = output_entity->get_name();
    Entity* output_node = our_entity()->get_child(output_name);
    Dep<OutputNodeCompute> output_node_compute = get_dep<OutputNodeCompute>(output_node);
    set_output(output_name, output_node_compute->get_output("out"));
  }
}

void GroupNodeCompute::reset_accumulate_data_nodes() {
  // Reset all the Accumulate nodes directly inside this group.
  // Because of dirtiness propagation, this will also get called on all inner groups recursively.
  const Entity::NameToChildMap &children = our_entity()->get_children();
  for (auto &iter: children) {
    if (our_entity()->has_comp_with_did(ComponentIID::kICompute, ComponentDID::kAccumulateDataNodeCompute)) {
      Dep<AccumulateDataNodeCompute> c = get_dep<AccumulateDataNodeCompute>(iter.second);
      c->clear_override();
    }
    // Recurse into all group node types.
    EntityDID did = iter.second->get_did();
    if (did == EntityDID::kBaseGroupNodeEntity ||
        did == EntityDID::kGroupNodeEntity ||
        did == EntityDID::kScriptGroupNodeEntity ||
        did == EntityDID::kBrowserGroupNodeEntity ||
        did == EntityDID::kFirebaseGroupNodeEntity ||
        did == EntityDID::kMQTTGroupNodeEntity ||
        did == EntityDID::kForEachGroupNodeEntity ||
        did == EntityDID::kIfGroupNodeEntity) {
      Dep<GroupNodeCompute> c = get_dep<GroupNodeCompute>(iter.second);
      c->reset_accumulate_data_nodes();
    }
  }
}

bool GroupNodeCompute::update_state() {
  internal();
  Compute::update_state();

  // Bring input values from outside the group to the inside.
  copy_inputs_to_input_nodes();

  // Find all of our output entities.
  // For each one if there is an associated output node, we clean it and cache the result.
  Entity* outputs = get_entity(Path({".","outputs"}));
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

  // Close out the group.
  _manipulator->clean_exit_group(our_entity());
  return true;
}

}

