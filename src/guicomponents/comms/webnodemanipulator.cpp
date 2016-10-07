#include <guicomponents/comms/webnodemanipulator.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/basefactory.h>

#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/baseoutputs.h>
#include <components/computes/baseinputs.h>

#include <components/compshapes/nodeshape.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/compshapecollective.h>

//#include <guicomponents/comms/webrecorder.h>
//#include <guicomponents/comms/webworker.h>
//#include <guicomponents/comms/taskscheduler.h>
//#include <guicomponents/comms/filemodel.h>


namespace ngs {

WebNodeManipulator::WebNodeManipulator(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _factory(this),
      _node(NULL),
      _compute(NULL) {
  _factory = get_dep<BaseFactory>(get_app_root());
}

void WebNodeManipulator::build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state) {
  build_compute_node(compute_did, chain_state);
  link(_node);
}

void WebNodeManipulator::build_compute_node(ComponentDID compute_did, const QVariantMap& chain_state) {
  // Create the node.
  Entity* group = _factory->get_current_group();
  std::pair<Entity*, Compute*> internals = _factory->create_compute_node2(group, compute_did);
  _node = internals.first;
  _compute = internals.second;

  // Initialize and update the wires.
  _node->initialize_wires();
  group->clean_wires();

  // Set the values on all the inputs from the chain_state.
  QVariantMap::const_iterator iter;
  for (iter = chain_state.begin(); iter != chain_state.end(); ++iter) {
    // Find the input entity.
    Path path( { ".", "inputs" });
    path.push_back(iter.key().toStdString());
    Entity* input_entity = _node->has_entity(path);
    // Skip this key if the entity doesn't exist.
    if (!input_entity) {
      continue;
    }
    // Get the compute.
    Dep<InputCompute> compute = get_dep<InputCompute>(input_entity);
    // Skip inputs which are plugs and not params.
    if (compute->is_exposed()) {
      continue;
    }

    std::cerr << "setting name: " << iter.key().toStdString() << " value: " << iter.value().toString().toStdString() << " type: " << iter.value().type()
              << " usertype: " << iter.value().userType() << "\n";
    compute->set_value(iter.value());
  }
}

void WebNodeManipulator::link(Entity* downstream) {
  // Get the factory.
  Dep<BaseFactory> factory = get_dep<BaseFactory>(get_app_root());
  Entity* current_group = factory->get_current_group();

  // Get the current comp shape collective.
  Dep<CompShapeCollective> collective = get_dep<CompShapeCollective>(current_group);

  // Find the lowest node that's where we'll link to.
  Dep<CompShape> upstream_node = collective->get_lowest( { downstream });
  if (!upstream_node) {
    return;
  }
  Entity* upstream = upstream_node->our_entity();

  // -------------------------------------------------------------------------
  // At this point we have both the upstream and downstream nodes to connect.
  // -------------------------------------------------------------------------

  // Position our node below the upstream.
  Dep<NodeShape> downstream_node = get_dep<NodeShape>(downstream);
  glm::vec2 pos = upstream_node->get_pos();
  pos.y -= 700;
  downstream_node->set_pos(pos);

  // Get the exposed outputs from the upstream node.
  Dep<BaseOutputs> outputs = get_dep<BaseOutputs>(upstream);
  const std::unordered_map<std::string, Entity*>& exposed_outputs = outputs->get_exposed();

  // Get the exposed inputs from the downstream node.
  Dep<BaseInputs> inputs = get_dep<BaseInputs>(downstream);
  const std::unordered_map<std::string, Entity*>& exposed_inputs = inputs->get_exposed();

  // If we have exposed outputs and inputs then lets connect the first of each of them.
  if (!exposed_outputs.empty() && !exposed_inputs.empty()) {
    Entity* output = exposed_outputs.begin()->second;
    Entity* input = exposed_inputs.begin()->second;

    // Link the computes.
    Dep<OutputCompute> output_compute = get_dep<OutputCompute>(output);
    Dep<InputCompute> input_compute = get_dep<InputCompute>(input);
    input_compute->link_output_compute(output_compute);

    // Create a link.
    Entity* links_folder = current_group->get_entity(Path( { ".", "links" }));
    Entity* link = factory->instance_entity(links_folder, "link", EntityDID::kLinkEntity);
    link->create_internals();

    // Link the link, input and output shapes.
    Dep<OutputShape> output_shape = get_dep<OutputShape>(output);
    Dep<InputShape> input_shape = get_dep<InputShape>(input);
    Dep<LinkShape> link_shape = get_dep<LinkShape>(link);
    link_shape->start_moving();
    link_shape->link_input_shape(input_shape);
    link_shape->link_output_shape(output_shape);
    link_shape->finished_moving();
  }

  // Clean the wires in this group, as new nodes and links were created.
  _factory->get_current_group()->clean_wires();
}

}
