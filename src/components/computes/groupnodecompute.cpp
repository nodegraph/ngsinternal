#include <base/objectmodel/entity.h>
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/deploader.h>
#include <entities/entityids.h>
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

struct {
    bool operator()(const Dep<CompShape>& a, const Dep<CompShape>& b)
    {
        return a->get_pos().x < b->get_pos().x;
    }
} CompShapeCompare;

GroupNodeCompute::GroupNodeCompute(Entity* entity, ComponentDID did):
    Compute(entity, did),
    _factory(this) {
  get_dep_loader()->register_fixed_dep(_factory, Path({}));
}

GroupNodeCompute::~GroupNodeCompute() {
}

void GroupNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_namespace("links");
}

void GroupNodeCompute::update_wires() {
  internal();

  bool changed = false;

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
    Entity* child = iter.second;
    EntityDID did = child->get_did();
    if (did == EntityDID::kInputNodeEntity) {
      // Update the set of exposed_inputs.
      exposed_inputs.insert(child_name);
      // If we already have an input plug corresponding to the input node, then continue.
      if (inputs_space->has_child_name(child_name)) {
        continue;
      }
      // Otherwise we create an input plug.
      InputEntity* in = static_cast<InputEntity*>(_factory->instance_entity(inputs_space, child_name, EntityDID::kInputEntity));
      in->create_internals();
      in->set_exposed(true);
      in->initialize_wires();
      changed = true;
    } else if (did == EntityDID::kOutputNodeEntity) {
      // Update the set of exposed_outputs.
      exposed_outputs.insert(child_name);
      // If we already have an output plug corresponding to the output node, then continue.
      if (outputs_space->has_child_name(child_name)) {
        continue;
      }
      // Otherwise we create an output plug.
      OutputEntity* out = static_cast<OutputEntity*>(_factory->instance_entity(outputs_space, child_name, EntityDID::kOutputEntity));
      out->create_internals();
      out->set_exposed(true);
      out->initialize_wires();
      changed = true;
    }
  }

   // Now remove any inputs/outputs that no longer have associated nodes in this group.
  {
    std::vector<Entity*> _inputs_to_destroy;
    for (auto &iter : inputs_space->get_children()) {
      const std::string& child_name = iter.first;
      if (!exposed_inputs.count(child_name)) {
        _inputs_to_destroy.push_back(iter.second);
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
      changed = true;
    }
    for (Entity* e : _outputs_to_destroy) {
      // This can leave dangling link shapes, which are cleaned up by subsequest passes of update_hierarchy().
      delete_ff(e);
      changed = true;
    }
  }

  // Now do the base Compute::update_wires().
  Compute::update_wires();
}

void GroupNodeCompute::synchronize_internal_dirtiness() {
  // Update the group's dirtiness from our internals.
  dirty_group_from_internals();
  dirty_internals_from_group();
}

void GroupNodeCompute::dirty_internals_from_group() {
  if (is_state_dirty()) {
    dirty_all_node_computes();
  }
}

void GroupNodeCompute::dirty_group_from_internals() {
  // Dirty the computes on all nodes in this group.
  const Entity::NameToChildMap& children = our_entity()->get_children();
  for (auto &iter : children) {
    const std::string& child_name = iter.first;
    Entity* child = iter.second;
    EntityDID did = child->get_did();
    if (did != EntityDID::kBaseNamespaceEntity) {
      Dep<Compute> compute = get_dep<Compute>(child);
      if (compute->is_state_dirty()) {
        dirty_state();
        return;
      }
    }
  }
}

void GroupNodeCompute::dirty_all_node_computes() {
  // Dirty the computes on all nodes in this group.
  const Entity::NameToChildMap& children = our_entity()->get_children();
  for (auto &iter : children) {
    const std::string& child_name = iter.first;
    Entity* child = iter.second;
    EntityDID did = child->get_did();
    if (did != EntityDID::kBaseNamespaceEntity) {
      Dep<Compute> compute = get_dep<Compute>(child);
      if (compute) {
        compute->dirty_state();
      }
    }
  }
}

void GroupNodeCompute::dirty_input_node_computes() {
  // For each input on this group if there is an associated input node, we set data on the input node.
  for (auto &iter : _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    const std::string& input_name = input->get_name();
    // Find the input node inside this group with the same name as the input.
    Entity* input_node = our_entity()->get_child(input_name);
    // Make sure we have an input node.
    if (!input_node) {
      assert(false);
      continue;
    }
    // Dirty all the input compute nodes.
    Dep<InputNodeCompute> input_node_compute = get_dep<InputNodeCompute>(input_node);
    if (input_node_compute) {
      input_node_compute->dirty_state();
    }
  }
}

void GroupNodeCompute::propagate_dirtiness(Component* dirty_source) {
  Component::propagate_dirtiness(dirty_source);
}

void GroupNodeCompute::set_self_dirty(bool dirty) {
  Compute::set_self_dirty(dirty);
}

bool GroupNodeCompute::clean_inputs() {
  // Clean each input on this group node.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    if (!input->clean_state()) {
      return false;
    }
  }

  // Copy the input values onto the input nodes inside this group node.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    const std::string& input_name = input->get_name();
    // Find the input node inside this group with the same name as the input.
    Entity* input_node = our_entity()->get_child(input_name);
    // Make sure we have an input node.
    if (!input_node) {
      assert(false);
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
  return true;
}

bool GroupNodeCompute::update_state() {
  internal();
  Compute::update_state();

  // For each input on this group if there is an associated input node, we set data on the input node.
  for (auto &iter: _inputs->get_all()) {
    const Dep<InputCompute>& input = iter.second;
    const std::string& input_name = input->get_name();
    // Find the input node inside this group with the same name as the input.
    Entity* input_node = our_entity()->get_child(input_name);
    // Make sure we have an input node.
    if (!input_node) {
      assert(false);
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

  // Find all of our output entities.
  // For each one if there is an associated output node, we clean it and cache the result.
  Entity* outputs = get_entity(Path({".","outputs"}));
  for (auto &iter: outputs->get_children()) {
    Entity* output_entity = iter.second;
    const std::string& output_name = output_entity->get_name();
    // Find an output node in this group with the same name as the output.
    Entity* output_node = our_entity()->get_child(output_name);
    // Make sure we have an output node.
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
  for (auto &iter: outputs->get_children()) {
    Entity* output_entity = iter.second;
    const std::string& output_name = output_entity->get_name();
    Entity* output_node = our_entity()->get_child(output_name);
    Dep<OutputNodeCompute> output_node_compute = get_dep<OutputNodeCompute>(output_node);
    set_output(output_name, output_node_compute->get_output("out"));
  }

  return true;
}

}

