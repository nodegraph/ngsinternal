#include <guicomponents/computes/entergroupcompute.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>


namespace ngs {

EnterGroupCompute::EnterGroupCompute(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _inputs(this) {
  get_dep_loader()->register_fixed_dep(_inputs, Path({"."}));
}

EnterGroupCompute::~EnterGroupCompute() {
}


// ----------------------------------------------------------------------------

ExitGroupCompute::ExitGroupCompute(Entity* entity, ComponentDID did):
  Compute(entity, did){
}

ExitGroupCompute::~ExitGroupCompute() {

}

}
