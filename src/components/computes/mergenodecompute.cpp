#include <base/objectmodel/entity.h>
#include <components/computes/mergenodecompute.h>
#include <components/computes/inputcompute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

namespace ngs {

MergeNodeCompute::MergeNodeCompute(Entity* entity):
    Compute(entity, kDID()) {
}

MergeNodeCompute::~MergeNodeCompute() {
}

void MergeNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QJsonObject());
  create_input("layer", QJsonObject());
  create_output("out");
}

const QJsonObject MergeNodeCompute::_hints = MergeNodeCompute::init_hints();
QJsonObject MergeNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "This object will get data from the \"layer\" input merged into it.");


  add_hint(m, "layer", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "layer", HintType::kDescription, "The object will layer over the data from the \"in\" input.");

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
      output = deep_merge(output, c->get_output("out"));
    }
  }

  // If our "layer" input is connected, we merge that value in.
  {
    const Dep<InputCompute>& c = _inputs->get("layer");
    if (c->is_connected()) {
      output = deep_merge(output, c->get_output("out"));
    }
  }

  set_output("out", output);
  return true;
}

}
