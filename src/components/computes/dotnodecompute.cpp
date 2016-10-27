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
  create_input("in", QJSValue());
  create_output("out");
}

const QJSValue DotNodeCompute::_hints = DotNodeCompute::init_hints();
QJSValue DotNodeCompute::init_hints() {
  QJSValue m;

  add_hint(m, "in", HintType::kJSType, to_underlying(JSType::kObject));
  add_hint(m, "in", HintType::kDescription, "The main object that flows through this node. This cannot be set manually.");

  return m;
}

bool DotNodeCompute::update_state() {
  internal();
  Compute::update_state();
  set_output("out", get_input_value("in"));
  return true;
}

}
