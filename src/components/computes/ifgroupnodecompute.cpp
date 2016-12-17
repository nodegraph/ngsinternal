#include <base/objectmodel/deploader.h>
#include <base/utils/stringutil.h>
#include <components/computes/jsonutils.h>

#include <components/computes/ifgroupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/outputnodecompute.h>

#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>


namespace ngs {

IfGroupNodeCompute::IfGroupNodeCompute(Entity* entity, ComponentDID did):
    GroupNodeCompute(entity, did) {
}

IfGroupNodeCompute::~IfGroupNodeCompute() {
}

bool IfGroupNodeCompute::update_state() {
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
  } else {
    // Otherwise we run the normal group compute.
    return GroupNodeCompute::update_state();
  }
  return true;
}

}

