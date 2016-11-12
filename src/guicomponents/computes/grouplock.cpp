#include <guicomponents/computes/grouplock.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <base/objectmodel/dep.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/inputs.h>


namespace ngs {

GroupLock::GroupLock(Entity* entity, ComponentDID did)
    : Compute(entity, did),
      _inputs(this) {
  get_dep_loader()->register_fixed_dep(_inputs, Path({"."}));
}

GroupLock::~GroupLock() {
}

void GroupLock::set_self_dirty(bool dirty) {
  Component::set_self_dirty(dirty);
  if (_manipulator) {
      // We mark our surrounding group node as dirty/clean, as we are invisible.
      _manipulator->update_clean_marker(our_entity()->get_parent(), !is_state_dirty());
  }
}

bool GroupLock::update_state() {
  internal();
// Notify the gui side that a computation is now processing on the compute side.
  if (_manipulator) {
    // We mark our surrounding group node as being processed, as we are invisible.
    _manipulator->set_processing_node(our_entity()->get_parent());
  }
  return true;
}

}
