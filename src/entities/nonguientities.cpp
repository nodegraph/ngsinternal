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

void BaseNamespaceEntity::create_internals(const std::vector<size_t>& ids) {
}

void BaseAppEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  new_ff Factory(this);
}

void BaseGroupNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff GroupNodeCompute(this))->create_inputs_outputs();
}

void BaseLinkEntity::create_internals(const std::vector<size_t>& ids) {
}

void BaseDotNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff DotNodeCompute(this))->create_inputs_outputs();
}

void BaseInputNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff InputNodeCompute(this))->create_inputs_outputs();
}

void BaseOutputNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff OutputNodeCompute(this))->create_inputs_outputs();
}

void BaseMockNodeEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  (new_ff MockNodeCompute(this))->create_inputs_outputs();
}

//void BaseScriptNodeEntity::create_internals(const std::vector<size_t>& ids) {
//  // Our components.
//  (new_ff ScriptNodeCompute(this))->create_inputs_outputs();
//}

void BaseInputEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  new_ff InputCompute(this);
}

void BaseInputEntity::set_param_type(JSType param_type) {
  get<InputCompute>()->set_type(param_type);
}

void BaseOutputEntity::create_internals(const std::vector<size_t>& ids) {
  // Our components.
  new_ff OutputCompute(this);
}

}
