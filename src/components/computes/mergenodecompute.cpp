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

  EntityConfig c = config;
  c.expose_plug = true;
  c.unconnected_value = QJsonObject();

  create_input("in", c);
  create_input("source", c);
  create_output("out", c);
}

const QJsonObject MergeNodeCompute::_hints = MergeNodeCompute::init_hints();
QJsonObject MergeNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "in", GUITypes::HintKey::DescriptionHint, "This object will get data from the \"source\" input merged into it.");
  add_hint(m, "source", GUITypes::HintKey::DescriptionHint, "This object will merge into the object from the \"in\" input.");

  return m;
}

bool MergeNodeCompute::update_state() {
  Compute::update_state();

  // This will hold our final output.
  QJsonObject output;

  // If our "in" input is connected, we merge that value in.
  QJsonObject in = _inputs->get_input_object("in");
  output = JSONUtils::deep_merge(output, in).toObject();

  // If our "source" input is connected, we merge that value in.
  QJsonObject src = _inputs->get_input_object("source");
  output = JSONUtils::deep_merge(output, src).toObject();

  set_output("out", output);
  return true;
}

}

