#include <base/objectmodel/entity.h>
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/deploader.h>
#include <entities/entityids.h>
#include <base/objectmodel/lowerhierarchychange.h>

#include <components/computes/groupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>

#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/linkshape.h>

namespace ngs {

GroupNodeCompute::GroupNodeCompute(Entity* entity):
    Compute(entity, kDID()) {
}

GroupNodeCompute::~GroupNodeCompute() {
}


void GroupNodeCompute::update_state() {

  // Connect to our inputs.
  if (dep_is_dirty(_lower_change)) {
    // Clear our old inputs.
    _inputs.clear();

    // Connect to our input plugs.
    const Entity::NameToChildMap& children = get_entity("./inputs")->get_children();
    for (auto &iter: children) {
      Dep<InputCompute> dep = get_dep<InputCompute>(iter.second);
      if (dep) {
        _inputs.insert({iter.first,dep});
      }
      // Because we just added a new connection in our cleaning state method,
      // we need to make sure that it is clean.
      dep->clean();
    }
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

