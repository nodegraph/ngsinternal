#include <base/objectmodel/deploader.h>

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

  // If the "condition" input is false then we copy the value from "in" to "out".
  if (!_inputs->get("condition")->get_output("out").toBool()) {
    std::cerr << "running false logic of if\n";
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
  } else {
    std::cerr << "running true logic of if\n";
    // Otherwise if the "condition" input is true then run the normal group compute.
    return GroupNodeCompute::update_state();
  }
  return true;
}

}

