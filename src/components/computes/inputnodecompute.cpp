#include <base/objectmodel/entity.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/inputcompute.h>

namespace ngs {

// Input Nodes don't have input plugs, but they are associated
// with an input plug on the surrounding group.
// The InputComputer mainly passes these calls onto its parent group node.

InputNodeCompute::InputNodeCompute(Entity* entity):
    Compute(entity, kDID()){
}

InputNodeCompute::~InputNodeCompute() {
}

void InputNodeCompute::create_inputs_outputs() {
  external();
  Compute::create_inputs_outputs();
  create_input("data_type", 0, JSType::kNumber, false);
  create_input("expose_on_group", true, JSType::kBoolean, false);
  create_output("out");
}

const QVariantMap InputNodeCompute::_hints = InputNodeCompute::init_hints();
QVariantMap InputNodeCompute::init_hints() {
  QVariantMap m;

  add_hint(m, "data_type", HintType::kJSType, to_underlying(JSType::kNumber));
  add_hint(m, "data_type", HintType::kEnum, to_underlying(EnumHint::kJSType));
  add_hint(m, "data_type", HintType::kDescription, "The type of data that will pass through this node.");

  add_hint(m, "expose_on_group", HintType::kJSType, to_underlying(JSType::kBoolean));
  add_hint(m, "expose_on_group", HintType::kDescription, "Whether to expose this as an input plug on our encompassing group.");

  return m;
}


void InputNodeCompute::set_value(const QVariant& value) {
  external();
  set_output("out", value);
}

QVariant InputNodeCompute::get_value() const {
  return get_output("out");
}

bool InputNodeCompute::expose_on_group() const {
  const std::unordered_map<std::string, Dep<InputCompute> >& inputs = _inputs->get_all();
  if (inputs.count("expose_on_group")) {
	  return inputs.at("expose_on_group")->get_param_value().toBool();
  } else {
    std::cerr << "Error: The expose_on_group parameter is missing.\n";
    assert(false);
  }
  return false;
}

}

