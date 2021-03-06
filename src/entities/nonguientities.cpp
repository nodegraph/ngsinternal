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
//#include <guicomponents/computes/scriptnodecompute.h>

namespace ngs {

void BaseNamespaceEntity::create_internals(const EntityConfig& config) {
}

void BaseAppEntity::create_internals(const EntityConfig& config) {
  // Our components.
  new_ff Factory(this);
}

void BaseGroupNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff GroupNodeCompute(this))->create_inputs_outputs(config);
}

void BaseLinkEntity::create_internals(const EntityConfig& config) {
}

void BaseDotNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff DotNodeCompute(this))->create_inputs_outputs(config);
}

void BaseInputNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff InputNodeCompute(this))->create_inputs_outputs(config);
}

void BaseOutputNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff OutputNodeCompute(this))->create_inputs_outputs(config);
}

void BaseMockNodeEntity::create_internals(const EntityConfig& config) {
  // Our components.
  (new_ff MockNodeCompute(this))->create_inputs_outputs(config);
}

//void BaseScriptNodeEntity::create_internals(const ConfigMap& config) {
//  // Our components.
//  (new_ff ScriptNodeCompute(this))->create_inputs_outputs(config);
//}

void BaseInputEntity::create_internals(const EntityConfig& config) {
  // Our components.
  new_ff InputCompute(this);
}

void BaseOutputEntity::create_internals(const EntityConfig& config) {
  // Our components.
  new_ff OutputCompute(this);
}

}
