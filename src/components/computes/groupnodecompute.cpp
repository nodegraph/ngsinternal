#include <base/objectmodel/entity.h>
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/deploader.h>
#include <entities/entityids.h>
#include <entities/guientities.h>
#include <base/objectmodel/lowerhierarchychange.h>

#include <components/computes/groupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>

#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/linkshape.h>

namespace ngs {

struct {
    bool operator()(const Dep<CompShape>& a, const Dep<CompShape>& b)
    {
        return a->get_pos().x < b->get_pos().x;
    }
} CompShapeCompare;

GroupNodeCompute::GroupNodeCompute(Entity* entity):
    Compute(entity, kDID()),
    _factory(this),
    _lower_change(this) {
  get_dep_loader()->register_fixed_dep(_factory, "");
  get_dep_loader()->register_fixed_dep(_lower_change, ".");
}

GroupNodeCompute::~GroupNodeCompute() {
}

HierarchyUpdate GroupNodeCompute::update_hierarchy() {
  bool hierarchy_changed = false;

  // Make sure the inputs and outputs on this group match up
  // with the input and output nodes inside this group.
  Entity* inputs_space = get_entity("./inputs");
  Entity* outputs_space = get_entity("./outputs");
  BaseEntityInstancer* ei = _factory->get_entity_instancer();

  // Make sure all the input/outputs nodes in this group are represented by inputs/outputs.
  const Entity::NameToChildMap& children = our_entity()->get_children();
  for (auto &iter : children) {
    const std::string& child_name = iter.first;
    Entity* child = iter.second;
    size_t did = child->get_did();
    if (did == kInputNodeEntity) {
      //InputNodeEntity* ine = static_cast<InputNodeEntity*>(child);
      if (inputs_space->has_child_name(child_name)) {
        continue;
      }
      InputEntity* in = static_cast<InputEntity*>(ei->instance(inputs_space, child_name, kInputEntity));
      in->create_internals();
      in->initialize_deps();
      hierarchy_changed = true;
    } else if (did == kOutputNodeEntity) {
      if (outputs_space->has_child_name(child_name)) {
        continue;
      }
      OutputEntity* out = static_cast<OutputEntity*>(ei->instance(outputs_space, child_name, kOutputEntity));
      out->create_internals();
      out->initialize_deps();
      hierarchy_changed = true;
    }
  }

   // Now remove any inputs/outputs that no longer have associated nodes in this group.
  {
    std::vector<Entity*> _inputs_to_destroy;
    for (auto &iter : inputs_space->get_children()) {
      const std::string& child_name = iter.first;
      if (!our_entity()->has_child_name(child_name)) {
        _inputs_to_destroy.push_back(iter.second);
      }
    }
    std::vector<Entity*> _outputs_to_destroy;
    for (auto &iter : outputs_space->get_children()) {
      const std::string& child_name = iter.first;
      if (!our_entity()->has_child_name(child_name)) {
        _outputs_to_destroy.push_back(iter.second);
      }
    }
    for (Entity* e : _inputs_to_destroy) {
      // This can leave dangling link shapes, which are cleaned up by subsequest passes of update_hierarchy().
      delete_ff(e);
      hierarchy_changed = true;
    }
    for (Entity* e : _outputs_to_destroy) {
      // This can leave dangling link shapes, which are cleaned up by subsequest passes of update_hierarchy().
      delete_ff(e);
      hierarchy_changed = true;
    }
  }

  if (hierarchy_changed) {
    return kChanged;
  }
  return kUnchanged;

}

void GroupNodeCompute::update_state() {
  Compute::update_state();

  if (!dep_is_dirty(_lower_change)) {
    return;
  }

  // For each input data if there is an associated input node, we set data on the input node
  // from the input data. The input data handles connections to other entities internally.
  for (auto &iter: _inputs) {
    Dep<InputCompute> op = iter.second;
    const std::string& input_name = op->our_entity()->get_name();
    const QVariant& input_value = op->get_result("out");
    // Find an entity in this group with the same name as the input data.
    Entity* e = our_entity()->get_child(input_name);
    // Make sure we have an entity.
    if (!e) {
      //assert(false);
      continue;
    }
    // If it has an input computer, copy the value from the input plug to the input computer.
    Dep<InputNodeCompute> input_computer = get_dep<InputNodeCompute>(e);
    if (input_computer) {
      input_computer->set_value(input_value);
    }
  }

  // Find all of our output data entities.
  // For each one if there is an associated output node, we clean it and cache the result.
  Entity* outputs = get_entity("./outputs");
  for (auto &iter: outputs->get_children()) {
    Entity* oe = iter.second;
    const std::string& output_name = oe->get_name();
    // Find an entity in this group with the same name as the output data.
    Entity* o = our_entity()->get_child(output_name);
    // Make sure we have an entity.
    if (!o) {
      //assert(false);
      continue;
    }
    // If it has an output computer, clean it.
    Dep<OutputNodeCompute> output_computer = get_dep<OutputNodeCompute>(o);
    if (output_computer) {
      output_computer->propagate_cleanliness();
    }
    // Copy the value from the output node to the output plug.
    set_result(output_name, output_computer->get_result("out"));
  }

}

}

