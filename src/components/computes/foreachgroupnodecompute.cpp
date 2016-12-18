#include <base/objectmodel/deploader.h>

#include <components/computes/jsonutils.h>
#include <components/computes/foreachgroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/loopdatanodecompute.h>
#include <components/computes/accumulatedatanodecompute.h>

#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

ForEachGroupNodeCompute::ForEachGroupNodeCompute(Entity* entity, ComponentDID did):
    GroupNodeCompute(entity, did),
    _infinite_counter(0),
    _restart_loop(true),
    _do_next(false) {
}

ForEachGroupNodeCompute::~ForEachGroupNodeCompute() {
}

void ForEachGroupNodeCompute::set_self_dirty(bool dirty) {
  GroupNodeCompute::set_self_dirty(dirty);
  _restart_loop = true;
}

bool ForEachGroupNodeCompute::update_state() {
  std::cerr << "ForEachGroupNodeCompute::update_state\n";

  internal();
  Compute::update_state();

  // Get the path to the elements in the in input.
  QString elements_path_string = _inputs->get("elements_path")->get_output("out").toString();
  Path elements_path(Path::split_string(elements_path_string.toStdString()));

  // Get the value of elements.
  QJsonObject in_obj = _inputs->get("in")->get_output("out").toObject();
  QJsonObject elements = JSONUtils::extract_value(in_obj, elements_path, QJsonObject()).toObject();

  if (elements.isEmpty()) {
    std::cerr << "elements is empty\n";
    // If the "condition" input is false then we copy the value from "in" to "out".
    // We set the value to zero for all other outputs.
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

  std::cerr << "elements has stuff inside\n";

  Entity* element_node = our_entity()->get_child("element");
  assert(element_node);
  Dep<LoopDataNodeCompute> element_compute = get_dep<LoopDataNodeCompute>(element_node);

  while (true) {
    if (_restart_loop) {
      // If we get then we're just starting our loop.
      // Reset all the accumulate data nodes.
      reset_accumulate_data_nodes();
      // Set the first element value.
      element_compute->set_override(QJsonObject());
      _restart_loop = false;
      _do_next = true;
    }

    // Set the inputs dirty when starting an iteration of the loop,
    // so that we can perform the compute again.
    if (_do_next) {

      // Set the inputs dirty.
      for (auto &iter : _inputs->get_all()) {
        // Name of input on the group.
        std::string input_name = iter.second->get_name();
        // Find the input compute inside the group.
        Entity* input_node = our_entity()->get_child(input_name);
        assert(input_node);
        Dep<Compute> input_compute = get_dep<Compute>(input_node);
        input_compute->dirty_state();
      }
      _do_next = false;

      // Find the next element and set it as the override on the element node.
      QJsonObject::iterator iter;
      if (element_compute->get_override().toObject().empty()) {
        // On the first iteration we take the first element.
        iter = elements.begin();
      } else {
        // Otherwise we increment the iterator to get the next element.
        iter = elements.find(element_compute->get_override().toObject().begin().key());
        assert(iter != elements.end());
        ++iter;
      }

      // If after increment we reach the end, then we're done.
      if (iter == elements.end()) {
        // Copy our output values from the inside to the outside.
        copy_output_nodes_to_outputs();
        _restart_loop = true;
        _do_next = false;
        return true;
      }

      // Otherwise we set the element value onto the element node.
      QJsonObject element_value;
      element_value.insert(iter.key(), iter.value());
      element_compute->set_override(element_value);
      std::cerr << "looping over: " << iter.key().toStdString() << "\n";
    }

    std::cerr << "performing foreach compute\n";

    // Otherwise if the "condition" input is true then run the normal group compute.
    if (!GroupNodeCompute::update_state()) {
      return false;
    }

    // We return false once in a while so the user can stop infinite while loops.
    if ((_infinite_counter++ % 2) == 0) {
      _manipulator->continue_cleaning_to_ultimate_targets_on_idle();
      return false;
    }

    // If we've made it here we successfully go to the next element.
    // This logic is needed because some of the internal nodes may return false, in the
    // above GroupNodeCompute::update_state(). In this case we are not ready to move onto
    // the next element yet.
    _do_next = true;
  }

  _restart_loop = true;
  _do_next = false;
  return true;
}

}

