#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/entity.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/computes/compute.h>
#include <components/computes/dotnodecompute.h>
#include <components/computes/groupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/mocknodecompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/outputnodecompute.h>
#include <entities/factory.h>
#include <entities/nonguientities.h>
#include <guicomponents/computes/scriptnodecompute.h>

namespace ngs {

void BaseNamespaceEntity::create_internals() {
}

void BaseAppEntity::create_internals() {
  // Our components.
  new_ff Factory(this);
}

Entity* BaseNodeHelperEntity::add_namespace(Entity* parent, const std::string& name) {
  Entity* links = new_ff BaseNamespaceEntity(parent, name);
  links->create_internals();
  return links;
}

Entity* BaseNodeHelperEntity::add_input(Entity* parent, const std::string& name) {
  Entity* input = new_ff BaseInputEntity(parent, name);
  input->create_internals();
  return input;
}

Entity* BaseNodeHelperEntity::add_output(Entity* parent, const std::string& name) {
  Entity* output = new_ff BaseOutputEntity(parent, name);
  output->create_internals();
  return output;
}

void BaseNodeHelperEntity::create_internals() {
  _inputs = add_namespace(this, "inputs");
  _outputs = add_namespace(this, "outputs");
}

void BaseGroupNodeEntity::create_internals() {
  BaseNodeHelperEntity::create_internals();

  // Our components.
  new_ff GroupNodeCompute(this);

  // Our sub entities.
  add_namespace(this, "links");
}

void BaseLinkEntity::create_internals() {
}

void BaseDotNodeEntity::create_internals() {
  BaseNodeHelperEntity::create_internals();

  // Our components.
  new_ff DotNodeCompute(this);

  // Our sub entities.
  add_input(_inputs, "in");
  add_output(_outputs,"out");
}

void BaseInputNodeEntity::create_internals() {
  BaseNodeHelperEntity::create_internals();

  // Our components.
  new_ff InputNodeCompute(this);

  // Our sub entities.
  add_output(_outputs, "out");
}

void BaseOutputNodeEntity::create_internals() {
  BaseNodeHelperEntity::create_internals();

  // Our components.
  new_ff OutputNodeCompute(this);

  // Our sub entities.
  add_input(_inputs, "in");
}

void BaseMockNodeEntity::create_internals() {
  BaseNodeHelperEntity::create_internals();

  // Our components.
  new_ff MockNodeCompute(this);

  // Our sub entities.
  add_input(_inputs, "a");
  add_input(_inputs, "b");
  add_output(_outputs, "c");
  add_output(_outputs, "d");
}

void BaseScriptNodeEntity::create_internals() {
  BaseNodeHelperEntity::create_internals();

  // Our components.
  Compute* dc = new_ff ScriptNodeCompute(this);

  // Our sub entities.
  add_input(_inputs, "in");
  add_output(_outputs, "out");
}

void BaseInputEntity::create_internals() {
  // Our components.
  new_ff InputCompute(this);
}

void BaseInputEntity::set_param_type(ParamType param_type) {
  get<InputCompute>()->set_param_type(param_type);
}

void BaseOutputEntity::create_internals() {
  // Our components.
  new_ff OutputCompute(this);
}

}
