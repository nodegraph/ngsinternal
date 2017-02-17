#include <base/objectmodel/deploader.h>

#include <components/computes/jsonutils.h>

#include <components/computes/whilegroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/scriptloopcontext.h>

#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

WhileGroupNodeCompute::WhileGroupNodeCompute(Entity* entity, ComponentDID did):
    GroupNodeCompute(entity, did),
    _infinite_counter(0),
    _restart_loop(true),
    _do_next(false){
}

WhileGroupNodeCompute::~WhileGroupNodeCompute() {
}

void WhileGroupNodeCompute::set_self_dirty(bool dirty) {
  GroupNodeCompute::set_self_dirty(dirty);
  _restart_loop = true;
}

void WhileGroupNodeCompute::reset_loop_context() {
  // Reset our script loop context.
  Dep<ScriptLoopContext> c = get_dep<ScriptLoopContext>(Path({"."}));
  c->reset();

  // Reset the script loop context on any child group nodes.
  const Entity::NameToChildMap &children = our_entity()->get_children();
  for (auto &iter: children) {
    if (iter.second->get_did() == EntityDID::kWhileGroupNodeEntity) {
      Dep<WhileGroupNodeCompute> w = get_dep<WhileGroupNodeCompute>(iter.second);
      w->reset_loop_context();
    }
  }
}

bool WhileGroupNodeCompute::update_state() {
  internal();
  Compute::update_state();

  // Get the path to the condition in the in input.
  QString condition_path_string = _inputs->get_input_string("condition_path");
  Path condition_path(Path::split_string(condition_path_string.toStdString()));

  // Get the value of the condition.
  QJsonObject in_obj = _inputs->get_input_object("in");
  QJsonValue condition_value = JSONUtils::extract_value(in_obj, condition_path, false);

  // If the "condition" input is false then we copy the value from "in" to "out".
  // We set the value to zero for all other outputs.
  if (!condition_value.toBool()) {
    Entity* outputs = get_entity(Path( { ".", kOutputsFolderName }));
    for (auto &iter : outputs->get_children()) {
      Entity* output_entity = iter.second;
      const std::string& output_name = output_entity->get_name();
      Entity* output_node = our_entity()->get_child(output_name);
      Dep<OutputNodeCompute> output_node_compute = get_dep<OutputNodeCompute>(output_node);
      if (output_name == "out") {
        // We copy the value from in to out.
        set_output("out", _inputs->get_input_object("in"));
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
      reset_loop_context();
      _restart_loop = false;
      _do_next = true;
    }

    // Set the inputs dirty when starting an iteration of the loop,
    // so that we can perform the compute again.
    if (_do_next) {
      for (auto &iter: _inputs->get_all()) {
        // Name of input on the group.
        std::string input_name = iter.second->get_name();
        // Find the input compute inside the group.
        Entity* input_node = our_entity()->get_child(input_name);
        assert(input_node);
        Dep<Compute> input_compute = get_dep<Compute>(input_node);
        input_compute->dirty_state();
      }
      _do_next = false;
    }

    // Run the regular group compute.
    if (!GroupNodeCompute::update_state()) {
      return false;
    }

    // Check the "out" value to see if the value at condition_path is false.
    QJsonObject out_obj = get_output("out").toObject();
    QJsonValue condition_value = JSONUtils::extract_value(out_obj, condition_path, false);
    if (!condition_value.toBool()) {
      break;
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

