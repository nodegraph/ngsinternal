#include <base/objectmodel/entity.h>
#include <components/computes/mergenodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

MergeNodeCompute::MergeNodeCompute(Entity* entity):
    Compute(entity, kDID()) {
}

MergeNodeCompute::~MergeNodeCompute() {
}

void MergeNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
  create_main_output(config);

  EntityConfig c = config;
  c.expose_plug = true;
  c.unconnected_value = QJsonObject();
  create_input("source", c);
}

const QJsonObject MergeNodeCompute::_hints = MergeNodeCompute::init_hints();
QJsonObject MergeNodeCompute::init_hints() {
  QJsonObject m;
  add_main_input_hint(m);
  add_hint(m, "source", GUITypes::HintKey::DescriptionHint, "The object to merge into the main input object.");

  return m;
}

bool MergeNodeCompute::update_state() {
  Compute::update_state();

  // This will hold our final output.
  QJsonObject output;

  // Merge in our main input.
  QJsonObject in = _inputs->get_main_input_object();
  output = JSONUtils::deep_merge(output, in).toObject();

  // Merge in our source input.
  QJsonObject src = _inputs->get_input_object("source");
  output = JSONUtils::deep_merge(output, src).toObject();

  set_main_output(output);
  return true;
}

}

