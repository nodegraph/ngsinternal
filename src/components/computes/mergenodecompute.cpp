#include <base/objectmodel/entity.h>
#include <components/computes/mergenodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/commtypes.h>

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
  create_input("layer", c);
  create_output("out", c);
}

const QJsonObject MergeNodeCompute::_hints = MergeNodeCompute::init_hints();
QJsonObject MergeNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "in", HintKey::kDescriptionHint, "This object will get data from the \"layer\" input merged into it.");
  add_hint(m, "layer", HintKey::kDescriptionHint, "The object will layer over the data from the \"in\" input.");

  return m;
}

bool MergeNodeCompute::update_state() {
  Compute::update_state();

  // This will hold our final output.
  QJsonValue output;

  // If our "in" input is connected, we merge that value in.
  {
    const Dep<InputCompute>& c = _inputs->get("in");
    if (c->is_connected()) {
      output = JSONUtils::deep_merge(output, c->get_output("out"));
    }
  }

  // If our "layer" input is connected, we merge that value in.
  {
    const Dep<InputCompute>& c = _inputs->get("layer");
    if (c->is_connected()) {
      output = JSONUtils::deep_merge(output, c->get_output("out"));
    }
  }

  set_output("out", output);
  return true;
}

}

