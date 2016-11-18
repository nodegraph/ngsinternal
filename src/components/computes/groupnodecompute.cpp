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

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

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
    Entity* child = iter.second;
    EntityDID did = child->get_did();
    if (did == EntityDID::kInputNodeEntity) {
      // Update the set of exposed_inputs.
      exposed_inputs.insert(child_name);
      // Get our input entity.
      InputEntity* in = NULL;
      // If we already have an input plug corresponding to the input node, then continue.
      if (inputs_space->has_child_name(child_name)) {
        in = static_cast<InputEntity*>(inputs_space->get_child(child_name));
        // Grab the computes on the input and the input node inside the group.
        Dep<InputCompute> outer = get_dep<InputCompute>(in);
        Dep<InputNodeCompute> inner = get_dep<InputNodeCompute>(child);
        // Note we don't change the existing value on the input plug.
        // But we make sure the hints match.
        const QJsonObject& inner_node_hints = inner->get_hints();
        add_param_hints(child_name, inner_node_hints.value("default_value"));
      } else {
        // Otherwise we create an input plug.
        in = static_cast<InputEntity*>(_factory->instance_entity(inputs_space, EntityDID::kInputEntity, child_name));
        in->create_internals();
        in->set_exposed(true);
        in->initialize_wires();
        // Grab the computes on the input and the input node inside the group.
        Dep<InputCompute> outer = get_dep<InputCompute>(in);
        Dep<InputNodeCompute> inner = get_dep<InputNodeCompute>(child);
        // Note an input node's input is not connected to anything and cleaning it won't start any asynchronous processed.
        inner->get_inputs()->clean_wires();
        inner->get_inputs()->clean_state();
        // Set the unconnected value;
        outer->set_unconnected_value(inner->get_inputs()->get_input_value("default_value"));
        // Make sure the hints match.
        const QJsonObject& inner_node_hints = inner->get_hints();
        add_param_hints(child_name, inner_node_hints.value("default_value"));
      }
    } else if (did == EntityDID::kOutputNodeEntity) {
      // Update the set of exposed_outputs.
      exposed_outputs.insert(child_name);
      // If we already have an output plug corresponding to the output node, then continue.
      if (outputs_space->has_child_name(child_name)) {
        continue;
      }
      // Otherwise we create an output plug.
      OutputEntity* out = static_cast<OutputEntity*>(_factory->instance_entity(outputs_space, EntityDID::kOutputEntity, child_name));
      out->create_internals();
      out->set_exposed(true);
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
      QJsonArray arr;
      arr.push_back(input_node_compute->get_override());
      arr.push_back(input->get_output("out"));
      QJsonDocument doc;
      doc.setArray(arr);
      QByteArray data = doc.toJson();
      std::cerr << "input node override and outer input values: " << data.toStdString() << "\n";

      if (input_node_compute->get_override() != input->get_output("out")) {
        std::cerr << "setting override: " << input->get_output("out").toString().toStdString() << "\n";
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
      QJsonArray arr;
      arr.push_back(input_node_compute->get_override());
      arr.push_back(input->get_output("out"));
      QJsonDocument doc;
      doc.setArray(arr);
      QByteArray data = doc.toJson();
      std::cerr << "input node override and outer input values: " << data.toStdString() << "\n";


      if (input_node_compute->get_override() != input->get_output("out")) {
        std::cerr << "setting override: " << input->get_output("out").toString().toStdString() << "\n";
        input_node_compute->set_override(input->get_output("out"));
      }
      // We let the output nodes propagate cleanliness up to us when needed.
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

