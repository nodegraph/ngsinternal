#include <base/objectmodel/deploader.h>

#include <components/computes/foreachgroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/loopdatanodecompute.h>
#include <components/computes/accumulatedatanodecompute.h>

#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>


namespace ngs {

ForEachGroupNodeCompute::ForEachGroupNodeCompute(Entity* entity, ComponentDID did):
    GroupNodeCompute(entity, did),
    _restart_loop(true) {
}

ForEachGroupNodeCompute::~ForEachGroupNodeCompute() {
}

void ForEachGroupNodeCompute::set_self_dirty(bool dirty) {
  GroupNodeCompute::set_self_dirty(dirty);
  _restart_loop = true;
}

void ForEachGroupNodeCompute::reset_accumulate_data_nodes() {
  // Reset all the Accumulate nodes directly inside this group.
  // Because of dirtiness propagation, this will also get called on all inner groups recursively.
  const Entity::NameToChildMap &children = our_entity()->get_children();
  for (auto &iter: children) {
    if (iter.second->get_did() == EntityDID::kAccumulateDataNodeEntity) {
      Dep<AccumulateDataNodeCompute> c = get_dep<AccumulateDataNodeCompute>(iter.second);
      c->clear_override();
    }
  }
}

bool ForEachGroupNodeCompute::update_state() {
  internal();
  Compute::update_state();

  // Get the elements.
  QJsonObject elements = _inputs->get_input_value("elements").toObject();

  if (elements.isEmpty()) {
    // If the "condition" input is false then we copy the value from "in" to "out".
    Entity* output_node = our_entity()->get_child("out");
    Dep<OutputNodeCompute> output_node_compute = get_dep<OutputNodeCompute>(output_node);
    set_output("out", output_node_compute->get_output("out"));

    Entity* outputs = get_entity(Path( { ".", "outputs" }));
    for (auto &iter : outputs->get_children()) {
      Entity* output_entity = iter.second;
      const std::string& output_name = output_entity->get_name();
      Entity* output_node = our_entity()->get_child(output_name);
      Dep<OutputNodeCompute> output_node_compute = get_dep<OutputNodeCompute>(output_node);
      if (output_name == "out") {
        // We copy the value from in to out.
        set_output("out", _inputs->get("in")->get_output("out"));
      } else {
        // We set the value to zero for all other outputs.
        set_output(output_name, 0);
      }
    }
    return true;
  }

  Entity* element_node = our_entity()->get_child("element");
  assert(element_node);
  Dep<LoopDataNodeCompute> element_compute = get_dep<LoopDataNodeCompute>(element_node);

  while (true) {
    if (_restart_loop) {
      // If we get then we're just starting our loop.
      // Reset all the accumulate data nodes.
      reset_accumulate_data_nodes();
      // Set the first element value.
      QJsonObject::iterator iter = elements.begin();
      QJsonObject element_value;
      element_value.insert(iter.key(), iter.value());
      element_compute->set_override(element_value);
      _restart_loop = false;
    } else {
      QJsonObject::iterator iter = elements.find(element_compute->get_override().toObject().begin().key());
      assert(iter != elements.end());
      ++iter;
      if (iter == elements.end()) {
        // Copy our output values from the inside to the outside.
        copy_output_nodes_to_outputs();
        _restart_loop = true;
        return true;
      }
      QJsonObject element_value;
      element_value.insert(iter.key(), iter.value());
      element_compute->set_override(element_value);
    }
    // Otherwise if the "condition" input is true then run the normal group compute.
    if (!GroupNodeCompute::update_state()) {
      return false;
    }
  }

  _restart_loop = true;
  return true;
}

}

