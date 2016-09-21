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

void BaseGroupNodeEntity::create_internals() {
  // Our components.
  new_ff GroupNodeCompute(this);
  // Our sub entities.
  {
    Entity* links = new_ff BaseNamespaceEntity(this, "links");
    links->create_internals();
  }
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
  }
}

void BaseLinkEntity::create_internals() {
}

void BaseDotNodeEntity::create_internals() {
  // Our components.
  new_ff DotNodeCompute(this);
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    Entity* input = new_ff BaseInputEntity(inputs, "in");
    input->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    Entity* output = new_ff BaseOutputEntity(outputs,"out");
    output->create_internals();
  }
}

void BaseInputNodeEntity::create_internals() {
  // Our components.
  new_ff InputNodeCompute(this);
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    Entity* output = new_ff BaseOutputEntity(outputs, "out");
    output->create_internals();
  }
}

void BaseOutputNodeEntity::create_internals() {
  // Our components.
  new_ff OutputNodeCompute(this);
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    Entity* input = new_ff BaseInputEntity(inputs, "in");
    input->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
  }
}

void BaseMockNodeEntity::create_internals() {
  // Our components.
  new_ff MockNodeCompute(this);
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    Entity* in_a = new_ff BaseInputEntity(inputs, "a");
    in_a->create_internals();
    Entity* in_b = new_ff BaseInputEntity(inputs, "b");
    in_b->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    Entity* out_c = new_ff BaseOutputEntity(outputs, "c");
    out_c->create_internals();
    Entity* out_d = new_ff BaseOutputEntity(outputs, "d");
    out_d->create_internals();
  }
}

void BaseComputeNodeEntity::create_internals() {
  // Our components.
  Compute* dc = new_ff ScriptNodeCompute(this);

  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    Entity* in = new_ff BaseInputEntity(inputs, "in");
    in->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    Entity* out = new_ff BaseOutputEntity(outputs, "out");
    out->create_internals();
  }
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
