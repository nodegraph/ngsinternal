#include <components/computes/dotnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>

#include <base/objectmodel/deploader.h>

namespace ngs {

DotNodeCompute::DotNodeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

DotNodeCompute::~DotNodeCompute() {
}

void DotNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("in", QVariantMap());
  create_output("out");
}

const QVariantMap DotNodeCompute::_hints = DotNodeCompute::init_hints();
QVariantMap DotNodeCompute::init_hints() {
  QVariantMap m;

  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");

  return m;
}

bool DotNodeCompute::update_state() {
  internal();
  Compute::update_state();
  set_output("out", _inputs->get_exposed().at("in")->get_output("out"));
  return true;
}

}
