#include <base/objectmodel/deploader.h>

#include <components/computes/jsonutils.h>

#include <components/computes/whilegroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/loopdatanodecompute.h>
#include <components/computes/accumulatedatanodecompute.h>

#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>


namespace ngs {

WhileGroupNodeCompute::WhileGroupNodeCompute(Entity* entity, ComponentDID did):
    GroupNodeCompute(entity, did),
    _infinite_counter(0),
    _restart_loop(true) {
}

WhileGroupNodeCompute::~WhileGroupNodeCompute() {
}

void WhileGroupNodeCompute::set_self_dirty(bool dirty) {
  GroupNodeCompute::set_self_dirty(dirty);
  _restart_loop = true;
}

bool WhileGroupNodeCompute::update_state() {
  internal();
  Compute::update_state();

  // Get the path to the condition in the in input.
  QString condition_path_string = _inputs->get("condition_path")->get_output("out").toString();
  Path condition_path(Path::split_string(condition_path_string.toStdString()));

  // Get the value of the condition.
  QJsonObject in_obj = _inputs->get("in")->get_output("out").toObject();
  QJsonValue condition_value = JSONUtils::extract_value(in_obj, condition_path, false);

  // If the "condition" input is false then we copy the value from "in" to "out".
  // We set the value to zero for all other outputs.
  if (!condition_value.toBool()) {
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

  while (true) {
    if (_restart_loop) {
      // If we get then we're just starting our loop.
      // Reset all the accumulate data nodes.
      reset_accumulate_data_nodes();
      _restart_loop = false;
    }
    // Run the regular group compute.
    if (!GroupNodeCompute::update_state()) {
      return false;
    } else {
      // Check the "out" value to see if the value at condition_path is false.
      QJsonObject out_obj = get_output("out").toObject();
      QJsonValue condition_value = JSONUtils::extract_value(out_obj, condition_path, false);
      if (!condition_value.toBool()) {
        break;
      }

      // We return false once in a while so the user can stop infinite while loops.
      if ((_infinite_counter++) % 2 == 0) {
        return false;
      }

      // Set the inputs dirty, so that we can perform the compute again.
      for (auto &iter: _inputs->get_all()) {
        const Dep<InputCompute>& input = iter.second;
        input->dirty_state();
      }
    }
  }

  _restart_loop = true;
  return true;
}

}

